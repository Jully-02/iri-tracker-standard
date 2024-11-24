#pragma once

#include <QWidget>
#include "ui_AttendanceEventDeleteForm.h"

class AttendanceEventDeleteForm : public QWidget
{
	Q_OBJECT

public:
	AttendanceEventDeleteForm(QWidget *parent = nullptr);
	~AttendanceEventDeleteForm();


public:
	void btnOkClicked();
	void btnCancelClicked();
	void handleFormAction(const QString& userId, int id);

signals: 
	void attendanceEventDeleteChanged();

private:
	Ui::AttendanceEventDeleteFormClass ui;
	QString selectUserId;
	int eventId;
};
