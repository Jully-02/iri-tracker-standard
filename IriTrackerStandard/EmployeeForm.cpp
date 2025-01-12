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
#include "SingletonManager.h"
#include <QMediaPlayer>
#include <QSettings>

EmployeeForm::EmployeeForm(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	iriTracker = SingletonManager::getInstance().getIriTrackerForm();
	threadStream = new QThread();
	
	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

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


	ui.rightIrisImgLabel->installEventFilter(this);
	ui.leftIrisImgLabel->installEventFilter(this);

	SingletonManager& manager = SingletonManager::getInstance();

	connect(manager.getIriTracker(), &IriTracker::imageProcessed,
		this, &EmployeeForm::onImageProcessed);

}

EmployeeForm::~EmployeeForm()
{
	SingletonManager& manager = SingletonManager::getInstance();
	auto employeeForm = manager.getEmployeeForm();
	if (employeeForm) {
		employeeForm->deleteLater();
	}
}

Ui::EmployeeFormClass EmployeeForm::getUi() {
	return ui;
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

	// dis
	disconnect(threadStream, &QThread::started, nullptr, nullptr);

	// Di chuyển IriTracker vào thread để xử lý capture
	iriTracker->moveToThread(threadStream);

	// Kết nối captureThread đã được bắt đầu để gọi run trong IriTracker
	if (!threadStream->isRunning()) {
		// Kết nối captureThread đã được bắt đầu để gọi run trong IriTracker
		connect(threadStream, &QThread::started, iriTracker, [=]() {
			iriTracker->run(false, true, true);
			});
	}


	// Bắt đầu thread
	threadStream->start();

	QMediaPlayer* player = new QMediaPlayer();
	QSettings settings("config.ini", QSettings::IniFormat);
	QString language = settings.value("General/language", "en").toString();
	if (language == "en") {
		player->setMedia(QUrl::fromLocalFile("D:\\IrisTech\\Project\\IriTrackerStandard\\sounds\\move_eye_heather.wav"));
	}
	else {
		player->setMedia(QUrl::fromLocalFile("D:\\IrisTech\\Project\\IriTrackerStandard\\sounds\\move_eye_heather_vi.wav"));
	}
	player->setVolume(100);
	player->play();
}


void EmployeeForm::onPathTemplate(unsigned char* data, int size) {

		if (eyeSide == EyeSide::Right && eyeRight.isEmpty()) {
			eyeRight = FunctionPublic::templateConvertToByte(data, size);
			qDebug() << "Eye Right: " << eyeRight;
			return;
		}
		else if (eyeSide == EyeSide::Left && eyeLeft.isEmpty()) {
			eyeLeft = FunctionPublic::templateConvertToByte(data, size);
			qDebug() << "Eye Left: " << eyeLeft;
			return;
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
		if (threadStream->isRunning()) {
			threadStream->quit();
		}
	}
	else if (eyeSide == EyeSide::Left) {
		ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
		if (threadStream->isRunning()) {
			threadStream->quit();
		}
	}
}



bool EmployeeForm::eventFilter(QObject* obj, QEvent* event) {
	if (event->type() == QEvent::MouseButtonPress) {
		QString imagePath;
		if (obj == ui.rightIrisImgLabel) {
			qDebug() << "Eye Right Clicked";
			eyeRight = "";
			eyeSide = EyeSide::Right;
			if (eyeLeft.isEmpty()) {
				QPixmap iriPixmap;
				if (device) {
					iriPixmap.load("D://IrisTech//Project//IriTrackerStandard//icons//has-device.jpg");
					ui.leftIrisImgLabel->setPixmap(iriPixmap);
				}
				else {
					iriPixmap.load("D://IrisTech//Project//IriTrackerStandard//icons//no-iris.jpg");
					ui.leftIrisImgLabel->setPixmap(iriPixmap);
				}
			}
			processStreaming();
			return true;
		}
		else if (obj == ui.leftIrisImgLabel) {
			qDebug() << "Eye Left Clicked";
			eyeLeft = "";
			eyeSide = EyeSide::Left;
			if (eyeRight.isEmpty()) {
				QPixmap iriPixmap;
				if (device) {
					iriPixmap.load("D://IrisTech//Project//IriTrackerStandard//icons//has-device.jpg");
					ui.rightIrisImgLabel->setPixmap(iriPixmap);
				}
				else {
					iriPixmap.load("D://IrisTech//Project//IriTrackerStandard//icons//no-iris.jpg");
					ui.rightIrisImgLabel->setPixmap(iriPixmap);
				}
			}
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

	// Kiểm tra các trường bắt buộc
	bool valid = !id.isEmpty() && !firstName.isEmpty() && !lastName.isEmpty();

	// Kiểm tra dữ liệu eyeLeft và eyeRight
	bool eyeDataAvailable = !eyeLeft.isEmpty() || !eyeRight.isEmpty();

	if (!eyeDataAvailable) {
		// Nếu không có dữ liệu eyeLeft và eyeRight, kiểm tra mật khẩu
		valid = allowPassword && !password.isEmpty() && password.length() >= 4 && (password == confirmPassword);
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
	QPixmap avatarPixmap;
	avatarPixmap.load("D://IrisTech//Project//IriTrackerStandard//icons//no-image.png");
	ui.avatarLabel->setPixmap(avatarPixmap);
}

void EmployeeForm::handleFormAction(const QString& action, QString id) {
	currentAction = action;
	userId = id;
	if (device) {
		QString imagePath = "../icons/has-device.jpg";
		QPixmap pixmap(imagePath);
		if (pixmap.isNull()) {
			qDebug() << "Failed to load has-device.jpg!";
		}
		else {
			ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
			ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
		}
	}
	else {
		QString imagePath = "../icons/no-device.jpg";
		QPixmap noDevicePixmap(imagePath);
		if (noDevicePixmap.isNull()) {
			qDebug() << "Failed to load no-device.jpg!";
		}
		else {
			ui.rightIrisImgLabel->setPixmap(noDevicePixmap);
			ui.leftIrisImgLabel->setPixmap(noDevicePixmap);
		}
	}
	ui.errorLabel->clear();
	ui.errorLabel->setStyleSheet("");
	QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);
	for (const Department& department : departments) {
		ui.departmentCombobox->addItem(department.getName(), department.getDepartmentId());
	}
	ui.tabWidget->setCurrentIndex(0);
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
			if (!eyeRight.isEmpty()) {
				ui.noticeRightLabel->setText(tr("( Registered )"));
			}

			if (!eyeLeft.isEmpty()) {
				ui.noticeLeftLabel->setText(tr("( Registered )"));
			}

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
		ui.departmentCombobox->clear();
		ui.noticeRightLabel->setText(tr("(? Not register yet )"));
		ui.noticeLeftLabel->setText(tr("(? Not register yet )"));
		QPixmap avatarPixmap;
		avatarPixmap.load("D:/IrisTech/Project/IriTrackerStandard/icons/no-image.png");
		ui.avatarLabel->setPixmap(avatarPixmap);

		if (device) {
			QString imagePath = "../icons/has-device.jpg";
			QPixmap pixmap(imagePath);
			ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
			ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
		}
		else {
			QString imagePath = "../icons/no-device.jpg";
			QPixmap pixmap(imagePath);
			ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
			ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
		}

		QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);
		for (const Department& department : departments) {
			ui.departmentCombobox->addItem(department.getName(), department.getDepartmentId());
		}
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
	if (threadStream->isRunning()) {
		threadStream->quit();
	}

	iriTracker->isCancel = true;

	disconnect(iriTracker, &IriTracker::imageProcessed, this, &EmployeeForm::onImageProcessed);
	disconnect(iriTracker, &IriTracker::imageResult, this, &EmployeeForm::onImageProcessed);
	disconnect(iriTracker, &IriTracker::resultTemplate, this, &EmployeeForm::onPathTemplate);

	if (device) {
		QString imagePath = "../icons/has-device.jpg";
		QPixmap pixmap(imagePath);
		ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
		ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
	}
	else {
		QString imagePath = "../icons/no-device.jpg";
		QPixmap pixmap(imagePath);
		ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
		ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
	}
	this->close();
}

void EmployeeForm::changeImageDevice(bool isDevice) {
	if (isDevice != device) {
		device = isDevice;
		if (isDevice) {
			QString imagePath = "../icons/has-device.jpg";
			QPixmap pixmap(imagePath);
			if (pixmap.isNull()) {
				qDebug() << "Failed to load has-device.jpg!";
			}
			else {
				ui.rightIrisImgLabel->setPixmap(pixmap.scaled(ui.rightIrisImgLabel->size(), Qt::KeepAspectRatio));
				ui.leftIrisImgLabel->setPixmap(pixmap.scaled(ui.leftIrisImgLabel->size(), Qt::KeepAspectRatio));
			}
		}
		else {
			QString imagePath = "../icons/no-device.jpg";
			QPixmap noDevicePixmap(imagePath);
			if (noDevicePixmap.isNull()) {
				qDebug() << "Failed to load no-device.jpg!";
			}
			else {
				ui.rightIrisImgLabel->setPixmap(noDevicePixmap);
				ui.leftIrisImgLabel->setPixmap(noDevicePixmap);
			}
		}

		ui.rightIrisImgLabel->repaint();
		ui.leftIrisImgLabel->repaint();
	}
}
