#include "DatabaseHelper.h"
#include "ChangeDatabase.h"
#include <QProcess>

IDatabase* DatabaseHelper::dbInstance = nullptr;
QSqlDatabase DatabaseHelper::db;
QString DatabaseHelper::databaseName = "";
QString DatabaseHelper::m_hostName = "";
QString DatabaseHelper::m_password = "";
QString DatabaseHelper::m_userName = "";
DatabaseType DatabaseHelper::currentDatabaseType = DatabaseType::Unknown;

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
    return currentDatabaseType;
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
    currentDatabaseType = DatabaseType::SQLite;
    qDebug() << "SQLite database initialized and opened successfully.";
    return true;
}

DatabaseHelper::~DatabaseHelper() {
    closeDatabase();
}

// Backup Database
bool DatabaseHelper::backupSQLite(const QString& dbPath, const QString& backupPath) {
    // Sao lưu
    if (QFile::exists(backupPath)) {
        QFile::remove(backupPath);
    }
    if (!QFile::copy(dbPath, backupPath)) {
        qDebug() << "SQLite database backup failed!";
        return false;
    }
    qDebug() << "SQLite database backup successful!";
    return true;
}


bool DatabaseHelper::backupMySQL(const QString& backupPath) {
    // Check connection information
    qDebug() << "Backup Path: " << backupPath;
    if (m_hostName.isEmpty() || m_userName.isEmpty() || databaseName.isEmpty()) {
        qDebug() << "Missing MySQL connection details. Cannot perform backup.";
        return false;
    }

    // Build the mysqldump command
    QStringList arguments;
    arguments << "--column-statistics=0" // Turn off column statistics
        << "-h" << m_hostName
        << "-u" << m_userName
        << "--password=" + m_password 
        << databaseName;

    // Execute the mysqldump command using QProcess
    QProcess process;
    process.start("mysqldump", arguments);

    // Wait for the command to finish executing
    process.waitForFinished();

    // Check if the command executed successfully
    if (process.exitCode() == 0) {
        // Get command output (backup data)
        QByteArray output = process.readAllStandardOutput();

        // Open file to write backup data
        QFile file(backupPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(output);  // Write backup data to file
            file.close();
            qDebug() << "MySQL backup successful. File saved at:" << backupPath;
            return true;
        }
        else {
            qDebug() << "Failed to open backup file for writing.";
            return false;
        }
    }
    else {
        // If command fails, read error from QProcess
        qDebug() << "MySQL backup failed. Error:" << process.readAllStandardError();
        return false;
    }
}



bool DatabaseHelper::backupDatabase(const QString& dbPath, const QString& backupPath) {
    if (db.driverName() == "QSQLITE") {
        return backupSQLite(dbPath, backupPath);
    }
    else if (db.driverName() == "QMYSQL") {
        return backupMySQL(backupPath);
    }
    else {
        qDebug() << "Unsupported database type!";
        return false;
    }
}

bool DatabaseHelper::changeDatabase(DatabaseType type, const QString& dbName,
    const QString& hostName, const QString& userName,
    const QString& password) {
    // Backup current database connection
    QSqlDatabase backupDb = db;
    IDatabase* backupDbInstance = dbInstance;

    User user = DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkIfAdminExist();
    user.setDepartmentId(1);

    closeDatabase();

    // Configure new database connection
    db = QSqlDatabase::addDatabase((type == DatabaseType::MySQL) ? "QMYSQL" : "QSQLITE");
    if (type == DatabaseType::SQLite) {
        db.setDatabaseName(dbName);
        databaseName = dbName;
    }
    else {
        db.setHostName(hostName);
        db.setUserName(userName);
        db.setPassword(password);
    }

    if (!db.open()) {
        qDebug() << "Failed to switch database:" << db.lastError().text();
        revertDatabaseConnection(backupDb, backupDbInstance);
        return false;
    }

    if (db.driverName() == "QSQLITE") {
        if (!setupSQLiteDatabase(db, user)) {
            revertDatabaseConnection(backupDb, backupDbInstance);
            return false;
        }
        else {
            currentDatabaseType = DatabaseType::SQLite;
        }
        m_hostName = "";
        m_userName = "";
        m_password = "";
    }
    else if (db.driverName() == "QMYSQL") {
        if (!setupMySQLDatabase(db, dbName, user)) {
            revertDatabaseConnection(backupDb, backupDbInstance);
            return false;
        }
        else {
            databaseName = dbName;
            m_hostName = hostName;
            m_userName = userName;
            m_password = password;
            currentDatabaseType = DatabaseType::MySQL;
        }
    }

    qDebug() << "Database switched successfully!";
    return true;
}

void DatabaseHelper::revertDatabaseConnection(QSqlDatabase& backupDb, IDatabase* backupDbInstance) {
    db = backupDb;
    dbInstance = backupDbInstance;

    if (!db.isOpen() && !db.open()) {
        qDebug() << "Critical error: Failed to reconnect to the backup database!";
        reconnectDefaultDatabase();
    }
    else {
        qDebug() << "Reverted to the backup database connection.";
    }
}

void DatabaseHelper::reconnectDefaultDatabase() {
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
    }
}

void DatabaseHelper::reconnectMySQL() {
    // Check if required information has not been set
    if (m_hostName.isEmpty() || m_userName.isEmpty() || databaseName.isEmpty()) {
        qDebug() << m_hostName << " " << m_userName << " " << m_password << " " << databaseName;
        qDebug() << "Missing connection details for MySQL. Please check your configuration.";
        return;
    }

    // Delete old connection (if any)
    if (db.isOpen()) {
        db.close();
        QString connectionName = db.connectionName();
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Establish a new MySQL connection
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(m_hostName);
    db.setDatabaseName(databaseName);
    db.setUserName(m_userName);
    db.setPassword(m_password);

    // Reconnect the database
    if (db.open()) {
        qDebug() << "Reconnected to the MySQL database successfully!";
        dbInstance = new MySQLDatabase();
    }
    else {
        qDebug() << "Failed to reconnect to the MySQL database. Error:" << db.lastError().text();
    }
}


bool DatabaseHelper::setupSQLiteDatabase(QSqlDatabase& db, const User& user) {
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
    dbInstance = new SQLiteDatabase();
    return true;
}

bool DatabaseHelper::setupMySQLDatabase(QSqlDatabase& db, const QString& dbName, const User& user) {
    QSqlQuery query(db);

    query.prepare("SELECT SCHEMA_NAME FROM information_schema.SCHEMATA WHERE SCHEMA_NAME = :dbName");
    query.bindValue(":dbName", dbName);
    bool isNew = false;
    if (!query.exec() || !query.next()) {
        qDebug() << "Database" << dbName << "does not exist. Creating it...";
        if (!createMySQLDatabase(db, dbName)) {
            return false;
        }
        else {
            isNew = true;
        }
    }

    qDebug() << "Database" << dbName << "exists or was created successfully.";
    db.close();
    db.setDatabaseName(dbName);
    if (!db.open()) {
        qDebug() << "Failed to open MySQL database!";
        return false;
    }
    if (isNew) {
        createTable();
        DatabaseHelper::getDatabaseInstance()->getUserRepository()->insert(user);
    }
    dbInstance = new MySQLDatabase();
    return true;
}

bool DatabaseHelper::createMySQLDatabase(QSqlDatabase& db, const QString& dbName) {
    QSqlQuery createDbQuery(db);
    if (!createDbQuery.exec(QString("CREATE DATABASE `%1`").arg(dbName))) {
        qDebug() << "Failed to create MySQL database:" << createDbQuery.lastError().text();
        return false;
    }
        qDebug() << "MySQL database created successfully.";
        return true;
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


bool DatabaseHelper::restoreSQLiteFromFile(const QString& dbPath, const QString& backupFilePath) {
    // Check if the backup file exists
    if (!QFile::exists(backupFilePath)) {
        qDebug() << "Backup file does not exist!";
        return false;
    }

    // Close the connection if the database is open
    if (db.isOpen()) {
        db.close();
    }
    closeDatabase();
    // Check if the main database file already exists, delete it before copying
    if (QFile::exists(dbPath)) {
        if (!QFile::remove(dbPath)) {
            qDebug() << "Failed to remove the old database file!";
            return false;
        }
    }

    // Copy backup file to database location
    if (QFile::copy(backupFilePath, dbPath)) {
        qDebug() << "Database restored successfully by replacing the old database file!";
        return true;
    }
    else {
        qDebug() << "Failed to restore database by replacing the old database file!";
        return false;
    }
}

bool DatabaseHelper::restoreMySQLFromFile(const QString& backupPath) {
    // Check connection information
    qDebug() << "Restore Path: " << backupPath;
    if (m_hostName.isEmpty() || m_userName.isEmpty() || databaseName.isEmpty()) {
        qDebug() << "Missing MySQL connection details. Cannot perform restore.";
        return false;
    }

    // Check if the backup file exists
    QFile file(backupPath);
    if (!file.exists()) {
        qDebug() << "Backup file does not exist.";
        return false;
    }

    // Build mysql command to restore
    QStringList arguments;
    arguments << "-h" << m_hostName
        << "-u" << m_userName
        << "--password=" + m_password 
        << databaseName;  

    // Open the backup file to read data
    QFile backupFile(backupPath);
    if (!backupFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open backup file for reading.";
        return false;
    }

    // Read backup file content
    QByteArray backupData = backupFile.readAll();
    backupFile.close();

    // Execute mysql command to restore database from backup file
    QProcess process;
    process.start("mysql", arguments);

    // Send backup data to input of mysql command
    process.write(backupData);
    process.closeWriteChannel(); // Close the recording channel

   // Wait for the command to finish executing
    process.waitForFinished();

    // Check if the command executed successfully
    if (process.exitCode() == 0) {
        qDebug() << "MySQL restore successful.";
        return true;
    }
    else {
        // If there is an error while executing the command, read the error from QProcess
        qDebug() << "MySQL restore failed. Error:" << process.readAllStandardError();
        return false;
    }
}
