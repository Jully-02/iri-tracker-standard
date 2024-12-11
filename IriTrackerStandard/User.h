#ifndef USER_H
#define USER_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include "FunctionPublic.h"
#include <QList>
#include <QVariantList>
#include <QByteArray>

class User {
public:
    // Constructors
    User();
    User(const QString& userId, const QString& firstName, const QString& lastName, bool isPassword, const QString& password,
        const QString& dateOfBirth, qint64 startWorkingDate, bool isActive, const QByteArray& avatar,
        const QString& email, const QString& phone, const QString& cellPhone, const QString& address, const QByteArray& eyeRight, const QByteArray& eyeLeft,
        const QDateTime& createdAt, const QDateTime& updatedAt, int departmentId, int roleId);

    // Getters and Setters
    QString getUserId() const;
    void setUserId(const QString& userId);

    QString getFirstName() const;
    void setFirstName(const QString& firstName);

    QString getLastName() const;
    void setLastName(const QString& lastName);

    bool getIsPassword() const;
    void setIsPassword(bool isPassword);

    QString getPassword() const;
    void setPassword(const QString& password);

    QString getDateOfBirth() const;
    void setDateOfBirth(const QString& dateOfBirth);

    qint64 getStartWorkingDate() const;
    void setStartWorkingDate(qint64 startWorkingDate);

    bool getIsActive() const;
    void setIsActive(bool isActive);

    QByteArray getAvatar() const;
    void setAvatar(const QByteArray& avatar);

    QString getEmail() const;
    void setEmail(const QString& email);

    QString getPhone() const;
    void setPhone(const QString& phone);

    QString getCellPhone() const;
    void setCellPhone(const QString& cellPhone);

    QString getAddress() const;
    void setAddress(const QString& address);

    QByteArray getEyeRight() const;
    void setEyeRight(const QByteArray& eyeRight);

    QByteArray getEyeLeft() const;
    void setEyeLeft(const QByteArray& eyeLeft);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& createdAt);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& updatedAt);

    int getDepartmentId() const;
    void setDepartmentId(int departmentId);

    int getRoleId() const;
    void setRoleId(int roleId);

private:
    QString m_userId;
    QString m_firstName;
    QString m_lastName;
    bool m_isPassword;
    QString m_password;
    QString m_dateOfBirth;
    qint64 m_startWorkingDate;
    bool m_isActive;
    QByteArray m_avatar;
    QString m_email;
    QString m_phone;
    QString m_cellPhone;
    QString m_address;
    QByteArray m_eyeRight;
    QByteArray m_eyeLeft;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    int m_departmentId;
    int m_roleId;
};

#endif // USER_H
