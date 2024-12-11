#pragma once

#include <QWidget>
#include "ui_AssignmentExceptionForm.h"

class AssignmentExceptionForm : public QWidget
{
	Q_OBJECT

public:
	AssignmentExceptionForm(QWidget *parent = nullptr);
	~AssignmentExceptionForm();

	void loadException();

private:
	Ui::AssignmentExceptionFormClass ui;
};
