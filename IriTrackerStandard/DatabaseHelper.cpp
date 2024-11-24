#include "DatabaseHelper.h"
#include "ChangeDatabase.h"
#include <QProcess>

IDatabase* DatabaseHelper::dbInstance = nullptr;
QSqlDatabase DatabaseHelper::db;
QString DatabaseHelper::databaseName = "";

DatabaseHelper::DatabaseHelper() 
{
}

void DatabaseHelper::setDatabaseName(const QString& name) {
    databaseName = name;
}

QSqlDatabase DatabaseHelper::getDatabase() {
    return db;
}

QString DatabaseHelper::getDatabaseName() {
    return databaseName;
}

IDatabase* DatabaseHelper::getDatabaseInstance() {
    return dbInstance;
}

void DatabaseHelper::closeDatabase() {
    if (db.isOpen()) {
        if (dbInstance != nullptr) {
            delete dbInstance;
            dbInstance = nullptr;
        }

        if (db.isOpenError()) {
            qDebug() << "Database has open errors: " << db.lastError().text();
        }

        QSqlQuery query(db);
        if (!query.isActive()) {
            qDebug() << "No active query detected.";
        }

        db.close();
        QString connectionName = db.connectionName();
        db.close();
        db = QSqlDatabase(); // Reset object db
        QSqlDatabase::removeDatabase(connectionName);
    }
    else {
        if (db.isOpenError()) {
            qDebug() << "Failed to close database. Open error detected:"
                << db.lastError().text();
        }
        else {
            qDebug() << "Database is already closed.";
        }
    }
}

DatabaseType DatabaseHelper::getCurrentDatabaseType() {
    QString driver = db.driverName();

    if (driver == "QSQLITE") {
        return DatabaseType::SQLite;
    }
    else if (driver == "QMYSQL") {
        return DatabaseType::MySQL;
    }

    return DatabaseType::Unknown;
}

bool DatabaseHelper::initializeAndOpenDatabase() {
    QFile dbFile(databaseName);

    if (!dbFile.exists()) {
        qDebug() << "SQLite database file not found. Creating a new one.";

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(databaseName);

        if (db.open()) {
            qDebug() << "SQLite database created successfully.";
            createTable();
        }
        else {
            qDebug() << "Failed to create SQLite database:" << db.lastError().text();
            return false;
        }
    }
    else {
        qDebug() << "SQLite database file exists. Opening...";

        db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(databaseName);

        if (!db.open()) {
            qDebug() << "Failed to open existing SQLite database:" << db.lastError().text();
            return false;
        }
    }
    dbInstance = new SQLiteDatabase();
    qDebug() << "SQLite database initialized and opened successfully.";
    return true;
}

DatabaseHelper::~DatabaseHelper() {
    closeDatabase();
}

// Backup Database
bool DatabaseHelper::backupSQLite(const QString& dbPath, const QString& backupPath) {
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath); // Xóa file backup cũ nếu tồn tại
    }
    if (QFile::copy(dbPath, backupPath)) {
        qDebug() << "SQLite database backup successful!";
        return true;
    }
    else {
        qDebug() << "SQLite database backup failed!";
        return false;
    }
}

bool DatabaseHelper::backupMySQL(const QString& host, const QString& user, const QString& password, const QString& database, const QString& backupPath) {
    QString command = QString("mysqldump -h %1 -u%2 -p%3 %4 > %5")
        .arg(host)
        .arg(user)
        .arg(password)
        .arg(database)
        .arg(backupPath);

    QProcess process;
    process.start("cmd.exe", QStringList() << "/c" << command);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        qDebug() << "MySQL database backup successful!";
        return true;
    }
    else {
        qDebug() << "MySQL database backup failed!";
        qDebug() << process.readAllStandardError();
        return false;
    }
}

bool DatabaseHelper::backupDatabase(const QString& dbType, const QString& dbPath, const QString& backupPath,
    const QString& host, const QString& user, const QString& password, const QString& database) {
    if (dbType == "SQLite") {
        return backupSQLite(dbPath, backupPath);
    }
    else if (dbType == "MySQL") {
        return backupMySQL(host, user, password, database, backupPath);
    }
    else {
        qDebug() << "Unsupported database type!";
        return false;
    }
}

// Change Database
bool DatabaseHelper::changeDatabase(DatabaseType type,
    const QString& dbName,
    const QString& hostName,
    const QString& userName,
    const QString& password) {
    // Backup current database connection
    QSqlDatabase backupDb = db; 
    IDatabase* backupDbInstance = dbInstance;

    User user = DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkIfAdminExist();
    user.setDepartmentId(1);

    closeDatabase(); 

    db = QSqlDatabase::addDatabase((type == DatabaseType::MySQL) ? "QMYSQL" : "QSQLITE");
    if (DatabaseType::SQLite == type) {
        db.setDatabaseName(dbName);
    }

    if (type == DatabaseType::MySQL) {
        db.setHostName(hostName);
        db.setUserName(userName);
        db.setPassword(password);
    }

    if (!db.open()) {
        QSqlError error = db.lastError();
        qDebug() << "Failed to switch database!";
        qDebug() << "Error text: " << error.text();

        db = backupDb;
        dbInstance = backupDbInstance;

        if (db.isOpen()) {
            qDebug() << "Reverted to the previous database connection.";
        }
        else {
            qDebug() << "Failed to revert to the previous database connection! Attempting to reconnect...";

            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("../db/iri-tracker-standard.db");

            if (db.open()) {
                if (dbInstance == nullptr) {
                    dbInstance = new SQLiteDatabase();
                }
                qDebug() << "Reconnected to the default SQLite database.";
            }
            else {
                qDebug() << "Critical error: Unable to connect to the default SQLite database!";
                return false;
            }
        }

        return false;
    }

    qDebug() << "Database switched successfully!";
    if (db.driverName() == "QSQLITE") {
        if (dbInstance == nullptr) {
            dbInstance = new SQLiteDatabase();
        }
        QSqlQuery query(db);
        query.prepare("SELECT name FROM sqlite_master WHERE type='table';");

        if (!query.exec()) {
            qDebug() << "Error querying SQLite schema:" << query.lastError().text();
            return false;
        }

        if (!query.next()) {
            qDebug() << "The SQLite database exists but has no tables. Creating default tables...";

            createTable();
            DatabaseHelper::getDatabaseInstance()->getUserRepository()->insert(user);
            qDebug() << "Default tables created successfully.";
        }
    }

    else if (db.driverName() == "QMYSQL") {
        QSqlQuery query(db);

        if (!db.isOpen()) {
            if (!db.open()) {
                qDebug() << "Failed to open MySQL database!";
                return false;
            }
        }

        query.prepare("SELECT SCHEMA_NAME FROM information_schema.SCHEMATA WHERE SCHEMA_NAME = :dbName");
        query.bindValue(":dbName", dbName);

        if (!query.exec()) {
            qDebug() << "Failed to execute query:" << query.lastError().text();

            db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName("../db/iri-tracker-standard.db");

            if (db.open()) {
                if (dbInstance == nullptr) {
                    dbInstance = new SQLiteDatabase();
                }
                qDebug() << "Reconnected to the default SQLite database.";
                return true;
            }
            else {
                qDebug() << "Critical error: Unable to connect to the default SQLite database!";
                return false;
            }
        }

        if (!query.next()) {
            qDebug() << "Database" << dbName << "does not exist. Creating it...";

            QSqlQuery createDbQuery(db);
            if (!createDbQuery.exec(QString("CREATE DATABASE `%1`").arg(dbName))) {
                qDebug() << "Failed to create database:" << createDbQuery.lastError().text();

                db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName("../db/iri-tracker-standard.db");

                if (db.open()) {
                    if (dbInstance == nullptr) {
                        dbInstance = new SQLiteDatabase();
                    }
                    qDebug() << "Reconnected to the default SQLite database.";
                    return true;
                }
                else {
                    qDebug() << "Critical error: Unable to connect to the default SQLite database!";
                    return false;
                }
            }
            qDebug() << "Database created successfully.";

            // Đóng kết nối MySQL cũ và kết nối lại với cơ sở dữ liệu mới
            db.close();
            db.setDatabaseName(dbName);

            if (!db.open()) {
                qDebug() << "Failed to revert to the previous database connection! Attempting to reconnect...";

                db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName("../db/iri-tracker-standard.db");

                if (db.open()) {
                    if (dbInstance == nullptr) {
                        dbInstance = new SQLiteDatabase();
                    }
                    qDebug() << "Reconnected to the default SQLite database.";
                    return true;
                }
                else {
                    qDebug() << "Critical error: Unable to connect to the default SQLite database!";
                    return false;
                }
            }

            createTable();  
            DatabaseHelper::getDatabaseInstance()->getUserRepository()->insert(user);
            return true;

        }
        else {
            qDebug() << "Database" << dbName << "already exists.";
            db.close();
            db.setDatabaseName(dbName);

            if (!db.open()) {
                qDebug() << "Failed to revert to the previous database connection! Attempting to reconnect...";

                db = QSqlDatabase::addDatabase("QSQLITE");
                db.setDatabaseName("../db/iri-tracker-standard.db");

                if (db.open()) {
                    dbInstance = new SQLiteDatabase();
                    qDebug() << "Reconnected to the default SQLite database.";
                    return true;
                }
                else {
                    qDebug() << "Critical error: Unable to connect to the default SQLite database!";
                    return false;
                }
            }

            qDebug() << "Reverted to the previous database connection.";
            dbInstance = new MySQLDatabase();
            return true;
        }
    }

}

void DatabaseHelper::createTable()
{
    QFile file;
    if (db.driverName() == "QSQLITE") {
        file.setFileName("../db/iritracker-ini-sqlite.sql");
        if (dbInstance == nullptr) {
            dbInstance = new SQLiteDatabase();
        }
    }
    else if (db.driverName() == "QMYSQL") {
        file.setFileName("../db/iritracker-ini-mysql.sql");
        if (dbInstance == nullptr) {
            dbInstance = new MySQLDatabase();
        }
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open SQL file:";
        return;
    }

    QTextStream stream(&file);
    QString sqlCommands = stream.readAll();
    file.close();

    QSqlQuery query;
    QStringList sqlStatements = sqlCommands.split(";", Qt::SkipEmptyParts);

    for (const QString& statement : sqlStatements) {
        QString trimmedStatement = statement.trimmed();
        if (!trimmedStatement.isEmpty()) {
            if (!query.exec(trimmedStatement)) {
                qDebug() << "Error executing SQL statement:" << trimmedStatement
                    << "Error:" << query.lastError();
            }
            else {
                qDebug() << "Successfully executed SQL statement:" << trimmedStatement;
            }
        }
    }
}

//void DatabaseHelper::createTableForSQLite()
//{
//    QSqlQuery query;
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS role ("
//        "role_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "
//        "name TEXT NOT NULL UNIQUE, "
//        "description TEXT, "
//        "created_at TEXT, "
//        "updated_at TEXT);")) {
//        qDebug() << "Error creating role table:" << query.lastError();
//    }
//
//    query.prepare("INSERT OR IGNORE INTO role (name, description, created_at, updated_at) VALUES (?, ?, datetime('now'), datetime('now'))");
//    query.addBindValue("Admin");
//    query.addBindValue("admin");
//    if (!query.exec()) {
//        qDebug() << "Error inserting into role table:" << query.lastError();
//    }
//
//    query.addBindValue("User");
//    query.addBindValue("user");
//    if (!query.exec()) {
//        qDebug() << "Error inserting into role table:" << query.lastError();
//    }
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS department ("
//        "department_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "
//        "name TEXT NOT NULL UNIQUE, "
//        "description TEXT, "
//        "created_at TEXT, "
//        "updated_at TEXT, "
//        "is_active INTEGER);")) {
//        qDebug() << "Error creating department table:" << query.lastError();
//    }
//
//    query.prepare("INSERT OR IGNORE INTO department (name, description, created_at, updated_at, is_active) VALUES (?, ?, datetime('now'), datetime('now'), ?)");
//    query.addBindValue("Other");
//    query.addBindValue("other");
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS user ("
//        "user_id TEXT PRIMARY KEY NOT NULL UNIQUE, "
//        "first_name TEXT, "
//        "last_name TEXT, "
//        "is_password INTEGER, "
//        "password TEXT, "
//        "department_id INTEGER, "
//        "date_of_birth TEXT, "
//        "start_working_date INTEGER, "
//        "is_active INTEGER NOT NULL, "
//        "avatar BLOB, "
//        "email TEXT, "
//        "phone TEXT, "
//        "cell_phone TEXT, "
//        "address TEXT, "
//        "eye_right BLOB, "
//        "eye_left BLOB, "
//        "role_id INTEGER NOT NULL, "
//        "created_at TEXT, "
//        "updated_at TEXT, "
//        "FOREIGN KEY (department_id) REFERENCES department(department_id), "
//        "FOREIGN KEY (role_id) REFERENCES role(role_id))")) {
//        qDebug() << "Error creating user table:" << query.lastError();
//    }
//
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS exception ("
//        "exception_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "
//        "name TEXT NOT NULL UNIQUE, "
//        "paid_hours INTEGER NOT NULL, "
//        "paid_coefficient REAL NOT NULL, "
//        "work_coefficient REAL NOT NULL, "
//        "is_overtime INTEGER NOT NULL, "
//        "created_at TEXT, "
//        "updated_at TEXT);")) {
//        qDebug() << "Error creating exception table:" << query.lastError();
//    }
//
//    query.prepare("INSERT OR IGNORE INTO exception (name, paid_hours, paid_coefficient, work_coefficient, is_overtime, created_at, updated_at) VALUES (?, ?, ?, ?, ?, datetime('now'), datetime('now'))");
//    query.addBindValue("Sickly");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    query.addBindValue("Vacation");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//   
//    query.addBindValue("Holiday");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS attendance_event ("
//        "attendance_event_id INTEGER PRIMARY KEY AUTOINCREMENT, "
//        "type TEXT NOT NULL, "
//        "date INTEGER NOT NULL, "
//        "user_id TEXT NOT NULL, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id));")) {
//        qDebug() << "Error creating attendance_event table:" << query.lastError();
//    }
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS user_exception ("
//        "user_exception_id INTEGER PRIMARY KEY AUTOINCREMENT, "
//        "user_id TEXT NOT NULL, "
//        "exception_id INTEGER NOT NULL, "
//        "start_date INTEGER NOT NULL, "
//        "end_date INTEGER NOT NULL, "
//        "is_all_date INTEGER NOT NULL, "
//        "day_of_week NUMERIC NOT NULL, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id), "
//        "FOREIGN KEY(exception_id) REFERENCES exception(exception_id));")) {
//        qDebug() << "Error creating user_exception table:" << query.lastError();
//    }
//
//    if (!query.exec("CREATE TABLE IF NOT EXISTS bulletin_board ("
//        "bulletin_board_id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE, "
//        "title TEXT NOT NULL, "
//        "content TEXT NOT NULL, "
//        "start_date INTEGER, "
//        "end_date INTEGER, "
//        "is_active INTEGER, "
//        "is_high_priority INTEGER, "
//        "user_id TEXT NOT NULL, "
//        "created_at TEXT, "
//        "updated_at TEXT, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id));")) {
//        qDebug() << "Error creating bulletin_board table:" << query.lastError();
//    }
//}

//void DatabaseHelper::createTableForMySQL()
//{
//    QSqlQuery query;
//
//    // Tạo bảng role
//    if (!query.exec("CREATE TABLE IF NOT EXISTS role ("
//        "role_id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE, "
//        "name VARCHAR(50) NOT NULL UNIQUE, "
//        "description TEXT, "
//        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);")) {
//        qDebug() << "Error creating role table:" << query.lastError();
//    }
//
//    // Thêm dữ liệu vào bảng role
//    query.prepare("INSERT IGNORE INTO role (name, description, created_at, updated_at) VALUES (?, ?, NOW(), NOW())");
//    query.addBindValue("Admin");
//    query.addBindValue("admin");
//    if (!query.exec()) {
//        qDebug() << "Error inserting into role table:" << query.lastError();
//    }
//
//    query.prepare("INSERT IGNORE INTO role (name, description, created_at, updated_at) VALUES (?, ?, NOW(), NOW())");
//    query.addBindValue("User");
//    query.addBindValue("user");
//    if (!query.exec()) {
//        qDebug() << "Error inserting into role table:" << query.lastError();
//    }
//
//    // Tạo bảng department
//    if (!query.exec("CREATE TABLE IF NOT EXISTS department ("
//        "department_id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE, "
//        "name VARCHAR(50) NOT NULL UNIQUE, "
//        "description TEXT, "
//        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "is_active INTEGER);")) {
//        qDebug() << "Error creating department table:" << query.lastError();
//    }
//
//    // Thêm dữ liệu vào bảng department
//    query.prepare("INSERT IGNORE INTO department (name, description, created_at, updated_at, is_active) VALUES (?, ?, NOW(), NOW(), ?)");
//    query.addBindValue("Other");
//    query.addBindValue("other");
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    // Tạo bảng user
//    if (!query.exec("CREATE TABLE IF NOT EXISTS user ("
//        "user_id VARCHAR(36) PRIMARY KEY NOT NULL UNIQUE, "
//        "first_name VARCHAR(50), "
//        "last_name VARCHAR(50), "
//        "is_password INTEGER, "
//        "password TEXT, "
//        "department_id INTEGER, "
//        "date_of_birth VARCHAR(50), "
//        "start_working_date INTEGER, "
//        "is_active INTEGER NOT NULL, "
//        "avatar BLOB, "
//        "email VARCHAR(100), "
//        "phone VARCHAR(15), "
//        "cell_phone VARCHAR(15), "
//        "address TEXT, "
//        "eye_right BLOB, "
//        "eye_left BLOB, "
//        "role_id INTEGER NOT NULL, "
//        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
//        "FOREIGN KEY (department_id) REFERENCES department(department_id), "
//        "FOREIGN KEY (role_id) REFERENCES role(role_id));")) {
//        qDebug() << "Error creating user table:" << query.lastError();
//    }
//
//    // Tạo bảng exception
//    if (!query.exec("CREATE TABLE IF NOT EXISTS exception ("
//        "exception_id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE, "
//        "name VARCHAR(50) NOT NULL UNIQUE, "
//        "paid_hours INTEGER NOT NULL, "
//        "paid_coefficient REAL NOT NULL, "
//        "work_coefficient REAL NOT NULL, "
//        "is_overtime INTEGER NOT NULL, "
//        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);")) {
//        qDebug() << "Error creating exception table:" << query.lastError();
//    }
//   
//    // Thêm dữ liệu vào bảng exception
//    query.prepare("INSERT IGNORE INTO exception (name, paid_hours, paid_coefficient, work_coefficient, is_overtime, created_at, updated_at) VALUES (?, ?, ?, ?, ?, NOW(), NOW())");
//    query.addBindValue("Sickly");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    query.addBindValue("Vaction");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    query.addBindValue("Holiday");
//    query.addBindValue(28800);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    query.addBindValue(1);
//    if (!query.exec()) {
//        qDebug() << "Error inserting into department table:" << query.lastError();
//    }
//
//    // Tạo bảng attendance_event
//    if (!query.exec("CREATE TABLE IF NOT EXISTS attendance_event ("
//        "attendance_event_id INTEGER PRIMARY KEY AUTO_INCREMENT, "
//        "type VARCHAR(50) NOT NULL, "
//        "date INTEGER NOT NULL, "
//        "user_id VARCHAR(36) NOT NULL, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id));")) {
//        qDebug() << "Error creating attendance_event table:" << query.lastError();
//    }
//
//    // Tạo bảng user_exception
//    if (!query.exec("CREATE TABLE IF NOT EXISTS user_exception ("
//        "user_exception_id INTEGER PRIMARY KEY AUTO_INCREMENT, "
//        "user_id VARCHAR(36) NOT NULL, "
//        "exception_id INTEGER NOT NULL, "
//        "start_date INTEGER NOT NULL, "
//        "end_date INTEGER NOT NULL, "
//        "is_all_date INTEGER NOT NULL, "
//        "day_of_week INTEGER NOT NULL, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id), "
//        "FOREIGN KEY(exception_id) REFERENCES exception(exception_id));")) {
//        qDebug() << "Error creating user_exception table:" << query.lastError();
//    }
//
//    // Tạo bảng bulletin_board
//    if (!query.exec("CREATE TABLE IF NOT EXISTS bulletin_board ("
//        "bulletin_board_id INTEGER PRIMARY KEY AUTO_INCREMENT NOT NULL UNIQUE, "
//        "title TEXT NOT NULL, "
//        "content TEXT NOT NULL, "
//        "start_date INTEGER, "
//        "end_date INTEGER, "
//        "is_active INTEGER, "
//        "is_high_priority INTEGER, "
//        "user_id VARCHAR(36) NOT NULL, "
//        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP, "
//        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, "
//        "FOREIGN KEY(user_id) REFERENCES user(user_id));")) {
//        qDebug() << "Error creating bulletin_board table:" << query.lastError();
//    }
//}
