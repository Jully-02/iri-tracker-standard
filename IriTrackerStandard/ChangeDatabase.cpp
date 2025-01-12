#include "ChangeDatabase.h"
#include <QFileDialog>
#include "DatabaseHelper.h"
#include <QFileInfo>
#include <QDebug>

ChangeDatabase::ChangeDatabase(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	this->setWindowTitle(tr("Change Database"));

	QIcon logoIcon("../icons/logo.png");
	this->setWindowIcon(logoIcon);

	QLinearGradient gradient(0, 0, 0, this->height());
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(1.0, QColor("#87A8D2"));

	QPalette palette;
	palette.setBrush(QPalette::Window, gradient);
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	connect(ui.btnBrowseFile, &QPushButton::clicked, this, &ChangeDatabase::btnBrowseFileClicked);
	if (!DatabaseHelper::getDatabaseName().isEmpty()) {
		QFileInfo fileInfo(DatabaseHelper::getDatabaseName());
		ui.pathDBLineEdit->setText(fileInfo.absoluteFilePath());
	}

	connect(ui.sqliteRadio, &QRadioButton::toggled, this, &ChangeDatabase::changeDBRadio);
	connect(ui.mysqlRadio, &QRadioButton::toggled, this, &ChangeDatabase::changeDBRadio);

	connect(ui.pathDBLineEdit, &QLineEdit::textChanged, this, &ChangeDatabase::changeLineEdit);
	connect(ui.hostNameLineEdit, &QLineEdit::textChanged, this, &ChangeDatabase::changeLineEdit);
	connect(ui.dbNameLineEdit, &QLineEdit::textChanged, this, &ChangeDatabase::changeLineEdit);
	connect(ui.userNameLineEdit, &QLineEdit::textChanged, this, &ChangeDatabase::changeLineEdit);
	connect(ui.passwordLineEdit, &QLineEdit::textChanged, this, &ChangeDatabase::changeLineEdit);

	connect(ui.btnConnectDB, &QPushButton::clicked, this, &ChangeDatabase::btnConnectClicked);

	if (DatabaseHelper::getCurrentDatabaseType() == DatabaseType::MySQL) {
		ui.mysqlRadio->setChecked(true);
		ui.pathDBLineEdit->setText("");
		ui.pathDBLineEdit->setDisabled(true);
		ui.dbNameLineEdit->setText(DatabaseHelper::getDatabase().databaseName());
		ui.hostNameLineEdit->setText(DatabaseHelper::getDatabase().hostName());
		ui.userNameLineEdit->setText(DatabaseHelper::getDatabase().userName());
		ui.passwordLineEdit->setText(DatabaseHelper::getDatabase().password());
	}
	else if (DatabaseHelper::getCurrentDatabaseType() == DatabaseType::SQLite) {
		ui.sqliteRadio->setChecked(true);
		ui.pathDBLineEdit->setText(DatabaseHelper::getDatabase().databaseName());
		ui.dbNameLineEdit->setText("");
		ui.hostNameLineEdit->setText("");
		ui.userNameLineEdit->setText("");
		ui.passwordLineEdit->setText("");
		ui.dbNameLineEdit->setDisabled(true);
		ui.hostNameLineEdit->setDisabled(true);
		ui.userNameLineEdit->setDisabled(true);
		ui.passwordLineEdit->setDisabled(true);
	}
}

ChangeDatabase::~ChangeDatabase()
{}

void ChangeDatabase::btnBrowseFileClicked() {
	// Open the file selection dialog box
	QString filePath;

	if (ui.createNewRadio->isChecked()) {
		// Open dialog to select folder to save new file
		QString directory = QFileDialog::getExistingDirectory(
			this,                           // Parent widget
			tr("Select Directory"),        // Title of the dialog
			QString()                      // Default directory
		);

		// Check if the user has selected a directory
		if (!directory.isEmpty()) {
			// Create full path with default filename
			filePath = QDir(directory).filePath("iri-tracker-standard.db");
		}
	}
	else if (ui.exisitngDBRadio->isChecked()){
		// Open dialog to select existing file
		filePath = QFileDialog::getOpenFileName(
			this,                                   // Parent widget
			tr("Select Database File"),            // Title of the dialog
			QString(),                             // Default directory
			tr("Database Files (*.db *.sqlite *.sql);;All Files (*)") // File filter
		);
	}

	// If path is not empty, append pathDBLineEdit
	if (!filePath.isEmpty()) {
		ui.pathDBLineEdit->setText(filePath);
	}
}

void ChangeDatabase::changeDBRadio() {
	if (ui.sqliteRadio->isChecked()) {
		if (ui.pathDBLineEdit->text().isEmpty()) {
			ui.btnConnectDB->setDisabled(true);
		}
		else {
			ui.btnConnectDB->setDisabled(false);
		}
		ui.pathDBLineEdit->setDisabled(false);
		ui.hostNameLineEdit->setDisabled(true);
		ui.dbNameLineEdit->setDisabled(true);
		ui.userNameLineEdit->setDisabled(true);
		ui.passwordLineEdit->setDisabled(true);
	}
	else if (ui.mysqlRadio->isChecked()) {
		ui.btnConnectDB->setDisabled(true);
		ui.hostNameLineEdit->setDisabled(false);
		ui.dbNameLineEdit->setDisabled(false);
		ui.userNameLineEdit->setDisabled(false);
		ui.passwordLineEdit->setDisabled(false);
		ui.pathDBLineEdit->setDisabled(true);
	}
}

void ChangeDatabase::changeLineEdit() {
	if (ui.sqliteRadio->isChecked()) {
		if (ui.pathDBLineEdit->text().isEmpty()) {
			ui.btnConnectDB->setDisabled(true);
		}
		else {
			ui.btnConnectDB->setDisabled(false);
		}
	}
	else if (ui.mysqlRadio->isChecked()) {
		if (ui.hostNameLineEdit->text().isEmpty() || ui.dbNameLineEdit->text().isEmpty() || ui.userNameLineEdit->text().isEmpty()) {
			ui.btnConnectDB->setDisabled(true);
		}
		else {
			ui.btnConnectDB->setDisabled(false);
		}
	}
}

void ChangeDatabase::btnConnectClicked() {
	if (ui.sqliteRadio->isChecked()) {
		bool isCheck = DatabaseHelper::changeDatabase(DatabaseType::SQLite, ui.pathDBLineEdit->text());
		if (isCheck) {
			this->close();
		}
		else {
			ui.errorLabel->setText("  Database connection failed!");
			ui.errorLabel->setStyleSheet("QLabel { background-color: red; color: white; padding: 5px; }");
		}
	}
	else if (ui.mysqlRadio->isChecked()) {
		bool isCheck = DatabaseHelper::changeDatabase(DatabaseType::MySQL,
			ui.dbNameLineEdit->text(),
			ui.hostNameLineEdit->text(),
			ui.userNameLineEdit->text(),
			ui.passwordLineEdit->text());
		if (isCheck) {
			this->close();
		}
		else {
			ui.errorLabel->setText("  Database connection failed!");
			ui.errorLabel->setStyleSheet("QLabel { background-color: red; color: white; padding: 5px; }");
		}
	}
}