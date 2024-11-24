#include "IriTrackerStandard.h"
#include "Login.h"
#include "DatabaseHelper.h"
#include "DepartmentForm.h"
#include "Logout.h"
#include "EmployeeForm.h"
#include <QLinearGradient>
#include <QPalette>
#include <QMessageBox> 
#include <QRegularExpression>
#include <QIcon>
#include <QToolButton> 
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QSqlTableModel>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QTimer>
#include "CustomLineEdit.h"
#include "CustomFilterProxyModel.h"
#include "Department.h"
#include "User.h"
#include "AttendanceEventForm.h"
#include "AttendanceEvent.h"
#include <QList>
#include "AttendanceEventDeleteForm.h"
#include "QMessageBoxCustom.h"
#include "DatabaseHelper.h"
#include "ChangeDatabase.h"
#include "ExceptionForm.h"
#include <QTime>

IriTrackerStandard::IriTrackerStandard(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

	this->setWindowTitle("IriTracker - Time And Attendence Management System");

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.btnLogin, &QPushButton::clicked, this, &IriTrackerStandard::btnLoginClicked);

	ui.btnDepartment->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnEmployee->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnEvent->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnException->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnReport->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnBullentin->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnTool->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


	// Function of Tools
	ui.btnBackup->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnChangeDB->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnChangePassword->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnRestore->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	ui.btnSetting->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


	ui.layoutNavbar->setAlignment(Qt::AlignLeft);
	connect(ui.btnDepartment, &QPushButton::clicked, this, &IriTrackerStandard::btnDepartmentClicked);
	connect(ui.btnEmployee, &QPushButton::clicked, this, &IriTrackerStandard::btnEmployeeClicked);
	connect(ui.btnEvent, &QPushButton::clicked, this, &IriTrackerStandard::btnEventClicked);

	connect(ui.btnDepartmentAdd, &QPushButton::clicked, this, &IriTrackerStandard::btnDepartmentAddClicked);
	connect(ui.btnDepartmentEdit, &QPushButton::clicked, this, &IriTrackerStandard::btnDepartmentEditClicked);
	connect(ui.btnDepartmentDelete, &QPushButton::clicked, this, &IriTrackerStandard::btnDepartmentDeleteClicked);

	connect(ui.btnEmployeeAdd, &QPushButton::clicked, this, &IriTrackerStandard::btnEmployeeAddClicked);
	connect(ui.btnEmployeeEdit, &QPushButton::clicked, this, &IriTrackerStandard::btnEmployeeEditClicked);
	connect(ui.btnEmployeeDelete, &QPushButton::clicked, this, &IriTrackerStandard::btnEmployeeDeleteClicked);

	connect(ui.btnAddEvent, &QPushButton::clicked, this, &IriTrackerStandard::btnAddEventClicked);
	connect(ui.btnEditEvent, &QPushButton::clicked, this, &IriTrackerStandard::btnEditEventClicked);
	connect(ui.btnCheckoutEvent, &QPushButton::clicked, this, &IriTrackerStandard::btnCheckoutEventClicked);
	connect(ui.btnDeleteEvent, &QPushButton::clicked, this, &IriTrackerStandard::btnDeleteEventClicked);

	connect(ui.btnException, &QPushButton::clicked, this, &IriTrackerStandard::btnExceptionClicked);
	connect(ui.btnAddException, &QPushButton::clicked, this, &IriTrackerStandard::btnAddExceptionClicked);
	connect(ui.btnEditException, &QPushButton::clicked, this, &IriTrackerStandard::btnEditExceptionClicked);
	connect(ui.btnDeleteException, &QPushButton::clicked, this, &IriTrackerStandard::btnDeleteExceptionClicked);

	
	connect(ui.btnTool, &QPushButton::clicked, this, &IriTrackerStandard::btnToolClicked);
	// Functions of Tools
	connect(ui.btnChangeDB, &QPushButton::clicked, this, &IriTrackerStandard::btnChangeDBClicked);

	connect(ui.viewFormComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &IriTrackerStandard::handleViewFormCombobox);
	ui.viewFormComboBox->setCurrentIndex(5);

	!DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectAll().isEmpty() ? ui.stackedWidget->setCurrentIndex(5) : ui.stackedWidget->setCurrentIndex(0);
	loadDepartmentsAndUsers();
	setupRealTimeClock();
}

IriTrackerStandard::~IriTrackerStandard()

{
	DatabaseHelper::closeDatabase();
	delete DatabaseHelper::getDatabaseInstance();
}

void IriTrackerStandard::setupRealTimeClock() {
	QTimer* timer = new QTimer(this);

	connect(timer, &QTimer::timeout, this, &IriTrackerStandard::updateDateTime);
	timer->start(1000);

	updateDateTime();
}

void IriTrackerStandard::updateDateTime() {
	QString timeString = QTime::currentTime().toString("hh:mm:ss AP");  

	QString dateString = QDate::currentDate().toString("dddd, MMMM d, yyyy");

	ui.timeLabel->setText(timeString);
	ui.dateLabel->setText(dateString);
}

void IriTrackerStandard::loadDepartmentsAndUsers() {
	QList<Department> departmentsList = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);

	QStandardItemModel* model = new QStandardItemModel();
	ui.passwordInOutEdit->setEchoMode(QLineEdit::Password);
	ui.treeView->header()->setVisible(false);

	int totalEmployees = 0;

	for (const Department& department : departmentsList) {
		QStandardItem* departmentItem = new QStandardItem(department.getName());
		departmentItem->setFlags(departmentItem->flags() & ~Qt::ItemIsEditable);

		QList<QVariantList> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectByDepartmentId(department.getDepartmentId());
		int employeeCount = 0;

		for (const QVariantList& user : users) {
			QString userId = user[0].toString();
			QString fullname = user[1].toString() + ", " + user[2].toString();
			QString userDisplay = QString("%1 (%2)").arg(fullname).arg(userId);

			QStandardItem* userItem = new QStandardItem(userDisplay);
			userItem->setFlags(userItem->flags() & ~Qt::ItemIsEditable);
			departmentItem->appendRow(userItem);

			employeeCount++;
		}

		QString departmentDisplay = QString("%1 (%2)").arg(department.getName()).arg(employeeCount);
		departmentItem->setText(departmentDisplay);

		model->appendRow(departmentItem);

		totalEmployees += employeeCount;
	}

	CustomFilterProxyModel* filterModel = new CustomFilterProxyModel(this);
	filterModel->setSourceModel(model);
	filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	filterModel->setFilterKeyColumn(0);

	ui.treeView->setModel(filterModel);
	ui.treeView->expandAll();

	CustomLineEdit* customLineEdit = nullptr;
	if (ui.showLayout_1->count() > 0) {
		customLineEdit = qobject_cast<CustomLineEdit*>(ui.showLayout_1->itemAt(0)->widget());
	}

	if (!customLineEdit) {
		customLineEdit = new CustomLineEdit(this);
		ui.showLayout_1->insertWidget(0, customLineEdit);
	}

	customLineEdit->setEmployeeCount(totalEmployees);

	connect(customLineEdit, &QLineEdit::textChanged, this, [this, filterModel](const QString& text) {
		if (!text.startsWith("Employee(")) {
			filterModel->setFilterFixedString(text);
		}
		});

	connect(ui.treeView->selectionModel(), &QItemSelectionModel::currentChanged, this,
		[this](const QModelIndex& current, const QModelIndex& previous) {
			if (!current.isValid()) {
				return;
			}
			ui.checkInOutStackedWidget->setCurrentIndex(1);

			QModelIndex parentIndex = current.parent();
			if (parentIndex.isValid()) {
				QString selectedUser = current.data().toString();
				ui.nameInOutEdit->setText(selectedUser);
				// Tách lấy userId từ chuỗi selectedUser
				int startPos = selectedUser.lastIndexOf('(');
				int endPos = selectedUser.lastIndexOf(')');
				QString userId;
				if (startPos != -1 && endPos != -1 && endPos > startPos) {
					userId = selectedUser.mid(startPos + 1, endPos - startPos - 1);
				}
				User user = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectById(userId);
				QByteArray avatarData = user.getAvatar();
				if (!avatarData.isEmpty()) {
					QPixmap pixmap;
					pixmap.loadFromData(avatarData);
					ui.avatarInOutLabel->setPixmap(pixmap.scaled(ui.avatarInOutLabel->size(), Qt::KeepAspectRatio));
				}
				else {
					ui.avatarInOutLabel->setPixmap(QPixmap("../icons/no-image.png"));
				}
				bool isOrphan  = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(userId);

				ui.btnCheckInOut->setText(isOrphan ? "Check In" : "Check Out");
			}
		});

	connect(ui.btnBackInfo, &QPushButton::clicked, this, [this]() {
		ui.checkInOutStackedWidget->setCurrentIndex(0);
		ui.treeView->setCurrentIndex(QModelIndex());
		ui.treeView->clearSelection();
		});

	connect(ui.btnBackInOut, &QPushButton::clicked, this, [this]() {
		ui.checkInOutStackedWidget->setCurrentIndex(0);
		ui.treeView->setCurrentIndex(QModelIndex());
		ui.treeView->clearSelection();
		});

	connect(ui.btnCheckInOut, &QPushButton::clicked, this, [this]() {
		QString selectedUser = ui.nameInOutEdit->text();
		int startPos = selectedUser.lastIndexOf('(');
		int endPos = selectedUser.lastIndexOf(')');
		QString userId;

		if (startPos != -1 && endPos != -1 && endPos > startPos) {
			userId = selectedUser.mid(startPos + 1, endPos - startPos - 1);
		}

		QString password = ui.passwordInOutEdit->text();

		bool result = DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkInOut(userId, FunctionPublic::hashPassword(password));
		if (result) {
			qint64 currentTimestamp = QDateTime::currentDateTime().toSecsSinceEpoch();
			QString checkType = ui.btnCheckInOut->text();


			AttendanceEvent inEvent;
			inEvent.setType("In");
			inEvent.setDate(currentTimestamp);
			inEvent.setUserId(userId);

			AttendanceEvent outEvent;
			outEvent.setType("Out");
			outEvent.setDate(currentTimestamp);
			outEvent.setUserId(userId);

			bool success = false;
			if (checkType == "Check In") {
				success = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(inEvent);
			}
			else if (checkType == "Check Out") {
				success = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(outEvent);
			}

			if (success) {
				ui.checkInOutStackedWidget->setCurrentIndex(2);
				// Cập nhật trạng thái nút và trường password
				ui.passwordInOutEdit->setText("");
				bool isOrphan = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(userId);

				ui.btnCheckInOut->setText(isOrphan ? "Check In" : "Check Out");

				// Lấy thông tin người dùng
				User user = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectById(userId);

				// Tính toán thời gian làm việc
				double todayWorkHours = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->calculateTotalHours(userId, "Today");
				double weekWorkHours = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->calculateTotalHours(userId, "This Week");
				double monthWorkHours = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->calculateTotalHours(userId, "This Month");

				// Hiển thị thông tin
				QString welcomeMessage = (checkType == "Check In") ? "Welcome" : "Goodbye";

				ui.welcomeLabel_4->setText(welcomeMessage + ", " + user.getFirstName() + " " + user.getLastName());
				ui.idInOutLabel->setText("ID: " + user.getUserId());
				ui.todayLabel_4->setText("     Today: " + QString::number(todayWorkHours, 'f', 2) + " hours");
				ui.thisWeekLabel_4->setText("     This week: " + QString::number(weekWorkHours, 'f', 2) + " hours");
				ui.thisMonthLabel_4->setText("     This month: " + QString::number(monthWorkHours, 'f', 2) + " hours");

				if (checkType == "Check In") {
					ui.checkInOutLabel_4->setText("CHECK IN SUCCESSFULLY");
					ui.checkInOutLabel_4->setStyleSheet("QLabel {"
						"background-color: #6647ed;"
						"color: #fff;"
						"border - radius: 4px;"
						"}");
				}
				else if (checkType == "Check Out") {
					ui.checkInOutLabel_4->setText("CHECK OUT SUCCESSFULLY");
					ui.checkInOutLabel_4->setStyleSheet("QLabel {"
						"background-color: #a32a2a;"
						"color: #fff;"
						"border - radius: 4px;"
						"}");
				}
			}
			else {
				qDebug() << "Error updating attendance events for user:" << userId;
			}
		}
		else {
			qDebug() << "Wrong password!";
		}
		});
}

void IriTrackerStandard::onLoginSuccessful() {
	ui.btnLogin->setText("Log Out");
	ui.btnLogin->setIcon(QIcon("../icons/logout.png"));

	ui.stackedWidget->setCurrentIndex(1);

	disconnect(ui.btnLogin, &QPushButton::clicked, this, &IriTrackerStandard::btnLoginClicked);
	connect(ui.btnLogin, &QPushButton::clicked, this, &IriTrackerStandard::btnLogoutClicked);
}

void IriTrackerStandard::onLogoutSuccessful()
{
	ui.btnLogin->setText("Log In");
	ui.btnLogin->setIcon(QIcon("../icons/login.png"));

	ui.stackedWidget->setCurrentIndex(0);

	disconnect(ui.btnLogin, &QPushButton::clicked, this, &IriTrackerStandard::btnLogoutClicked);
	connect(ui.btnLogin, &QPushButton::clicked, this, &IriTrackerStandard::btnLoginClicked);

	if (DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkIfAdminExist().getUserId() == "admin") {
		if (!DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectAll().isEmpty()) {
			loadDepartmentsAndUsers();
			ui.stackedWidget->setCurrentIndex(5);
		}
		else {
			ui.stackedWidget->setCurrentIndex(0);
		}
	
	}
}

void IriTrackerStandard::btnLogoutClicked() {
	if (ui.btnLogin->text() == "Log Out") {
		QMessageBoxCustom logoutDialog;
		logoutDialog.setQuestionLabel("Do you want to log out Administrator?");
		logoutDialog.setWindowTitleCustom("Log Out");

		if (logoutDialog.exec() == QDialog::Accepted) {
			onLogoutSuccessful();
		}
	}
}

void IriTrackerStandard::btnLoginClicked() {
	Login* loginWindow = new Login();

	connect(loginWindow, &Login::loginSuccessful, this, &IriTrackerStandard::onLoginSuccessful);

	loginWindow->show();
}

void IriTrackerStandard::initializeBackButton() {
	if (!backButton) {
		backButton = new QPushButton("Back", this);

		backButton->setStyleSheet(
			"QPushButton {"
			"   border-radius: 0;"
			"   border: none;"
			"   text-align: center;"
			"   font-size: 17px;"
			"   width: 100;"
			"	font-family: Arial;"
			"	height: 16777215"
			"}"
			"QPushButton:hover {"
			"   background-color: #D0D0D0;"
			"   border: 1px solid #A0A0A0;"
			"   box-shadow: 2px 2px 5px rgba(0, 0, 0, 0.3);"
			"}"
		);

		backButton->setIcon(QIcon("../icons/back.png"));
		backButton->setMaximumWidth(80);

		ui.layoutNavbar->insertWidget(0, backButton);

		connect(backButton, &QPushButton::clicked, this, &IriTrackerStandard::backButtonClicked);
	}

	backButton->show();
}

void IriTrackerStandard::btnDepartmentClicked() {
	ui.stackedWidget->setCurrentIndex(2);
	initializeBackButton();

	ui.tableDepartment->setRowCount(0);
	ui.tableDepartment->verticalHeader()->setVisible(false);
	ui.tableDepartment->setColumnHidden(0, true);

	// Lấy danh sách các phòng ban từ lớp Department
	QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(true);

	for (const Department& dept : departments) {
		int row = ui.tableDepartment->rowCount();
		ui.tableDepartment->insertRow(row);

		// Thêm dữ liệu vào bảng
		ui.tableDepartment->setItem(row, 0, new QTableWidgetItem(QString::number(dept.getDepartmentId())));
		ui.tableDepartment->setItem(row, 1, new QTableWidgetItem(dept.getName()));
		ui.tableDepartment->setItem(row, 2, new QTableWidgetItem(dept.getDesc())); 

		// Tô màu cho hàng
		QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
		for (int col = 0; col < ui.tableDepartment->columnCount(); ++col) {
			QTableWidgetItem* item = ui.tableDepartment->item(row, col);
			if (item) {
				item->setBackground(QBrush(rowColor));
			}
		}
	}

	ui.tableDepartment->setSelectionBehavior(QAbstractItemView::SelectRows);

	// Kết nối sự kiện khi click vào ô
	connect(ui.tableDepartment, &QTableWidget::cellClicked, this, [this](int row, int column) {
		ui.tableDepartment->selectRow(row);
		ui.tableDepartment->setCurrentItem(ui.tableDepartment->item(row, column));
		});
}

void IriTrackerStandard::btnEmployeeClicked() {
	qDebug() << "Test";
	ui.stackedWidget->setCurrentIndex(3);
	initializeBackButton();

	// Reset lại bảng
	ui.tableEmployee->setRowCount(0);
	ui.tableEmployee->verticalHeader()->setVisible(false);  // Không ẩn cột id

	// Lấy danh sách người dùng
	QList<User> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectAll();

	// Lấy danh sách phòng ban
	QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false); // Truy vấn tất cả phòng ban đang hoạt động

	// Tạo một bảng ánh xạ departmentId với departmentName
	QMap<int, QString> departmentMap;
	for (const Department& dep : departments) {
		departmentMap.insert(dep.getDepartmentId(), dep.getName());
	}

	// Duyệt qua danh sách người dùng và thêm vào bảng
	for (const User& user : users) {
		QString userId = user.getUserId();  // Lấy userId trực tiếp từ đối tượng User
		QString firstName = user.getFirstName();
		QString lastName = user.getLastName();
		QString dateOfBirth = user.getDateOfBirth();
		qint64 startWorkingTimestamp = user.getStartWorkingDate();

		// Chuyển đổi qint64 thành QDateTime, sau đó lấy QDate
		QDateTime workingDateTime = QDateTime::fromSecsSinceEpoch(startWorkingTimestamp);
		QDate workingDate = workingDateTime.date();
		int departmentId = user.getDepartmentId();
		int isActive = user.getIsActive();

		// Lấy tên phòng ban từ bảng ánh xạ
		QString departmentName = departmentMap.value(departmentId, "N/A"); // Nếu không tìm thấy, dùng "N/A"
		QString type = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->getLastAttendanceType(userId);
		int row = ui.tableEmployee->rowCount();
		ui.tableEmployee->insertRow(row);

		ui.tableEmployee->setItem(row, 0, new QTableWidgetItem(userId));
		ui.tableEmployee->setItem(row, 1, new QTableWidgetItem(firstName + " " + lastName));
		ui.tableEmployee->setItem(row, 2, new QTableWidgetItem(dateOfBirth));
		ui.tableEmployee->setItem(row, 3, new QTableWidgetItem(workingDate.toString("MM/dd/yyyy")));
		ui.tableEmployee->setItem(row, 4, new QTableWidgetItem(type));
		ui.tableEmployee->setItem(row, 5, new QTableWidgetItem(departmentName));
		ui.tableEmployee->setItem(row, 6, new QTableWidgetItem(isActive == 1 ? "Active" : "Disable"));

		// Đặt màu nền cho hàng
		QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
		for (int col = 0; col < ui.tableEmployee->columnCount(); ++col) {
			QTableWidgetItem* item = ui.tableEmployee->item(row, col);
			if (item) {
				item->setBackground(QBrush(rowColor));
			}
		}
	}

	ui.tableEmployee->setSelectionBehavior(QAbstractItemView::SelectRows);

	// Kết nối sự kiện khi click vào ô
	connect(ui.tableEmployee, &QTableWidget::cellClicked, this, [this](int row, int column) {
		ui.tableEmployee->selectRow(row);
		ui.tableEmployee->setCurrentItem(ui.tableEmployee->item(row, column));
		});
}

void IriTrackerStandard::backButtonClicked()
{
	ui.stackedWidget->setCurrentIndex(1);

	if (backButton) {
		backButton->hide();
	}
}

void IriTrackerStandard::btnDepartmentAddClicked() {
	DepartmentForm* departmentForm = new DepartmentForm();
	departmentForm->setWindowTitle("Add Department");

	connect(this, &IriTrackerStandard::departmentFormOpened, departmentForm, &DepartmentForm::handleFormAction);
	connect(departmentForm, &DepartmentForm::departmentChanged, this, &IriTrackerStandard::btnDepartmentClicked);
	connect(departmentForm, &DepartmentForm::departmentChanged, this, &IriTrackerStandard::loadDepartmentsAndUsers);

	emit departmentFormOpened("Add");

	departmentForm->show();
}

void IriTrackerStandard::btnDepartmentEditClicked() {
	int currentRow = ui.tableDepartment->currentRow();

	if (currentRow < 0) {
		qDebug() << "Vui lòng chọn một hàng để chỉnh sửa!";
		return; 
	}

	QString name = ui.tableDepartment->item(currentRow, 1)->text();
	QString description = ui.tableDepartment->item(currentRow, 2)->text();
	int departmentId = ui.tableDepartment->item(currentRow, 0)->text().toInt();
	DepartmentForm* departmentForm = new DepartmentForm();
	departmentForm->setWindowTitle("Edit Department");

	connect(this, &IriTrackerStandard::departmentFormOpened, departmentForm, &DepartmentForm::handleFormAction);
	connect(departmentForm, &DepartmentForm::departmentChanged, this, &IriTrackerStandard::btnDepartmentClicked);
	connect(departmentForm, &DepartmentForm::departmentChanged, this, &IriTrackerStandard::loadDepartmentsAndUsers);

	emit departmentFormOpened("Edit", departmentId, name, description);

	departmentForm->show();
}

void IriTrackerStandard::btnDepartmentDeleteClicked() {
	int currentRow = ui.tableDepartment->currentRow();

	if (currentRow < 0) {
		qDebug() << "Please select a row to delete!";
		return;
	}

	int departmentId = ui.tableDepartment->item(currentRow, 0)->text().toInt();

	QMessageBoxCustom deleteDialog;
	deleteDialog.setQuestionLabel("Are you sure you want to delete this department?");
	deleteDialog.setWindowTitleCustom("Delete Department");

	if (deleteDialog.exec() == QDialog::Accepted) {
		if (DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->deleteItem(departmentId)) {
			qDebug() << "Department deleted successfully!";
			btnDepartmentClicked();
			loadDepartmentsAndUsers();
		}
		else {
			qDebug() << "Error while deleting department!";
		}
	}
}

void IriTrackerStandard::btnEmployeeAddClicked() {
	EmployeeForm* employeeForm = new EmployeeForm();
	employeeForm->setWindowTitle("Add Employee");

	connect(this, &IriTrackerStandard::employeeFormOpened, employeeForm, &EmployeeForm::handleFormAction);
	connect(employeeForm, &EmployeeForm::employeeChanged, this, &IriTrackerStandard::btnEmployeeClicked);
	connect(employeeForm, &EmployeeForm::employeeChanged, this, &IriTrackerStandard::loadDepartmentsAndUsers);
	emit employeeFormOpened("Add");

	employeeForm->show();
}

void IriTrackerStandard::btnEmployeeEditClicked() {
	int currentRow = ui.tableEmployee->currentRow();

	if (currentRow < 0) {
		qDebug() << "Vui lòng chọn một hàng để chỉnh sửa!";
		return;
	}

	QString userId = ui.tableEmployee->item(currentRow, 0)->text();
	EmployeeForm* employeeForm = new EmployeeForm();
	employeeForm->setWindowTitle("Edit Employee");

	connect(this, &IriTrackerStandard::employeeFormOpened, employeeForm, &EmployeeForm::handleFormAction);
	connect(employeeForm, &EmployeeForm::employeeChanged, this, &IriTrackerStandard::btnEmployeeClicked);
	connect(employeeForm, &EmployeeForm::employeeChanged, this, &IriTrackerStandard::loadDepartmentsAndUsers);

	emit employeeFormOpened("Edit", userId);

	employeeForm->show();
}

void IriTrackerStandard::btnEmployeeDeleteClicked() {
	int currentRow = ui.tableEmployee->currentRow();

	if (currentRow < 0) {
		qDebug() << "Please select a row to delete!";
		return;
	}

	QString userId = ui.tableEmployee->item(currentRow, 0)->text();

	QMessageBoxCustom msgBox(this);
	msgBox.setQuestionLabel("Are you sure you want to delete this user?");
	msgBox.setWindowTitleCustom("Delete User");

	if (msgBox.exec() == QDialog::Accepted) {
		if (DatabaseHelper::getDatabaseInstance()->getUserRepository()->deleteItem(userId)) {
			qDebug() << "User deleted successfully!";
			btnEmployeeClicked();
			loadDepartmentsAndUsers();
		}
		else {
			qDebug() << "Error while deleting user!";
		}
	}
}

void IriTrackerStandard::btnEventClicked() {
	// Các dòng mã hiện tại trong btnEventClicked
	ui.stackedWidget->setCurrentIndex(6);
	initializeBackButton();
	QList<Department> departmentsList = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);
	QStandardItemModel* model = new QStandardItemModel();
	ui.treeViewAttendanceEvent->header()->setVisible(false);

	int totalEmployees = 0;

	for (const Department& department : departmentsList) {
		QStandardItem* departmentItem = new QStandardItem(department.getName());

		QList<QVariantList> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectByDepartmentId(department.getDepartmentId());

		for (const QVariantList& user : users) {
			QString userId = user[0].toString();
			QString fullname = user[1].toString() + " " + user[2].toString();
			QString userDisplay = QString("%1 (%2)").arg(fullname).arg(userId);

			QStandardItem* userItem = new QStandardItem(userDisplay);
			userItem->setData(userId, Qt::UserRole);  // Lưu userId vào item data
			departmentItem->appendRow(userItem);

			totalEmployees++;
		}

		model->appendRow(departmentItem);
	}

	CustomFilterProxyModel* filterModel = new CustomFilterProxyModel(this);
	filterModel->setSourceModel(model);
	filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	filterModel->setFilterKeyColumn(0);

	ui.treeViewAttendanceEvent->setModel(filterModel);
	ui.treeViewAttendanceEvent->expandAll();

	CustomLineEdit* customLineEdit = nullptr;
	if (ui.showLayout_2->count() > 0) {
		customLineEdit = qobject_cast<CustomLineEdit*>(ui.showLayout_2->itemAt(0)->widget());
	}

	if (!customLineEdit) {
		customLineEdit = new CustomLineEdit(this);
		ui.showLayout_2->insertWidget(0, customLineEdit);
	}

	customLineEdit->setEmployeeCount(totalEmployees);

	connect(customLineEdit, &QLineEdit::textChanged, filterModel, &CustomFilterProxyModel::setFilterFixedString);

	connect(customLineEdit, &CustomLineEdit::searchCleared, this, [this]() {
		ui.treeViewAttendanceEvent->setModel(nullptr);
		loadDepartmentsAndUsers();
		});

	// Bắt sự kiện khi người dùng click vào một item trong treeView
	connect(ui.treeViewAttendanceEvent, &QTreeView::clicked, this, &IriTrackerStandard::onUserItemClicked);
}

void IriTrackerStandard::onUserItemClicked(const QModelIndex& index) {
	selectedEventUserId = index.data(Qt::UserRole).toString();  // Lấy userId từ item data
	// Gọi hàm để lấy danh sách sự kiện điểm danh của user
	onLoadEvent();
}

void IriTrackerStandard::onLoadEvent() {
	bool isCheck = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->checkForOrphanInEvents(selectedEventUserId);
	if (isCheck) {
		ui.btnCheckoutEvent->setDisabled(true);
	}
	else {
		ui.btnCheckoutEvent->setDisabled(false);
	}

	//QList<AttendanceEvent> attendanceEvents = AttendanceEvent::selectAttendanceByUserId(selectedEventUserId);

	QList<AttendanceEvent> attendanceEvents = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->selectAttendanceEventByUserId(selectedEventUserId, timeFilter);

	// Xóa dữ liệu hiện tại trong bảng
	ui.attendanceEventsTable->setRowCount(0);
	ui.attendanceEventsTable->verticalHeader()->setVisible(false);
	ui.attendanceEventsTable->setColumnHidden(0, true);

	// Duyệt qua danh sách và thêm dữ liệu vào bảng
	for (const AttendanceEvent& event : attendanceEvents) {
		int row = ui.attendanceEventsTable->rowCount();
		ui.attendanceEventsTable->insertRow(row);

		// Chuyển đổi qint64 (Unix timestamp) thành chuỗi định dạng ngày và giờ
		qint64 timestamp = event.getDate(); 
		QDateTime dateTime = QDateTime::fromSecsSinceEpoch(timestamp);
		QString date = dateTime.date().toString("MM/dd/yyyy"); // Định dạng ngày
		QString time = dateTime.time().toString("hh:mm:ss AP"); // Định dạng thời gian
		ui.attendanceEventsTable->setItem(row, 0, new QTableWidgetItem(QString::number(event.getAttendanceEventId())));
		ui.attendanceEventsTable->setItem(row, 1, new QTableWidgetItem(event.getType()));
		ui.attendanceEventsTable->setItem(row, 2, new QTableWidgetItem(date));
		ui.attendanceEventsTable->setItem(row, 3, new QTableWidgetItem(time));
		ui.attendanceEventsTable->setItem(row, 4, new QTableWidgetItem("")); // Để trống cho cột Exception

		// Tô màu cho hàng (giống như trong tableDepartment)
		QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
		for (int col = 0; col < ui.attendanceEventsTable->columnCount(); ++col) {
			QTableWidgetItem* item = ui.attendanceEventsTable->item(row, col);
			if (item) {
				item->setBackground(QBrush(rowColor));
			}
		}
	}

	ui.attendanceEventsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void IriTrackerStandard::btnAddEventClicked() {
	AttendanceEventForm* attendanceEventForm = new AttendanceEventForm();
	attendanceEventForm->setWindowTitle("Add Employee Attendance Event");
	
	connect(this, &IriTrackerStandard::attendanceEventFormOpened, attendanceEventForm, &AttendanceEventForm::handleFormAction);
	connect(attendanceEventForm, &AttendanceEventForm::attendanceEventChanged, this, &IriTrackerStandard::onLoadEvent);
	emit attendanceEventFormOpened("Add", selectedEventUserId);

	attendanceEventForm->show();
}

void IriTrackerStandard::btnEditEventClicked() {
	int currentRow = ui.attendanceEventsTable->currentRow();

	if (currentRow < 0) {
		qDebug() << "Vui lòng chọn một hàng để chỉnh sửa!";
		return;
	}
	int eventId = ui.attendanceEventsTable->item(currentRow, 0)->text().toInt();

	AttendanceEventForm* attendanceEventForm = new AttendanceEventForm();
	attendanceEventForm->setWindowTitle("Edit Employee Attendance Event");
	connect(this, &IriTrackerStandard::attendanceEventFormOpened, attendanceEventForm, &AttendanceEventForm::handleFormAction);
	connect(attendanceEventForm, &AttendanceEventForm::attendanceEventChanged, this, &IriTrackerStandard::onLoadEvent);

	emit attendanceEventFormOpened("Edit", selectedEventUserId, eventId);

	attendanceEventForm->show();

}

void IriTrackerStandard::btnCheckoutEventClicked() {
	// Lấy thời gian hiện tại
	QDateTime currentDateTime = QDateTime::currentDateTime();

	// Định dạng thời gian hiện tại theo kiểu "MM/dd/yyyy hh:mm:ss AP"
	QString formattedTime = currentDateTime.toString("MM/dd/yyyy hh:mm:ss AP");
	qDebug() << "Current Time (formatted):" << formattedTime;

	// Chuyển đổi QDateTime thành qint64 (timestamp)
	qint64 timestamp = currentDateTime.toSecsSinceEpoch();
	qDebug() << "Current Time (qint64):" << timestamp;

	// Mở hộp thoại xác nhận
	QMessageBoxCustom checkoutDialog;
	checkoutDialog.setQuestionLabel("Do you want to check out this employee?");
	checkoutDialog.setWindowTitleCustom("Check out employee");

	if (checkoutDialog.exec() == QDialog::Accepted) {

		AttendanceEvent outEvent;
		outEvent.setType("Out");
		outEvent.setDate(timestamp);
		outEvent.setUserId(selectedEventUserId);

		bool inSuccess = DatabaseHelper::getDatabaseInstance()->getAttendanceEventRepository()->insert(outEvent);

		if (!inSuccess) {
			qDebug() << "Error adding attendance events for user:" << selectedEventUserId;
		}
		else {
			onLoadEvent();
		}
	}
	else {
		qDebug() << "Checkout canceled.";
	}

}

void IriTrackerStandard::btnDeleteEventClicked() {
	int currentRow = ui.attendanceEventsTable->currentRow();

	if (currentRow < 0) {
		qDebug() << "Vui lòng chọn một hàng để chỉnh sửa!";
		return;
	}
	int eventId = ui.attendanceEventsTable->item(currentRow, 0)->text().toInt();
	AttendanceEventDeleteForm* attendanceEventDeleteForm = new AttendanceEventDeleteForm();
	attendanceEventDeleteForm->setWindowTitle("Delete Employee Attendance Event");
	connect(this, &IriTrackerStandard::attendanceEventDeleteFormOpened, attendanceEventDeleteForm, &AttendanceEventDeleteForm::handleFormAction);
	connect(attendanceEventDeleteForm, &AttendanceEventDeleteForm::attendanceEventDeleteChanged, this, &IriTrackerStandard::onLoadEvent);

	emit attendanceEventDeleteFormOpened(selectedEventUserId, eventId);

	attendanceEventDeleteForm->show();
}

void IriTrackerStandard::handleViewFormCombobox() {
	QString selectedText = ui.viewFormComboBox->currentText();
	timeFilter = selectedText;
	onLoadEvent();
}

// Exceptions
void IriTrackerStandard::btnExceptionClicked() {
	ui.stackedWidget->setCurrentIndex(7);
	initializeBackButton();

	ui.tableException->setRowCount(0);
	ui.tableException->verticalHeader()->setVisible(false);
	ui.tableException->setColumnHidden(0, true);

	QList<Exception> exceptions = DatabaseHelper::getDatabaseInstance()->getExceptionRepository()->selectAll();

	for (const Exception& exp : exceptions) {
		int row = ui.tableException->rowCount();
		ui.tableException->insertRow(row);

		qint64 seconds = exp.getPaidHours();

		// Thêm dữ liệu vào bảng
		ui.tableException->setItem(row, 0, new QTableWidgetItem((exp.getExceptionId())));
		ui.tableException->setItem(row, 1, new QTableWidgetItem(exp.getName()));
		ui.tableException->setItem(row, 2, new QTableWidgetItem(QTime(seconds / 3600, (seconds % 3600) / 60).toString("HH:mm")));
		ui.tableException->setItem(row, 3, new QTableWidgetItem(QString::number(exp.getPaidCoefficient(), 'f', 2)));
		ui.tableException->setItem(row, 4, new QTableWidgetItem(QString::number(exp.getWorkCoefficient(), 'f', 2)));

		// Tô màu cho hàng
		QColor rowColor = (row % 2 == 0) ? QColor("#e3e9f1") : QColor("#dbdbd8");
		for (int col = 0; col < ui.tableException->columnCount(); ++col) {
			QTableWidgetItem* item = ui.tableException->item(row, col);
			if (item) {
				item->setBackground(QBrush(rowColor));
			}
		}
	}

	ui.tableException->setSelectionBehavior(QAbstractItemView::SelectRows);

	// Kết nối sự kiện khi click vào ô
	connect(ui.tableException, &QTableWidget::cellClicked, this, [this](int row, int column) {
		ui.tableException->selectRow(row);
		ui.tableException->setCurrentItem(ui.tableException->item(row, column));
		});
}

void IriTrackerStandard::btnAddExceptionClicked() {
	ExceptionForm* exceptionForm = new ExceptionForm();
	exceptionForm->setWindowTitle("Add Exception");


	exceptionForm->show();
}

void IriTrackerStandard::btnEditExceptionClicked() {
	ExceptionForm* exceptionForm = new ExceptionForm();
	exceptionForm->setWindowTitle("Edit Exception");


	exceptionForm->show();
}

void IriTrackerStandard::btnDeleteExceptionClicked() {

}

// Tools
void IriTrackerStandard::btnToolClicked() {
	ui.stackedWidget->setCurrentIndex(8);
	initializeBackButton();
}

// Functions of Tools
void IriTrackerStandard::btnChangeDBClicked() {
	ChangeDatabase* changeDatabase = new ChangeDatabase();
	changeDatabase->show();
}