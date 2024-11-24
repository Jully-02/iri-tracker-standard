#pragma once

#include <QWidget>
#include "ui_ExceptionForm.h"

class ExceptionForm : public QWidget
{
	Q_OBJECT

public:
	ExceptionForm(QWidget *parent = nullptr);
	~ExceptionForm();

private:
	Ui::ExceptionFormClass ui;
};
