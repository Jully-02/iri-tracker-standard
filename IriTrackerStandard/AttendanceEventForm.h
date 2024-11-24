#pragma once

#include <QWidget>
#include "ui_AttendanceEventForm.h"

class AttendanceEventForm : public QWidget
{
	Q_OBJECT

public:
	AttendanceEventForm(QWidget *parent = nullptr);
	~AttendanceEventForm();

signals:
	void attendanceEventChanged();

public slots:
	void handleFormAction(const QString& action, const QString& userId, int id = -1);
	void onDateOrTimeChanged();

public: 
	void btnOkClicked();
	void btnCancelClicked();
	void updateDateTimeFields();
	bool handleErrorInRadio(qint64 checkinTimestamp, qint64 currentTimestamp);
	bool handleErrorOutRadio(qint64 checkoutTimestamp, qint64 currentTimestamp);
	bool handleErrorInOutRadio(qint64 checkinTimestamp, qint64 checkoutTimestamp, qint64 currentTimestamp);

private:
	Ui::AttendanceEventFormClass ui;

	QString currentAction;
	int attendanceEventId;
	QString userIdEvent;
	qint64 checkinTimestampDB, checkoutTimestampDB;
};

