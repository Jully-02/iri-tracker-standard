#ifndef ATTENDANCEVENTREPOSITORY_H
#define ATTENDANCEVENTREPOSITORY_H

#include "AttendanceEvent.h"

class AttendanceEventRepository {
public:
    virtual ~AttendanceEventRepository() {}

    virtual bool insert(const AttendanceEvent& attendanceEvent) = 0;
    virtual bool update(const AttendanceEvent& attendanceEvent) = 0;
    virtual bool deleteItem(int id) = 0;
    virtual AttendanceEvent selectById(int id) = 0;

    virtual QList<AttendanceEvent> selectAll(bool condition = false) = 0;

    virtual QList<AttendanceEvent> selectAttendanceByUserId(const QString& userId) = 0;
    virtual bool checkForOverlappingEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) = 0;
    virtual bool checkForDuplicateEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) = 0;
    virtual bool checkForOrphanInEventsHasTime(const QString& userId, qint64 checkoutTime) = 0;
    virtual bool checkForOrphanInEvents(const QString& userId) = 0;
    virtual bool isEventBeforeStartWorkingDate(const QString& userId, qint64 checkinTime = -1, qint64 checkoutTime = -1) = 0;
    virtual bool isCheckinBeforeExistingEvents(const QString& userId, qint64 checkinTime) = 0;
    virtual std::pair<AttendanceEvent, AttendanceEvent> getInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) = 0;
    virtual bool deleteInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) = 0;
    virtual QList<AttendanceEvent> selectAttendanceEventByUserId(const QString& userId, const QString& timeFilter = "All event") = 0;
    virtual double calculateTotalHours(const QString& userId, const QString& timeFilter) = 0;
    virtual QString getLastAttendanceType(const QString& userId) = 0;
};

#endif // ATTENDANCEVENTREPOSITORY_H