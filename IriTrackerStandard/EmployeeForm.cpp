#include "EmployeeForm.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QDate>
#include <QFileDialog>
#include <QByteArray>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QBuffer>
#include "User.h"
#include "Department.h"
#include <QPixmap>
#include "IriTracker.h"
#include <QThread>
#include "DatabaseHelper.h"

EmployeeForm::EmployeeForm(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	iriTracker = new IriTracker();
	threadStream = new QThread();

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.btnOk, &QPushButton::clicked, this, &EmployeeForm::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &EmployeeForm::btnCancelClicked);
	connect(ui.btnBrowse, &QPushButton::clicked, this, &EmployeeForm::btnBrowseClicked);
	connect(ui.btnClearPhoto, &QPushButton::clicked, this, &EmployeeForm::btnClearPhotoClicked);

	ui.btnOk->setDisabled(true);

	ui.passwordEdit->setEchoMode(QLineEdit::Password);
	ui.confirmPasswordEdit->setEchoMode(QLineEdit::Password);

	connect(ui.idEdit, &QLineEdit::textChanged, this, &EmployeeForm::checkInputs);
	connect(ui.firstNameEdit, &QLineEdit::textChanged, this, &EmployeeForm::checkInputs);
	connect(ui.lastNameEdit, &QLineEdit::textChanged, this, &EmployeeForm::checkInputs);
	connect(ui.passwordCheckbox, &QCheckBox::toggled, this, &EmployeeForm::checkInputs);
	connect(ui.passwordEdit, &QLineEdit::textChanged, this, &EmployeeForm::checkInputs);
	connect(ui.confirmPasswordEdit, &QLineEdit::textChanged, this, &EmployeeForm::checkInputs);

	connect(ui.passwordCheckbox, &QCheckBox::stateChanged, this, &EmployeeForm::updatePasswordFields);

	connect(iriTracker, &IriTracker::onOpenDevice, this, &EmployeeForm::changeImageDevice, Qt::QueuedConnection);

	ui.rightIrisImgLabel->installEventFilter(this);
	ui.leftIrisImgLabel->installEventFilter(this);

	connect(iriTracker, &IriTracker::imageProcessed,
		this, &EmployeeForm::onImageProcessed);
}

EmployeeForm::~EmployeeForm()
{
	
}

void EmployeeForm::processStreaming() {
	// Tạo thread mới cho quá trình capture
	if (threadStream->isRunning()) {
		threadStream->quit();
	}

	// Kết nối tín hiệu từ IriTracker đến updateFrame trong UI thread
	connect(iriTracker, &IriTracker::imageProcessed, this, &EmployeeForm::onImageProcessed);
	connect(iriTracker, &IriTracker::imageResult, this, &EmployeeForm::onImageProcessed);
	connect(iriTracker, &IriTracker::resultTemplate, this, &EmployeeForm::onPathTemplate);

	// Di chuyển IriTracker vào thread để xử lý capture
	iriTracker->moveToThread(threadStream);

	// Kết nối captureThread đã được bắt đầu để gọi run trong IriTracker
	connect(threadStream, &QThread::started, iriTracker, [=]() {
		iriTracker->run(false, true, true);
		});


	// Bắt đầu thread
	threadStream->start();
}


void EmployeeForm::onPathTemplate(unsigned char* data, int size) {
	if (threadStream && threadStream->isRunning()) {
		threadStream->quit();
	}
	if (eyeSide == EyeSide::Right) {
		qDebug() << "Eye Right: " << FunctionPublic::templateConvertToByte(data, size);
		eyeRight = FunctionPublic::templateConvertToByte(data,size);
	}
	else if (eyeSide == EyeSide::Left) {
		qDebug() << "Eye Left: " << FunctionPublic::templateConvertToByte(data, size);
		eyeLeft = FunctionPublic::templateConvertToByte(data, size);
	}
}


void EmployeeForm::onImageProcessed(unsigned char* imageData,
	int imageLen,
	int imageWidth,
	int imageHeight) {
	if (imageData == nullptr || imageLen <= 0) {
		qDebug() << "Dữ liệu hình ảnh không hợp lệ!";
		return;
	}

	QImage img(imageData, imageWidth, imageHeight, QImage::Format_Grayscale8);

	if (img.isNull()) {
		qDebug() << "Không thể tạo QImage từ dữ liệu hình ảnh!";
		return;
	}

	QPixmap pixmap = QPixmap::fromImage(img);
	if (eyeSide == EyeSide::Right) {
		ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
	}
	else if (eyeSide == EyeSide::Left) {
		ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
	}
}



bool EmployeeForm::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QString imagePath;
		if (obj == ui.rightIrisImgLabel) {
			qDebug() << "Eye Right Clicked";
			eyeRight = "";
			eyeSide = EyeSide::Right;
			/*QPixmap pixmap("D:\\IrisTech\\Project\\IriTrackerStandard\\icons\\has-device.jpg");
			ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));*/
			processStreaming();
			return true;
		}
		else if (obj == ui.leftIrisImgLabel) {
			qDebug() << "Eye Left Clicked";
			eyeLeft = "";
			eyeSide = EyeSide::Left;
			/*QPixmap pixmap("D:\\IrisTech\\Project\\IriTrackerStandard\\icons\\has-device.jpg");
			ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));*/
			processStreaming();
			return true;
		}
	}
	return QWidget::eventFilter(obj, event);
}


void EmployeeForm::updatePasswordFields() {
	bool isChecked = ui.passwordCheckbox->isChecked();
	ui.passwordEdit->setEnabled(isChecked);
	ui.confirmPasswordEdit->setEnabled(isChecked);
}


void EmployeeForm::checkInputs() {
	QString id = ui.idEdit->text().trimmed();
	QString firstName = ui.firstNameEdit->text().trimmed();
	QString lastName = ui.lastNameEdit->text().trimmed();
	bool allowPassword = ui.passwordCheckbox->isChecked();
	QString password = ui.passwordEdit->text();
	QString confirmPassword = ui.confirmPasswordEdit->text();

	bool valid = !id.isEmpty() && !firstName.isEmpty() && !lastName.isEmpty();

	if (eyeLeft.isEmpty() || eyeRight.isEmpty()) {
		valid = allowPassword && !password.isEmpty() && password.length() >= 4 && (password == confirmPassword);
	}
	else {
		valid = true;
	}

	ui.btnOk->setEnabled(valid);
}


void EmployeeForm::btnBrowseClicked() {
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Image"), "", tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

	if (!fileName.isEmpty()) {
		ui.pathLabel->setText(fileName);
		ui.pathLabel->hide();

		QPixmap pixmap(fileName);
		ui.avatarLabel->setPixmap(pixmap.scaled(ui.avatarLabel->size(), Qt::KeepAspectRatio)); 
	}
}

void EmployeeForm::btnClearPhotoClicked() {
	ui.pathLabel->clear();
	ui.avatarLabel->clear(); 
}

void EmployeeForm::handleFormAction(const QString& action, QString id) {
	currentAction = action;
	userId = id;
	QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);
	for (const Department& department : departments) {
		ui.departmentCombobox->addItem(department.getName(), department.getDepartmentId());
	}

	if (action == "Edit") {
		User user = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectById(userId);

		if (!user.getUserId().isEmpty()) {
			ui.idEdit->setText(userId);
			ui.idEdit->setDisabled(true);
			ui.firstNameEdit->setText(user.getFirstName());
			ui.lastNameEdit->setText(user.getLastName());
			ui.emailEdit->setText(user.getEmail());
			ui.phoneEdit->setText(user.getPhone());
			ui.cellPhoneEdit->setText(user.getCellPhone());
			ui.addressTextEdit->setPlainText(user.getAddress());
			ui.passwordCheckbox->setChecked(user.getIsPassword());
			eyeRight = user.getEyeRight();
			eyeLeft = user.getEyeLeft();

			if (user.getIsPassword()) {
				ui.passwordEdit->setText("1234");
				ui.passwordEdit->setDisabled(true);
				ui.confirmPasswordEdit->setText("1234");
				ui.confirmPasswordEdit->setDisabled(true);
			}

			QString dateOfBirthString = user.getDateOfBirth();
			QDate birthDate = QDate::fromString(dateOfBirthString, "MM/dd/yyyy");
			if (birthDate.isValid()) {
				ui.dateOfBirthEdit->setDate(birthDate);
			}

			qint64 startWorkingTimestamp = user.getStartWorkingDate();

			QDateTime workingDateTime = QDateTime::fromSecsSinceEpoch(startWorkingTimestamp);
			QDate workingDate = workingDateTime.date();

			if (workingDate.isValid()) {
				ui.workingDateEdit->setDate(workingDate);
			}

			ui.disableEmployeeCheckbox->setChecked(!user.getIsActive());

			QByteArray avatarData = user.getAvatar();
			if (!avatarData.isEmpty()) {
				QPixmap pixmap;
				pixmap.loadFromData(avatarData);
				ui.avatarLabel->setPixmap(pixmap.scaled(ui.avatarLabel->size(), Qt::KeepAspectRatio));
			}
			else {
				ui.avatarLabel->clear();
			}

			int departmentId = user.getDepartmentId();
			for (int i = 0; i < ui.departmentCombobox->count(); ++i) {
				if (ui.departmentCombobox->itemData(i) == departmentId) {
					ui.departmentCombobox->setCurrentIndex(i);
					break;
				}
			}
		}
		else {
			qDebug() << "Không thể truy vấn thông tin người dùng: Không tìm thấy user với ID:" << userId;
		}
	}
	else if (action == "Add") {
		ui.idEdit->clear();
		ui.firstNameEdit->clear();
		ui.lastNameEdit->clear();
		ui.passwordEdit->clear();
		ui.confirmPasswordEdit->clear();
		ui.passwordEdit->setDisabled(true);
		ui.confirmPasswordEdit->setDisabled(true);

		// Thiết lập ngày sinh và ngày bắt đầu làm việc mặc định
		QDate birthDate(1965, 1, 1);
		ui.dateOfBirthEdit->setDate(birthDate);

		QDate currentDate = QDate::currentDate();
		ui.workingDateEdit->setDate(currentDate);

		ui.disableEmployeeCheckbox->setCheckState(Qt::Unchecked);
	}
}


void EmployeeForm::btnOkClicked() {
	QString id = ui.idEdit->text().trimmed();
	QString firstName = ui.firstNameEdit->text().trimmed();
	QString lastName = ui.lastNameEdit->text().trimmed();
	QString email = ui.emailEdit->text().trimmed();
	QString phone = ui.phoneEdit->text().trimmed();
	QString cellPhone = ui.cellPhoneEdit->text().trimmed();
	QString address = ui.addressTextEdit->toPlainText().trimmed();

	bool allowPassword = ui.passwordCheckbox->isChecked();
	QString password = FunctionPublic::hashPassword(ui.passwordEdit->text());
	QString confirmPassword = ui.confirmPasswordEdit->text();

	QString dateOfBirth = ui.dateOfBirthEdit->date().toString("MM/dd/yyyy");

	QDate startWorkingDate = ui.workingDateEdit->date();

	QDateTime startWorkingDateTime(startWorkingDate, QTime(0, 0));
	qint64 startWorkingTimestamp = startWorkingDateTime.toSecsSinceEpoch();

	QString imagePath = ui.pathLabel->text();
	bool disableUser = ui.disableEmployeeCheckbox->isChecked();


	QByteArray imageData;

	if (!imagePath.isEmpty()) {
		QFile imageFile(imagePath);
		if (imageFile.open(QIODevice::ReadOnly)) {
			imageData = imageFile.readAll();
			imageFile.close();
		}
		else {
			qDebug() << "Không thể mở file ảnh!";
		}
	}
	else if (currentAction == "Edit") {
		QPixmap currentPixmap = ui.avatarLabel->pixmap(Qt::ReturnByValue);
		if (!currentPixmap.isNull()) {
			QByteArray byteArray;
			QBuffer buffer(&byteArray);
			buffer.open(QIODevice::WriteOnly);
			currentPixmap.save(&buffer, "PNG");
			imageData = byteArray;
		}
	}

	int departmentId = ui.departmentCombobox->currentData().toInt();
	User user;
	user.setUserId(id);
	user.setFirstName(firstName);
	user.setLastName(lastName);
	user.setPassword(password);
	user.setDateOfBirth(dateOfBirth);
	user.setStartWorkingDate(startWorkingTimestamp);
	user.setEmail(email);
	user.setPhone(phone);
	user.setCellPhone(cellPhone);
	user.setAddress(address);
	user.setAvatar(imageData);
	user.setDepartmentId(departmentId);
	user.setIsPassword(allowPassword);
	user.setEyeRight(eyeRight);
	user.setEyeLeft(eyeLeft);
	user.setRoleId(2);
	if (currentAction == "Add") {
		if (!DatabaseHelper::getDatabaseInstance()->getUserRepository()->insert(user)) {
			qDebug() << "Lỗi khi thêm người dùng.";
		}
		else {
			qDebug() << "Thêm nhân viên thành công!";
			emit employeeChanged();
			this->close();
		}
	}
	else if (currentAction == "Edit") {
		if (!DatabaseHelper::getDatabaseInstance()->getUserRepository()->update(user)) {
			qDebug() << "Lỗi khi cập nhật người dùng.";
		}
		else {
			qDebug() << "Cập nhật nhân viên thành công!";
			emit employeeChanged();
			this->close();
		}
	}
}



void EmployeeForm::btnCancelClicked() {
	if (currentAction == "Add") {
		if (eyeRight != "") {
			FunctionPublic::deleteFile(eyeRight);
		}
		if (eyeLeft != "") {
			FunctionPublic::deleteFile(eyeLeft);
		}
		this->close();
	}
	else {
		this->close();
	}
}

void EmployeeForm::changeImageDevice(bool isDevice) {
	if (isDevice) {
		QPixmap pixmap("D:\\IrisTech\\Project\\IriTrackerStandard\\icons\\has-device.jpg");
		ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
		ui.leftIrisImgLabel->setPixmap(QPixmap("../icons/has-device.jpg"));
	}
	else {
		ui.rightIrisImgLabel->setPixmap(QPixmap("../icons/no-device.jpg"));
		ui.leftIrisImgLabel->setPixmap(QPixmap("../icons/no-device.jpg"));
	}
}