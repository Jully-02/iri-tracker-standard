#pragma once

#include <QWidget>
#include "ui_DepartmentForm.h"

class DepartmentForm : public QWidget
{
	Q_OBJECT

public:
	DepartmentForm(QWidget *parent = nullptr);
	~DepartmentForm();

signals:
	void departmentChanged();

public slots:
	void handleFormAction(const QString& action, int id = -1, const QString& name = "", const QString& desc = "");

private: 
	void btnOkClicked();
	void btnCancelClicked();

private:
	Ui::DepartmentFormClass ui;
	QString currentAction;
	int departmentId;
	QString departmentName;
	QString departmentDesc;
};
