#include "UserException.h"
#include <QDebug>

// Constructors
UserException::UserException()
    : m_isAllDate(false), m_dayOfWeek(0) {}

UserException::UserException(const QString& userExceptionId, qint64 startDate, qint64 endDate,
    bool isAllDate, int dayOfWeek, const QString& userId, const QString& exceptionId)
    : m_userExceptionId(userExceptionId), m_startDate(startDate), m_endDate(endDate),
    m_isAllDate(isAllDate), m_dayOfWeek(dayOfWeek), m_userId(userId), m_exceptionId(exceptionId) {}

// Getters and Setters
QString UserException::getUserExceptionId() const { return m_userExceptionId; }
void UserException::setUserExceptionId(const QString& userExceptionId) { m_userExceptionId = userExceptionId; }

qint64 UserException::getStartDate() const { return m_startDate; }
void UserException::setStartDate(qint64 startDate) { m_startDate = startDate; }

qint64 UserException::getEndDate() const { return m_endDate; }
void UserException::setEndDate(qint64 endDate) { m_endDate = endDate; }

bool UserException::getIsAllDate() const { return m_isAllDate; }
void UserException::setIsAllDate(bool isAllDate) { m_isAllDate = isAllDate; }

int UserException::getDayOfWeek() const { return m_dayOfWeek; }
void UserException::setDayOfWeek(int dayOfWeek) { m_dayOfWeek = dayOfWeek; }

QString UserException::getUserId() const { return m_userId; }
void UserException::setUserId(const QString& userId) { m_userId = userId; }

QString UserException::getExceptionId() const { return m_exceptionId; }
void UserException::setExceptionId(const QString& exceptionId) { m_exceptionId = exceptionId; }
