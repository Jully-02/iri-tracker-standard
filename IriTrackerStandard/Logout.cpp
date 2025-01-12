#include "Logout.h"
#include <QLinearGradient>
#include <QPalette>

Logout::Logout(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	this->setWindowTitle(tr("Log Out"));

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.btnOk, &QPushButton::clicked, this, &Logout::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &Logout::btnCancelClicked);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

Logout::~Logout()
{}

void Logout::btnCancelClicked() {
	this->reject();
}

void Logout::btnOkClicked() {
	this->accept();
}
