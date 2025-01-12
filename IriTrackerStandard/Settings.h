#pragma once

#include <QWidget>
#include "ui_Settings.h"

class Settings : public QWidget
{
	Q_OBJECT

signals:
	void languageChanged(const QString& language);

public:
	Settings(QWidget *parent = nullptr);
	~Settings();

	void btnOkClicked();
	void btnCancelClicked();

private:
	Ui::SettingsClass ui;
};
