#pragma once

#include <QWidget>
#include "ui_ExceptionForm.h"

class ExceptionForm : public QWidget
{
	Q_OBJECT

public:
	ExceptionForm(QWidget *parent = nullptr);
	~ExceptionForm();

signals:
	void exceptionChanged();

public slots:
	void handleFormAction(const QString& action, int id = -1);

private:
	void btnOkClicked();
	void btnCancelClicked();
	void checkInputFields();

private:
	Ui::ExceptionFormClass ui;

	QString currentAction;
	int exceptionId;
};
