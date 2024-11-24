#ifndef ATTENDANCEEVENTREPOSITORYSQLITE_H
#define ATTENDANCEEVENTREPOSITORYSQLITE_H

#include "AttendanceEventRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class AttendanceEventRepositorySQLite : public AttendanceEventRepository {
public:
    AttendanceEventRepositorySQLite() = default;
    ~AttendanceEventRepositorySQLite() = default;

    bool insert(const AttendanceEvent& attendanceEvent) override;
    bool update(const AttendanceEvent& attendanceEvent) override;
    bool deleteItem(int id) override;
    AttendanceEvent selectById(int id) override;

    QList<AttendanceEvent> selectAll(bool condition = false) override;

    QList<AttendanceEvent> selectAttendanceByUserId(const QString& userId) override;
    bool checkForOverlappingEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) override;
    bool checkForDuplicateEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) override;
    bool checkForOrphanInEventsHasTime(const QString& userId, qint64 checkoutTime) override;
    bool checkForOrphanInEvents(const QString& userId) override;
    bool isEventBeforeStartWorkingDate(const QString& userId, qint64 checkinTime = -1, qint64 checkoutTime = -1) override;
    bool isCheckinBeforeExistingEvents(const QString& userId, qint64 checkinTime) override;
    std::pair<AttendanceEvent, AttendanceEvent> getInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) override;
    bool deleteInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) override;
    QList<AttendanceEvent> selectAttendanceEventByUserId(const QString& userId, const QString& timeFilter = "All event") override;
    double calculateTotalHours(const QString& userId, const QString& timeFilter) override;
    QString getLastAttendanceType(const QString& userId) override;
};

#endif // ATTENDANCEEVENTREPOSITORYSQLITE_H
