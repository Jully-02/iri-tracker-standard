#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_IriTrackerStandard.h"
#include <QList>
#include "AttendanceEvent.h"
#include <QPushButton>

class IriTrackerStandard : public QMainWindow
{
    Q_OBJECT

public:
    IriTrackerStandard(QWidget *parent = nullptr);
    ~IriTrackerStandard();

private:
    void btnLoginClicked();
    void btnLogoutClicked();
    void onLoginSuccessful();
    void onLogoutSuccessful();

    void btnDepartmentClicked();
    void btnEmployeeClicked();
    void btnEventClicked();

    void btnAddEventClicked();
    void btnEditEventClicked();
    void btnCheckoutEventClicked();
    void btnDeleteEventClicked();

    // Exceptions
    void btnExceptionClicked();
    void btnAddExceptionClicked();
    void btnEditExceptionClicked();
    void btnDeleteExceptionClicked();

    void initializeBackButton();
    void backButtonClicked();
    void btnDepartmentAddClicked();
    void btnDepartmentEditClicked();
    void btnDepartmentDeleteClicked();
    void btnEmployeeAddClicked();
    void btnEmployeeEditClicked();
    void btnEmployeeDeleteClicked();
    void loadDepartmentsAndUsers();
    void setupRealTimeClock();
    void updateDateTime();


    // Function of Tools
    //void btnBackupClicked();
    void btnChangeDBClicked();
    //void btnChangePasswordClicked();
    //void btnSettingClicked();
    //void btnRestoreClicked();

    void onLoadEvent();

    void handleViewFormCombobox();
    void btnToolClicked();
private slots:
    void onUserItemClicked(const QModelIndex& index);

signals:
    void departmentFormOpened(const QString& action, int id = -1, const QString& name = QString(), const QString& description = QString());
    void employeeFormOpened(const QString& action, QString id = "");
    void attendanceEventFormOpened(const QString& action, const QString& userId, int id = -1);
    void attendanceEventDeleteFormOpened(const QString& userId, int id = -1);
private:
    Ui::IriTrackerStandardClass ui;

    QPushButton* backButton;
    QString selectedEventUserId;
    QString timeFilter = "All event";

    QLabel* noDeviceLabel;
    QLabel* nameLabel;
    QLineEdit* nameDisplayLineEdit;
    QLabel* passwordLabel;
    QLineEdit* passwordLineEdit;
    QPushButton* checkButton;
    QPushButton* backButtonS;
    QGridLayout* mainLayout;
};
