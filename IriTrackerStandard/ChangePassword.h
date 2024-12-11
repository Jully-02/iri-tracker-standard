#pragma once

#include <QWidget>
#include "ui_ChangePassword.h"

class ChangePassword : public QWidget
{
	Q_OBJECT

public:
	ChangePassword(QWidget *parent = nullptr);
	~ChangePassword();

	void btnOkClicked();
	void btnCancelClicked();
	void checkInputFields();

private:
	Ui::ChangePasswordClass ui;
};
