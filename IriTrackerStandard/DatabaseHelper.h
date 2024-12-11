#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QString>
#include <QFile>
#include "IDatabase.h"
#include "SQLiteDatabase.h"
#include "MySQLDatabase.h"

enum class DatabaseType {
    SQLite,
    MySQL,
    Unknown
};

class DatabaseHelper {
public:
    DatabaseHelper();
    ~DatabaseHelper();
    static void closeDatabase();
    static DatabaseType getCurrentDatabaseType();
    static IDatabase* getDatabaseInstance();
    static QSqlDatabase getDatabase();
    bool initializeAndOpenDatabase();

    static void setDatabaseName(const QString& dbName); 
    static QString getDatabaseName();      

    static bool changeDatabase(DatabaseType type,
        const QString& dbName,
        const QString& hostName = "",
        const QString& userName = "",
        const QString& password = "");

    static void revertDatabaseConnection(QSqlDatabase& backupDb, IDatabase* backupDbInstance);

    // Reconnect Database
    static void reconnectDefaultDatabase();
    static void reconnectMySQL();

    static bool setupSQLiteDatabase(QSqlDatabase& db, const User& user);

    static bool createMySQLDatabase(QSqlDatabase& db, const QString& dbName);
    static bool setupMySQLDatabase(QSqlDatabase& db, const QString& dbName, const User& user);

    static bool restoreSQLiteFromFile(const QString& dbPath, const QString& backupFilePath);
    static bool restoreMySQLFromFile(const QString& backupPath);

    bool backupSQLite(const QString& dbPath, const QString& backupPath);
    bool backupMySQL(const QString& backupPath);

    bool backupDatabase(const QString& dbPath, const QString& backupPath);



private:
    static void createTable();

    static IDatabase* dbInstance;
    static QSqlDatabase db;

    static QString databaseName;
    static QString m_hostName;
    static QString m_userName;
    static QString m_password;
    static DatabaseType currentDatabaseType;
};

#endif 