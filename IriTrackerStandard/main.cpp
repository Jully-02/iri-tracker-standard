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
#include "EmployeeForm.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QThread* threadScanDevice = new QThread();
    IriTracker* iriTracker = new IriTracker();
    EmployeeForm* employeeForm = new EmployeeForm();
    iriTracker->moveToThread(threadScanDevice);

    QObject::connect(iriTracker, &IriTracker::onOpenDevice,
        employeeForm, &EmployeeForm::changeImageDevice);

    QObject::connect(threadScanDevice, &QThread::started, iriTracker, &IriTracker::get_device);

    // Bắt đầu thread
    threadScanDevice->start();

    qDebug() << QSqlDatabase::drivers();
    DatabaseHelper dbHelper;
    DatabaseHelper::setDatabaseName("../db/iri-tracker-standard.db");
    dbHelper.initializeAndOpenDatabase();

    IriTrackerStandard* mainWin = nullptr;
    SetAdministratorPassword* setAdmin = nullptr;

    if (DatabaseHelper::getDatabaseInstance()->getUserRepository()->checkIfAdminExist().getUserId() == "admin") {
        mainWin = new IriTrackerStandard();
        QObject::connect(iriTracker, &IriTracker::onOpenDevice,
            mainWin, &IriTrackerStandard::changeImageDevice);


        mainWin->show();
    }
    else {
        setAdmin = new SetAdministratorPassword();
        setAdmin->show();

        QObject::connect(setAdmin, &SetAdministratorPassword::adminCreated, [&]() {
            mainWin = new IriTrackerStandard();
            QObject::connect(iriTracker, &IriTracker::onOpenDevice,
                mainWin, &IriTrackerStandard::changeImageDevice);


            mainWin->show();

            setAdmin->close();  
            delete setAdmin;    
            });
    }

    return a.exec();
}
