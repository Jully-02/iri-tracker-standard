#include "ExceptionForm.h"
#include <QIcon>
#include <QLinearGradient>
#include <QPalette>
#include "Exception.h"
#include "DatabaseHelper.h"

ExceptionForm::ExceptionForm(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.btnOk, &QPushButton::clicked, this, &ExceptionForm::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &ExceptionForm::btnCancelClicked);
	connect(ui.exceptionNameEdit, &QLineEdit::textChanged, this, &ExceptionForm::checkInputFields);

	ui.btnOk->setEnabled(false);
}

ExceptionForm::~ExceptionForm()
{}

void ExceptionForm::handleFormAction(const QString& action, int id) {
	currentAction = action;
	exceptionId = id;

	Exception exception = DatabaseHelper::getDatabaseInstance()->getExceptionRepository()->selectById(exceptionId);
	if (action == "Edit") {
		ui.exceptionNameEdit->setText(exception.getName());

		ui.paidCoeffSpinBox->setValue(exception.getPaidCoefficient());
		ui.workCoeffSpinBox->setValue(exception.getWorkCoefficient());

		// Convert from qint64 to QTime to set the time
		QTime time = QTime::fromMSecsSinceStartOfDay(static_cast<int>(exception.getPaidHours() * 1000));
		ui.paidHourTime->setTime(time);

		// Set overtime status if necessary
		ui.isOverTimeCheckBox->setChecked(exception.getIsOvertime() != 0);
	}
	else if (action == "Add") {
		ui.exceptionNameEdit->setText("");
		ui.paidCoeffSpinBox->setValue(1.00);
		ui.workCoeffSpinBox->setValue(0.00);
		ui.paidHourTime->setTime(QTime(0, 0));
		ui.isOverTimeCheckBox->setChecked(false);
	}
} 

void ExceptionForm::btnOkClicked() {
	Exception exception;
	exception.setName(ui.exceptionNameEdit->text());
	exception.setPaidCoefficient(ui.paidCoeffSpinBox->value());
	exception.setWorkCoefficient(ui.workCoeffSpinBox->value());

	// Convert from QTime to qint64 to save hours
	QTime time = ui.paidHourTime->time();
	qint64 paidHours = static_cast<qint64>(time.msecsSinceStartOfDay() / 1000); // Convert from milliseconds to seconds
	exception.setPaidHours(paidHours);

	// Get overtime status
	exception.setIsOvertime(ui.isOverTimeCheckBox->isChecked() ? 1 : 0);

	bool success = false;
	if (currentAction == "Add") {
		success = DatabaseHelper::getDatabaseInstance()->getExceptionRepository()->insert(exception);
	}
	else if (currentAction == "Edit") {
		exception.setExceptionId(exceptionId);
		success = DatabaseHelper::getDatabaseInstance()->getExceptionRepository()->update(exception);
	}

	if (success) {
		if (currentAction == "Add") {
			qDebug() << "Exception added successfully!";
		}
		else if (currentAction == "Edit") {
			qDebug() << "Exception updated successfully!";
		}
		emit exceptionChanged();
		this->close();
	}
	else {
		qDebug() << "Error saving exception!";
	}
}

void ExceptionForm::btnCancelClicked() {
	this->close();
}

void ExceptionForm::checkInputFields() {
	if (ui.exceptionNameEdit->text().trimmed().length() == 0) {
		ui.btnOk->setEnabled(false);
	}
	else {
		ui.btnOk->setEnabled(true);
	}
}
