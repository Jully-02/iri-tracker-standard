#include "IriTrackerStandard.h"
#include "SetAdministratorPassword.h"
#include "Login.h"
#include "DatabaseHelper.h"
#include <QtWidgets/QApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include "IriTracker.h"
#include <QDebug>
#include <QThread>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    //QThread* thread = QThread::create([]() {
    //    IriTracker::get_divice();
    //    });

    //// Bắt đầu thread
    //thread->start();

    qDebug() << QSqlDatabase::drivers();
    DatabaseHelper dbHelper;
    DatabaseHelper::setDatabaseName("../db/iri-tracker-standard.db");
    dbHelper.initializeAndOpenDatabase();

    IriTrackerStandard* mainWin = nullptr;
    SetAdministratorPassword* setAdmin = nullptr;

    if (DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkIfAdminExist().getUserId() == "admin") {
        mainWin = new IriTrackerStandard();
        mainWin->show();
    }
    else {
        setAdmin = new SetAdministratorPassword();
        setAdmin->show();

        QObject::connect(setAdmin, &SetAdministratorPassword::adminCreated, [&]() {
            mainWin = new IriTrackerStandard();
            mainWin->show();

            setAdmin->close();  
            delete setAdmin;    
            });
    }

    return a.exec();
}
