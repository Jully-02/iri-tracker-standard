#pragma once

#include <QWidget>
#include "ui_ChangeDatabase.h"

class ChangeDatabase : public QWidget
{
	Q_OBJECT

public:
	ChangeDatabase(QWidget *parent = nullptr);
	~ChangeDatabase();

	void btnBrowseFileClicked();
	void changeDBRadio();

	void btnConnectClicked();


	void changeLineEdit();

private:
	Ui::ChangeDatabaseClass ui;
};
