#pragma once

#include <QWidget>
#include "ui_Login.h"

class Login : public QWidget
{
	Q_OBJECT

public:
	Login(QWidget *parent = nullptr);
	~Login();

signals:
	void loginSuccessful();

private:
	void onBtnOkClicked();
	void onBtnForgotPasswordClicked();
	QString hashPassword(const QString& password);

private:
	Ui::LoginClass ui;
};
