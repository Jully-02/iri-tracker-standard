#pragma once

#include <QDialog>
#include "ui_Logout.h"

class Logout : public QDialog
{
	Q_OBJECT

public:
	Logout(QWidget *parent = nullptr);
	~Logout();

private: 
	void btnOkClicked();
	void btnCancelClicked();

private:
	Ui::LogoutClass ui;
};
