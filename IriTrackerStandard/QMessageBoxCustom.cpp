#include "QMessageBoxCustom.h"
#include <QLinearGradient>
#include <QPalette>

QMessageBoxCustom::QMessageBoxCustom(QWidget *parent)
	: QDialog(parent)
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

	connect(ui.btnOk, &QPushButton::clicked, this, &QMessageBoxCustom::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &QMessageBoxCustom::btnCancelClicked);

	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
}

QMessageBoxCustom::~QMessageBoxCustom()
{}

void QMessageBoxCustom::setQuestionLabel(const QString& text) {
	ui.questionLabel->setWordWrap(true);
	ui.questionLabel->setText(text);
}

void QMessageBoxCustom::setWindowTitleCustom(const QString& title) {
	setWindowTitle(title);
}


void QMessageBoxCustom::btnCancelClicked() {
	this->reject();
}

void QMessageBoxCustom::btnOkClicked() {
	this->accept();
}
