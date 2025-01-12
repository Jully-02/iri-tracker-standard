#include "Login.h"
#include <QLinearGradient>
#include <QPalette>
#include <QMessageBox> 
#include <QRegularExpression>
#include <QIcon>
#include <QSqlQuery>
#include <QDateTime>
#include <QCryptographicHash>
#include <QSqlError>

Login::Login(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle(tr("Log In"));

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	ui.passwordEdit->setEchoMode(QLineEdit::Password);

	connect(ui.btnCancel, &QPushButton::clicked, this, &QWidget::close);
	connect(ui.btnOK, &QPushButton::clicked, this, &Login::onBtnOkClicked);
	connect(ui.btnForgotPassword, &QPushButton::clicked, this, &Login::onBtnForgotPasswordClicked);
}

Login::~Login()
{}

QString Login::hashPassword(const QString& password) {
    QByteArray byteArray = password.toUtf8();
    QByteArray hashed = QCryptographicHash::hash(byteArray, QCryptographicHash::Sha256);
    return QString(hashed.toHex());
}

void Login::onBtnOkClicked() {
    QString password = ui.passwordEdit->text();

    if (password.length() < 4) {
        ui.statusLabel->setStyleSheet("background-color: red; color: white;");
        ui.statusLabel->setText(QDateTime::currentDateTime().toString(" [yyyy-MM-dd hh:mm:ss AP]") + "Password is incorrect!");
        return;
    }

    QString storedPassword; 

    QSqlQuery query;
    query.prepare("SELECT password FROM user WHERE role_id = :role_id");
    query.bindValue(":role_id", 1);

    if (query.exec()) {
        if (query.next()) {
            storedPassword = query.value(0).toString();
        }
    }
    else {
        ui.statusLabel->setStyleSheet("background-color: red; color: white;");
        ui.statusLabel->setText(QDateTime::currentDateTime().toString(" [yyyy-MM-dd hh:mm:ss AP]") + "Database query failed:" + query.lastError().text());
        return;
    }

    if (storedPassword == hashPassword(password)) {
        emit loginSuccessful(); 
        this->close();
    }
    else {
        ui.statusLabel->setStyleSheet("background-color: red; color: white;");
        ui.statusLabel->setText(QDateTime::currentDateTime().toString(" [yyyy-MM-dd hh:mm:ss AP]") + "Password is incorrect!");
    }
}


void Login::onBtnForgotPasswordClicked() {
	QMessageBox::information(this, "Notice", "You just clicked Forgot Password Button");
	return;
}
