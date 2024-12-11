#include "User.h"
#include <QDebug>
#include <QDateTime>

// Constructors
User::User()
    :m_isPassword(false), m_isActive(true), m_departmentId(0), m_roleId(0) {}

User::User(const QString& userId, const QString& firstName, const QString& lastName, bool isPassword, const QString& password,
    const QString& dateOfBirth, qint64 startWorkingDate, bool isActive, const QByteArray& avatar,
    const QString& email, const QString& phone, const QString& cellPhone, const QString& address, const QByteArray& eyeRight, const QByteArray& eyeLeft,
    const QDateTime& createdAt, const QDateTime& updatedAt, int departmentId, int roleId)
    : m_userId(userId), m_firstName(firstName), m_lastName(lastName), m_isPassword(isPassword), m_password(password),
    m_dateOfBirth(dateOfBirth), m_startWorkingDate(startWorkingDate), m_isActive(isActive), m_avatar(avatar),
    m_email(email), m_phone(phone), m_cellPhone(cellPhone), m_address(address), m_eyeRight(eyeRight), m_eyeLeft(eyeLeft),
    m_createdAt(createdAt), m_updatedAt(updatedAt), m_departmentId(departmentId), m_roleId(roleId) {}

// Getters and Setters
QString User::getUserId() const { return m_userId; }
void User::setUserId(const QString& userId) { m_userId = userId; }

QString User::getFirstName() const { return m_firstName; }
void User::setFirstName(const QString& firstName) { m_firstName = firstName; }

QString User::getLastName() const { return m_lastName; }
void User::setLastName(const QString& lastName) { m_lastName = lastName; }

bool User::getIsPassword() const { return m_isPassword; }
void User::setIsPassword(bool isPassword) { m_isPassword = isPassword; }

QString User::getPassword() const { return m_password; }
void User::setPassword(const QString& password) { m_password = password; }

QString User::getDateOfBirth() const { return m_dateOfBirth; }
void User::setDateOfBirth(const QString& dateOfBirth) { m_dateOfBirth = dateOfBirth; }

qint64 User::getStartWorkingDate() const { return m_startWorkingDate; }
void User::setStartWorkingDate(qint64 startWorkingDate) { m_startWorkingDate = startWorkingDate; }

bool User::getIsActive() const { return m_isActive; }
void User::setIsActive(bool isActive) { m_isActive = isActive; }

QByteArray User::getAvatar() const { return m_avatar; }
void User::setAvatar(const QByteArray& avatar) { m_avatar = avatar;}

QString User::getEmail() const { return m_email; }
void User::setEmail(const QString& email) { m_email = email; }

QString User::getPhone() const { return m_phone; }
void User::setPhone(const QString& phone) { m_phone = phone; }

QString User::getCellPhone() const { return m_cellPhone; }
void User::setCellPhone(const QString& cellPhone) { m_cellPhone = cellPhone; }

QString User::getAddress() const { return m_address; }
void User::setAddress(const QString& address) { m_address = address; }

QByteArray User::getEyeRight() const { return m_eyeRight; }
void User::setEyeRight(const QByteArray& eyeRight) { m_eyeRight = eyeRight; };

QByteArray User::getEyeLeft() const { return m_eyeLeft; }
void User::setEyeLeft(const QByteArray& eyeLeft) { m_eyeLeft = eyeLeft; }

QDateTime User::getCreatedAt() const { return m_createdAt; }
void User::setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

QDateTime User::getUpdatedAt() const { return m_updatedAt; }
void User::setUpdatedAt(const QDateTime& updatedAt) { m_updatedAt = updatedAt; }

int User::getDepartmentId() const { return m_departmentId; }
void User::setDepartmentId(int departmentId) { m_departmentId = departmentId; }

int User::getRoleId() const { return m_roleId; }
void User::setRoleId(int roleId) { m_roleId = roleId; }
