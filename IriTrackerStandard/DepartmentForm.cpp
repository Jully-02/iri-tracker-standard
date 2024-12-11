#include "DepartmentForm.h"
#include <QIcon>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include "Department.h"
#include "DatabaseHelper.h"

DepartmentForm::DepartmentForm(QWidget *parent)
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

	connect(ui.btnOK, &QPushButton::clicked, this, &DepartmentForm::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &DepartmentForm::btnCancelClicked);
}

DepartmentForm::~DepartmentForm()
{}

void DepartmentForm::handleFormAction(const QString& action, int id, const QString& name, const QString& desc) {
	currentAction = action;
	departmentId = id;

	if (action == "Edit") {
		ui.nameEdit->setText(name);
		ui.nameEdit->setDisabled(true);
		ui.descText->setText(desc);
	}
	else if (action == "Add") {
		ui.nameEdit->clear();
		ui.descText->clear();
	}
}

void DepartmentForm::btnOkClicked() {
	QString name = ui.nameEdit->text();
	QString desc = ui.descText->toPlainText();

	if (name.isEmpty()) {
		qDebug() << "Tên department không được để trống!";
		return;
	}

	bool success = false;

	Department department;
	department.setDepartmentId(departmentId);
	department.setName(name);
	department.setDesc(desc);

	if (currentAction == "Add") {
		success = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->insert(department);
	}
	else if (currentAction == "Edit") {
		success = DatabaseHelper::getDatabaseInstance()->getDepartmentRepository()->update(department);
	}

	if (success) {
		if (currentAction == "Add") {
			qDebug() << "Thêm department thành công!";
		}
		else if (currentAction == "Edit") {
			qDebug() << "Cập nhật department thành công!";
		}
		emit departmentChanged();
		this->close();
	}
	else {
		qDebug() << "Lỗi khi lưu department!";
	}
}


void DepartmentForm::btnCancelClicked() {
	this->close();
}
