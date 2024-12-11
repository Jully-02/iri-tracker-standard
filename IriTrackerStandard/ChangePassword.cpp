#include "ChangePassword.h"
#include <QLinearGradient>
#include <QPalette>
#include <QIcon>
#include "DatabaseHelper.h"
#include "FunctionPublic.h"
ChangePassword::ChangePassword(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle("Change Password");

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.oldPasswordEdit, &QLineEdit::textChanged, this, &ChangePassword::checkInputFields);
	connect(ui.newPasswordEdit, &QLineEdit::textChanged, this, &ChangePassword::checkInputFields);
	connect(ui.retypeEdit, &QLineEdit::textChanged, this, &ChangePassword::checkInputFields);

	connect(ui.btnOK, &QPushButton::clicked, this, &ChangePassword::btnOkClicked);
	connect(ui.btnCancel, &QPushButton::clicked, this, &ChangePassword::btnOkClicked);

    ui.oldPasswordEdit->setEchoMode(QLineEdit::Password);
    ui.newPasswordEdit->setEchoMode(QLineEdit::Password);
    ui.retypeEdit->setEchoMode(QLineEdit::Password);

	ui.btnOK->setEnabled(false);
    ui.errorLabel->clear();

}

ChangePassword::~ChangePassword()
{}

void ChangePassword::btnOkClicked() {
    if (DatabaseHelper::getDatabaseInstance()->getUserRepository()->changePasswordAdmin(FunctionPublic::hashPassword(ui.oldPasswordEdit->text()),FunctionPublic::hashPassword(ui.newPasswordEdit->text()))) {
        this->close();
    }
    else {
        ui.errorLabel->setText("    Password was wrong");
        ui.errorLabel->setStyleSheet("background-color: red; color: white;");
    }
}

void ChangePassword::checkInputFields() {
    // Kiểm tra độ dài của các trường
    bool enableButton = ui.oldPasswordEdit->text().length() >= 4 &&
        ui.newPasswordEdit->text().length() >= 4 &&
        ui.retypeEdit->text().length() >= 4;

    // Cập nhật trạng thái nút btnOK
    ui.btnOK->setEnabled(enableButton);

    // Lấy giá trị từ các trường nhập liệu
    QString oldPassword = ui.oldPasswordEdit->text();
    QString newPassword = ui.newPasswordEdit->text();
    QString retypePassword = ui.retypeEdit->text();

    // Kiểm tra nếu newPassword giống oldPassword
    if (newPassword == oldPassword) {
        ui.errorLabel->setText("    New password and old password are the same");
        ui.errorLabel->setStyleSheet("background-color: red; color: white;");
        ui.btnOK->setEnabled(false);
    }
    // Kiểm tra nếu retypePassword không giống newPassword
    else if (retypePassword != newPassword) {
        ui.errorLabel->setText("    The retype password is not the same as the new password");
        ui.errorLabel->setStyleSheet("background-color: red; color: white;");
        ui.btnOK->setEnabled(false);
    }
    // Nếu không có lỗi, ẩn errorLabel
    else {
        ui.errorLabel->clear();
        ui.errorLabel->setStyleSheet(""); 
    }
}

void ChangePassword::btnCancelClicked() {
	this->close();
}