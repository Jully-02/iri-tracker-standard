#include "SetAdministratorPassword.h"
#include <QLinearGradient>
#include <QPalette>
#include <QMessageBox> 
#include <QRegularExpression>
#include <QIcon>
#include <QDateTime>
#include <QSqlQuery>
#include <QCryptographicHash>
#include <QDebug>
#include "User.h"
#include "DatabaseHelper.h"
#include <QSqlError>

SetAdministratorPassword::SetAdministratorPassword(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle("Set Administrator Password");

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon); 

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2")); 

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	ui.btnOK->setEnabled(false);

	ui.passwordEdit->setEchoMode(QLineEdit::Password);
	ui.retypePasswordEdit->setEchoMode(QLineEdit::Password);

	connect(ui.passwordEdit, &QLineEdit::textChanged, this, &SetAdministratorPassword::validateInput);
	connect(ui.retypePasswordEdit, &QLineEdit::textChanged, this, &SetAdministratorPassword::validateInput);
	connect(ui.emailEdit, &QLineEdit::textChanged, this, &SetAdministratorPassword::validateInput);

	connect(ui.btnCancel, &QPushButton::clicked, this, &QWidget::close);
	connect(ui.btnOK, &QPushButton::clicked, this, &SetAdministratorPassword::onBtnOkClicked);
}

SetAdministratorPassword::~SetAdministratorPassword()
{}

void SetAdministratorPassword::validateInput() {
	QString password = ui.passwordEdit->text();
	QString retypePassword = ui.retypePasswordEdit->text();
	QString email = ui.emailEdit->text();

	bool isPasswordValid = password.length() >= 4;

	bool isPasswordMatch = (password == retypePassword);

	QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-z]{2,3}$");
	bool isEmailValid = emailRegex.match(email).hasMatch();

	if (isPasswordValid && isPasswordMatch && isEmailValid) {
		ui.btnOK->setEnabled(true);
	}
	else {
		ui.btnOK->setEnabled(false);
	}
}

QString SetAdministratorPassword::hashPassword(const QString& password) {
	QByteArray byteArray = password.toUtf8(); 
	QByteArray hashed = QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256);
	return QString(hashed.toHex()); 
}

void SetAdministratorPassword::onBtnOkClicked() {
    QString password = ui.passwordEdit->text();
    QString retypePassword = ui.retypePasswordEdit->text();
    QString email = ui.emailEdit->text();

    if (password.length() < 4) {
        QMessageBox::warning(this, "Error", "Password must be at least 4 characters.");
        return;
    }

    if (password != retypePassword) {
        QMessageBox::warning(this, "Error", "Passwords do not match!");
        return;
    }

    QRegularExpression emailRegex("^[\\w\\.]+@[\\w\\.]+\\.[a-z]{2,3}$");
    if (!emailRegex.match(email).hasMatch()) {
        QMessageBox::warning(this, "Error", "Invalid email address!");
        return;
    }

    QString userId = "admin";
    int roleId = 1;
	int departmentId = 1;
    int isActive = 1;
    QString createdAt = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    QString updatedAt = createdAt;

	User user;
	user.setUserId(userId);
	user.setRoleId(1);
	user.setDepartmentId(1);
	user.setIsActive(1);
	user.setPassword(hashPassword(password));
	user.setEmail(email);
	user.setIsPassword(1);

	bool isCheck = DatabaseHelper::getDatabaseInstance()->getUserRepository()->insert(user);

    if (isCheck) {
		ui.statusLabel->setStyleSheet("background-color: green; color: white;");
        ui.statusLabel->setText(QDateTime::currentDateTime().toString(" [yyyy-MM-dd hh:mm:ss AP]") + " Register successfully!");
        emit adminCreated();
    }
    else {
		ui.statusLabel->setStyleSheet("background-color: red; color: white;");
        ui.statusLabel->setText(QDateTime::currentDateTime().toString(" [yyyy-MM-dd hh:mm:ss AP]") + " Register failed! ");
    }
}

