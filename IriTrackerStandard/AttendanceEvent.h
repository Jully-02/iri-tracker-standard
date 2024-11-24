#ifndef ATTENDANCEEVENT_H
#define ATTENDANCEEVENT_H

#include <QString>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>
#include <QList>
#include "User.h"

class AttendanceEvent {
public:
    // Constructors
    AttendanceEvent();
    AttendanceEvent(int attendanceEventId, const QString& type, qint64 date, const QString& userId);

    // Getters and Setters
    int getAttendanceEventId() const;
    void setAttendanceEventId(int attendanceEventId);

    QString getType() const;
    void setType(const QString& type);

    qint64 getDate() const; 
    void setDate(qint64 date); 

    QString getUserId() const;
    void setUserId(const QString& userId);

private:
    int m_attendanceEventId;
    QString m_type;
    qint64 m_date;
    QString m_userId;
};

#endif // ATTENDANCEEVENT_H
