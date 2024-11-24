#include "AttendanceEvent.h"
#include <QDebug>
#include <QDateTime>

// Constructors
AttendanceEvent::AttendanceEvent() : m_attendanceEventId(0) {}

AttendanceEvent::AttendanceEvent(int attendanceEventId, const QString& type, qint64 date, const QString& userId)
    : m_attendanceEventId(attendanceEventId), m_type(type), m_date(date), m_userId(userId) {}

// Getters and Setters
int AttendanceEvent::getAttendanceEventId() const {
    return m_attendanceEventId;
}

void AttendanceEvent::setAttendanceEventId(int attendanceEventId) {
    m_attendanceEventId = attendanceEventId;
}

QString AttendanceEvent::getType() const {
    return m_type;
}

void AttendanceEvent::setType(const QString& type) {
    m_type = type;
}

qint64 AttendanceEvent::getDate() const {
    return m_date;
}

void AttendanceEvent::setDate(qint64 date) {
    m_date = date;
}

QString AttendanceEvent::getUserId() const {
    return m_userId;
}

void AttendanceEvent::setUserId(const QString& userId) {
    m_userId = userId;
}





