#include "Settings.h"

Settings::Settings(QWidget *parent)
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

    connect(ui.btnOk, &QPushButton::clicked, this, &Settings::btnOkClicked);
    connect(ui.btnCancel, &QPushButton::clicked, this, &Settings::btnCancelClicked);

    ui.languageComboBox->addItem("English", "en");
    ui.languageComboBox->addItem("Vietnamese", "vi");

    ui.languageComboBox->setCurrentIndex(0);
}

Settings::~Settings()
{}

void Settings::btnOkClicked() {
    QString selectedLanguage = ui.languageComboBox->currentData().toString();
    emit languageChanged(selectedLanguage);
    this->close();
}

void Settings::btnCancelClicked() {
    this->close();
}