#pragma once

#include <QWidget>
#include "ui_EmployeeBulletins.h"

class EmployeeBulletins : public QWidget
{
	Q_OBJECT

public:
	EmployeeBulletins(QWidget *parent = nullptr);
	~EmployeeBulletins();

public slots:
	void handleEmit(const QString& userId);

private:
	Ui::EmployeeBulletinsClass ui;
};
