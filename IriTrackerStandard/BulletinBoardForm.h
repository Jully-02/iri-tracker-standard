#pragma once

#include <QWidget>
#include "ui_BulletinBoardForm.h"

class BulletinBoardForm : public QWidget
{
	Q_OBJECT

public:
	BulletinBoardForm(QWidget *parent = nullptr);
	~BulletinBoardForm();

signals:
	void bulletinBoardChanged();

public slots:
	void handleFormAction(const QString& action, int id = -1);

private:
	void onRadioChanged();
	void onInputChanged();

	void btnOkClicked();
	void btnCancelClicked();
	void loadEmployee();
	void loadDepartment();
	void filterEmployeeList();


private:
	Ui::BulletinBoardFormClass ui;
	QString currentAction;
	int bulletinBoardId;
	QList<QString> selectedUserIds;
};
