#include "ExceptionForm.h"
#include <QIcon>
#include <QLinearGradient>
#include <QPalette>

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
}

ExceptionForm::~ExceptionForm()
{}
