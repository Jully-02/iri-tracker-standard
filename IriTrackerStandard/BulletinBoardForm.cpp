#include "BulletinBoardForm.h"
#include <QIcon>
#include <QDate>
#include <QTableWidgetItem>
#include "Department.h"
#include "DatabaseHelper.h"
#include "User.h"

BulletinBoardForm::BulletinBoardForm(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	ui.startDateEdit->setDate(QDate::currentDate());
	ui.endDateEdit->setDate(QDate::currentDate().addDays(1));


	ui.departmentComboBox->setEnabled(false);
	ui.filterEmployee->setEnabled(false);
	ui.employeeTable->setEnabled(false);
	ui.allRadio->setChecked(true);
	ui.btnOk->setEnabled(false);

	connect(ui.allRadio, &QRadioButton::toggled, this, &BulletinBoardForm::onRadioChanged);
	connect(ui.departmentRadio, &QRadioButton::toggled, this, &BulletinBoardForm::onRadioChanged);
	connect(ui.selectionRadio, &QRadioButton::toggled, this, &BulletinBoardForm::onRadioChanged);

	connect(ui.titleLineEdit, &QLineEdit::textChanged, this, &BulletinBoardForm::onInputChanged);
	connect(ui.contentTextEdit, &QTextEdit::textChanged, this, &BulletinBoardForm::onInputChanged);

	connect(ui.btnOk, &QPushButton::clicked, this, &BulletinBoardForm::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &BulletinBoardForm::btnCancelClicked);

}

BulletinBoardForm::~BulletinBoardForm()
{}


void BulletinBoardForm::onRadioChanged() {
	if (ui.allRadio->isChecked()) {
		ui.filterEmployee->setEnabled(false);
		ui.departmentComboBox->setEnabled(false);
		ui.employeeTable->setEnabled(false);
	}
	else if (ui.departmentRadio->isChecked()) {
		ui.filterEmployee->setEnabled(false);
		ui.departmentComboBox->setEnabled(true);
		ui.employeeTable->setEnabled(false);
	}
	else if (ui.selectionRadio->isChecked()) {
		ui.filterEmployee->setEnabled(true);
		ui.departmentComboBox->setEnabled(false);
		ui.employeeTable->setEnabled(true);
	}
}

void BulletinBoardForm::onInputChanged() {
	bool isTitleEmpty = ui.titleLineEdit->text().trimmed().isEmpty();
	bool isContentEmpty = ui.contentTextEdit->toPlainText().trimmed().isEmpty();

	ui.btnOk->setEnabled(!isTitleEmpty && !isContentEmpty);
}

void BulletinBoardForm::loadEmployee() {
	QList<User> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectAll();
	ui.employeeTable->verticalHeader()->setVisible(false);

	ui.employeeTable->setColumnCount(2);
	QStringList headers = { "Select", "Employee" };
	ui.employeeTable->setHorizontalHeaderLabels(headers);

	ui.employeeTable->setRowCount(0);
	for (int i = 0; i < users.size(); ++i) {
		const User& user = users.at(i);

		ui.employeeTable->insertRow(i);

		QCheckBox* checkbox = new QCheckBox(ui.employeeTable);
		QWidget* checkboxWidget = new QWidget();
		QHBoxLayout* checkboxLayout = new QHBoxLayout(checkboxWidget);
		checkboxLayout->addWidget(checkbox);
		checkboxLayout->setAlignment(Qt::AlignCenter);
		checkboxLayout->setContentsMargins(0, 0, 0, 0);
		checkboxWidget->setLayout(checkboxLayout);
		ui.employeeTable->setCellWidget(i, 0, checkboxWidget);

		QString userInfo = QString("%1 %2 (%3)").arg(user.getFirstName()).arg(user.getLastName()).arg(user.getUserId());
		QTableWidgetItem* userInfoItem = new QTableWidgetItem(userInfo);
		userInfoItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		ui.employeeTable->setItem(i, 1, userInfoItem);

		connect(checkbox, &QCheckBox::toggled, this, [this, user](bool checked) {
			if (checked) {
				if (!selectedUserIds.contains(user.getUserId())) {
					selectedUserIds.append(user.getUserId());
				}
			}
			else {
				selectedUserIds.removeAll(user.getUserId());
			}
			});
	}

	ui.employeeTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui.employeeTable->setSelectionMode(QAbstractItemView::NoSelection);
	ui.employeeTable->resizeColumnsToContents();
	ui.employeeTable->horizontalHeader()->setStretchLastSection(true);
	ui.employeeTable->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(ui.employeeTable, &QTableWidget::cellClicked, this, [this](int row, int column) {
		ui.employeeTable->selectRow(row);
		ui.employeeTable->setCurrentItem(ui.employeeTable->item(row, column));
		});
	connect(ui.filterEmployee, &QLineEdit::textChanged, this, &BulletinBoardForm::filterEmployeeList);
}

void BulletinBoardForm::filterEmployeeList() {
	QString filterText = ui.filterEmployee->text().toLower();  

	for (int i = 0; i < ui.employeeTable->rowCount(); ++i) {
		QString userInfo = ui.employeeTable->item(i, 1)->text().toLower();  

		if (userInfo.contains(filterText)) {
			ui.employeeTable->showRow(i);  
		}
		else {
			ui.employeeTable->hideRow(i);  
		}
	}
}

void BulletinBoardForm::loadDepartment() {
	QList<Department> departments = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->selectAll(false);
	ui.departmentComboBox->clear();

	for (const Department& department : departments) {
		ui.departmentComboBox->addItem(department.getName(), department.getDepartmentId());
	}
}

void BulletinBoardForm::handleFormAction(const QString& action, int id) {
	currentAction = action;
	bulletinBoardId = id;

	loadDepartment();
	loadEmployee();
	if (currentAction == "Add") {
		selectedUserIds.clear();
	}
	else if (currentAction == "Edit") {
		selectedUserIds.clear();
		BulletinBoard existingBulletinBoard = DatabaseHelper::getDatabaseInstance()->getBulletinBoardRepository()->selectById(bulletinBoardId);
		ui.titleLineEdit->setText(existingBulletinBoard.getTitle());
		ui.contentTextEdit->setText(existingBulletinBoard.getContent());
		ui.activatedCheckbox->setChecked(existingBulletinBoard.getIsActive());
		ui.highPriorityCheckbox->setChecked(existingBulletinBoard.getIsHighPriority());

		ui.allRadio->setChecked(existingBulletinBoard.getToEmployee() == "All");
		ui.departmentRadio->setChecked(existingBulletinBoard.getToEmployee() == "Department");
		ui.selectionRadio->setChecked(existingBulletinBoard.getToEmployee() == "Selections");

		qint64 startDateTimestamp = existingBulletinBoard.getStartDate();
		qint64 endDateTimestamp = existingBulletinBoard.getEndDate();

		QDateTime startDateTime = QDateTime::fromSecsSinceEpoch(startDateTimestamp);
		QDateTime endDateTime = QDateTime::fromSecsSinceEpoch(endDateTimestamp);

		ui.startDateEdit->setDate(startDateTime.date());
		ui.endDateEdit->setDate(endDateTime.date());
		QList<UserBulletinBoard> userBulletinBoards = DatabaseHelper::getDatabaseInstance()->getUserBulletinBoardRepository()->selectByBulletinBoardId(bulletinBoardId);
		if (existingBulletinBoard.getToEmployee() == "Department") {
			User existingUser = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectById(userBulletinBoards[0].getUserId());
			int departmentId = existingUser.getDepartmentId();
			for (int i = 0; i < ui.departmentComboBox->count(); ++i) {
				if (ui.departmentComboBox->itemData(i).toInt() == departmentId) {
					ui.departmentComboBox->setCurrentIndex(i);
					break;
				}
			}
		}
		else if (existingBulletinBoard.getToEmployee() == "Selections") {
			for (const UserBulletinBoard& ubb : userBulletinBoards) {
				if (!selectedUserIds.contains(ubb.getUserId())) {
					selectedUserIds.append(ubb.getUserId());
				}
			}
			for (int row = 0; row < ui.employeeTable->rowCount(); ++row) {
				QCheckBox* checkbox = qobject_cast<QCheckBox*>(ui.employeeTable->cellWidget(row, 0)->findChild<QCheckBox*>());
				if (checkbox) {
					QString userId = ui.employeeTable->item(row, 1)->text().split("(").last().split(")").first();
					if (selectedUserIds.contains(userId)) {
						checkbox->setChecked(true);
					}
				}
			}
		}
	}
}

void BulletinBoardForm::btnOkClicked() {
	int departmentId = 0;
	BulletinBoard bulletinBoard;
	bulletinBoard.setTitle(ui.titleLineEdit->text());
	bulletinBoard.setContent(ui.contentTextEdit->toPlainText());

	QDate startDate = ui.startDateEdit->date();
	QDate endDate = ui.endDateEdit->date();

	QDateTime startDateTime = QDateTime(startDate.startOfDay());
	QDateTime endDateTime = QDateTime(endDate.startOfDay());

	qint64 startTimestamp = startDateTime.toSecsSinceEpoch();
	qint64 endTimestamp = endDateTime.toSecsSinceEpoch();

	bulletinBoard.setStartDate(startTimestamp);
	bulletinBoard.setEndDate(endTimestamp);

	bulletinBoard.setIsActive(ui.activatedCheckbox->isChecked() ? 1 : 0);
	bulletinBoard.setIsHighPriority(ui.highPriorityCheckbox->isChecked() ? 1 : 0);

	if (ui.allRadio->isChecked()) {
		bulletinBoard.setToEmployee("All");
	}
	else if (ui.departmentRadio->isChecked()) {
		departmentId = ui.departmentComboBox->currentData().toInt();
		bulletinBoard.setToEmployee("Department");
	}
	else {
		bulletinBoard.setToEmployee("Selections");
	}

	if (currentAction == "Add") {
		BulletinBoard newBulletinBoard = DatabaseHelper::getDatabaseInstance()->getBulletinBoardRepository()->insert(bulletinBoard);
		bulletinBoard.setBulletinBoardId(newBulletinBoard.getBulletinBoardId());
	}
	else if (currentAction == "Edit") {
		bulletinBoard.setBulletinBoardId(bulletinBoardId);
		DatabaseHelper::getDatabaseInstance()->getBulletinBoardRepository()->update(bulletinBoard);
		DatabaseHelper::getDatabaseInstance()->getUserBulletinBoardRepository()->deleteByBulletinBoardId(bulletinBoard.getBulletinBoardId());
	}

	if (bulletinBoard.getToEmployee() == "All") {
		QList<User> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectAll();
		for (User user : users) {
			UserBulletinBoard userBulletinBoard;
			userBulletinBoard.setBulletinBoardId(bulletinBoard.getBulletinBoardId());
			userBulletinBoard.setUserId(user.getUserId());
			if (DatabaseHelper::getDatabaseInstance()->getUserBulletinBoardRepository()->insert(userBulletinBoard)) {
				qDebug() << "Insert user bulletin board with user ID = " << user.getUserId() << " successfully!\n";
			}
		}
	}
	else if (bulletinBoard.getToEmployee() == "Department") {
		QList<QVariantList> users = DatabaseHelper::getDatabaseInstance()->getUserRepository()->selectByDepartmentId(departmentId);
		for (const QVariantList& userData : users) {
			QString userId = userData.at(0).toString();

			UserBulletinBoard userBulletinBoard;
			userBulletinBoard.setBulletinBoardId(bulletinBoard.getBulletinBoardId());
			userBulletinBoard.setUserId(userId);

			if (DatabaseHelper::getDatabaseInstance()->getUserBulletinBoardRepository()->insert(userBulletinBoard)) {
				qDebug() << "Insert user bulletin board with user ID = " << userId << " successfully!\n";
			}
		}
	}
	else {
		for (QString userId : selectedUserIds) {
			UserBulletinBoard userBulletinBoard;
			userBulletinBoard.setBulletinBoardId(bulletinBoard.getBulletinBoardId());
			userBulletinBoard.setUserId(userId);
			if (DatabaseHelper::getDatabaseInstance()->getUserBulletinBoardRepository()->insert(userBulletinBoard)) {
				qDebug() << "Insert user bulletin board with user ID = " << userId << " successfully!\n";
			}
		}
	}
	emit bulletinBoardChanged();
	this->close();
}

void BulletinBoardForm::btnCancelClicked() {
	this->close();
}