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

    bool backupSQLite(const QString& dbPath, const QString& backupPath);
    bool backupMySQL(const QString& host, const QString& user, const QString& password, const QString& database, const QString& backupPath);

    bool backupDatabase(const QString& dbType, const QString& dbPath, const QString& backupPath,
        const QString& host = "", const QString& user = "", const QString& password = "", const QString& database = "");



private:
    static void createTable();

    static IDatabase* dbInstance;
    static QSqlDatabase db;
    static QString databaseName;
};

#endif 