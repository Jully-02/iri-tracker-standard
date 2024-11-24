#pragma once

#include <QWidget>
#include "ui_SetAdministratorPassword.h"


class SetAdministratorPassword : public QWidget
{
	Q_OBJECT

public:
	SetAdministratorPassword(QWidget *parent = nullptr);
	~SetAdministratorPassword();

private:
	void onBtnOkClicked();
	void validateInput();
	QString hashPassword(const QString& password);

signals:
	void adminCreated();

private:
	Ui::SetAdministratorPasswordClass ui;
};
