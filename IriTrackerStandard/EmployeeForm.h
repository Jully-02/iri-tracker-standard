#pragma once

#include <QWidget>
#include "ui_EmployeeForm.h"
#include <QTimer>
#include "IriTracker.h"
#include <QThread>

enum class EyeSide {
	Left,
	Right,
	Unknow
};

class EmployeeForm : public QWidget
{
	Q_OBJECT

public:
	EmployeeForm(QWidget *parent = nullptr);
	~EmployeeForm();
	void startIrisCapture();

	Ui::EmployeeFormClass getUi();


signals:
	void employeeChanged();

public slots:
	void handleFormAction(const QString& action, QString id = "");
	void onImageProcessed(unsigned char* imageData,
		int imageLen,
		int imageWidth,
		int imageHeight);
	void onPathTemplate(unsigned char* data, int size);
	void processStreaming();

	void changeImageDevice(bool isDevice);
private: 
	void btnOkClicked();
	void btnCancelClicked();
	void btnBrowseClicked();
	void btnClearPhotoClicked();
	void checkInputs();
	void updatePasswordFields();

	bool eventFilter(QObject* obj, QEvent* event);


private:
	Ui::EmployeeFormClass ui;
	IriTracker* iriTracker;
	QString currentAction;
	QString userId;
	EyeSide eyeSide = EyeSide::Unknow;
	QByteArray eyeRight = "";
	QByteArray eyeLeft = "";

	QThread* threadStream;

	bool device = false;
};
