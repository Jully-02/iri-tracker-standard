#include "AttendanceEventRepositorySQLite.h"
#include "UserRepositorySQLite.h"

    bool AttendanceEventRepositorySQLite::insert(const AttendanceEvent& attendanceEvent) {
        QSqlQuery query;
        query.prepare("INSERT INTO attendance_event (type, date, user_id) "
            "VALUES (:type, :date, :user_id)");

        query.bindValue(":type", attendanceEvent.getType());
        query.bindValue(":date", attendanceEvent.getDate());
        query.bindValue(":user_id", attendanceEvent.getUserId());

        if (!query.exec()) {
            qDebug() << "Error inserting attendance event:" << query.lastError().text();
            return false;
        }

        return true;
    }
    bool AttendanceEventRepositorySQLite::update(const AttendanceEvent& attendanceEvent) {
        QSqlQuery query;
        query.prepare("UPDATE attendance_event "
            "SET type = :type, date = :date "
            "WHERE attendance_event_id = :attendanceEventId");

        query.bindValue(":type", attendanceEvent.getType());
        query.bindValue(":date", attendanceEvent.getDate());
        query.bindValue(":attendanceEventId", attendanceEvent.getAttendanceEventId());

        if (!query.exec()) {
            qDebug() << "Error updating attendance event:" << query.lastError().text();
            return false;
        }

        return true;
    }
    bool AttendanceEventRepositorySQLite::deleteItem(int id) {
        return false;
    }
    AttendanceEvent AttendanceEventRepositorySQLite::selectById(int id) {
        QSqlQuery query;
        AttendanceEvent event;

        query.prepare("SELECT user_id, type, date FROM attendance_event WHERE attendance_event_id = :eventId");
        query.bindValue(":eventId", id);

        if (!query.exec()) {
            qDebug() << "Error fetching attendance event by ID:" << query.lastError().text();
            return event;
        }

        if (query.next()) {
            event.setAttendanceEventId(id);
            event.setUserId(query.value("user_id").toString());
            event.setType(query.value("type").toString());
            event.setDate(query.value("date").toLongLong());
        }
        else {
            qDebug() << "No attendance event found with ID:" << id;
        }

        return event;
    }

    QList<AttendanceEvent> AttendanceEventRepositorySQLite::selectAll(bool condition){
        QList<AttendanceEvent> attendanceList;
        QSqlQuery query;

        query.prepare("SELECT `attendance_event_id`, `type`, `date`, `user_id` "
            "FROM `attendance_event`");

        if (!query.exec()) {
            qDebug() << "Error selecting attendance events:" << query.lastError().text();
            return attendanceList;
        }

        while (query.next()) {
            int attendanceEventId = query.value("attendance_event_id").toInt();
            QString type = query.value("type").toString();
            qint64 date = query.value("date").toLongLong();
            QString retrievedUserId = query.value("user_id").toString();

            AttendanceEvent event(attendanceEventId, type, date, retrievedUserId);
            qDebug() << event.getAttendanceEventId();
            attendanceList.append(event);
        }

        return attendanceList;
    }

    QList<AttendanceEvent> AttendanceEventRepositorySQLite::selectAttendanceByUserId(const QString& userId) {
        QList<AttendanceEvent> attendanceList;
        QSqlQuery query;

        query.prepare("SELECT attendance_event_id, type, date, user_id "
            "FROM attendance_event "
            "WHERE user_id = :user_id");
        query.bindValue(":user_id", userId);

        if (!query.exec()) {
            qDebug() << "Error selecting attendance events by user ID:" << query.lastError().text();
            return attendanceList;
        }

        while (query.next()) {
            int attendanceEventId = query.value("attendance_event_id").toInt();
            QString type = query.value("type").toString();
            qint64 date = query.value("date").toLongLong();
            QString retrievedUserId = query.value("user_id").toString();

            AttendanceEvent event(attendanceEventId, type, date, retrievedUserId);
            qDebug() << event.getAttendanceEventId();
            attendanceList.append(event);
        }

        return attendanceList;
    }

    bool AttendanceEventRepositorySQLite::checkForOverlappingEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) {
        QSqlQuery query;
        query.prepare(
            "SELECT COUNT(*) FROM attendance_event AS event_in "
            "JOIN attendance_event AS event_out ON event_in.user_id = event_out.user_id "
            "WHERE event_in.user_id = :userId AND event_in.type = 'In' AND event_out.type = 'Out' "
            "AND event_in.date < :newOutTime AND event_out.date > :newInTime "
            "AND event_in.date < event_out.date"
        );
        query.bindValue(":userId", userId);
        query.bindValue(":newInTime", newInTime);
        query.bindValue(":newOutTime", newOutTime);

        if (!query.exec()) {
            qDebug() << "Error checking for overlapping IN and OUT events:" << query.lastError().text();
            return false;
        }

        return query.next() && query.value(0).toInt() > 0;
    }

    bool AttendanceEventRepositorySQLite::checkForDuplicateEvents(const QString& userId, qint64 newInTime, qint64 newOutTime) {
        QSqlQuery query;

        // Check event "IN"
        query.prepare("SELECT COUNT(*) FROM attendance_event WHERE user_id = :userId AND type = 'In' AND date = :newInTime");
        query.bindValue(":userId", userId);
        query.bindValue(":newInTime", newInTime);

        if (!query.exec()) {
            qDebug() << "Error checking for duplicate 'IN' event:" << query.lastError().text();
            return false;
        }

        bool duplicateInEvent = query.next() && query.value(0).toInt() > 0;

        // Check event "OUT"
        query.prepare("SELECT COUNT(*) FROM attendance_event WHERE user_id = :userId AND type = 'Out' AND date = :newOutTime");
        query.bindValue(":userId", userId);
        query.bindValue(":newOutTime", newOutTime);

        if (!query.exec()) {
            qDebug() << "Error checking for duplicate 'OUT' event:" << query.lastError().text();
            return false;
        }

        bool duplicateOutEvent = query.next() && query.value(0).toInt() > 0;

        // Returns true if both 'IN' and 'OUT' events match
        return duplicateInEvent && duplicateOutEvent;
    }
    bool AttendanceEventRepositorySQLite::checkForOrphanInEventsHasTime(const QString& userId, qint64 checkoutTime) {
        QSqlQuery query;
        // If there is checkoutTime, check the IN and OUT counts
        if (checkForOrphanInEvents(userId)) {
            return true; // No odd IN
        }
        else {
            // If there is an odd IN, get the latest IN time to compare with checkoutTime
            query.prepare("SELECT date FROM attendance_event WHERE user_id = :userId AND type = 'In' ORDER BY date DESC LIMIT 1");
            query.bindValue(":userId", userId);
            if (!query.exec()) {
                qDebug() << "Error fetching the latest IN event:" << query.lastError().text();
                return false;
            }

            if (query.next()) {
                qint64 lastInTime = query.value(0).toLongLong();
                if (checkoutTime < lastInTime) {
                    return true; // Checkout before IN odd
                }
            }
            return false;
        }
    }


    bool AttendanceEventRepositorySQLite::checkForOrphanInEvents(const QString& userId) {
        QSqlQuery query;

        // Query the number of user IN and OUT events
        query.prepare("SELECT COUNT(*) FROM attendance_event WHERE user_id = :userId AND type = 'In'");
        query.bindValue(":userId", userId);
        query.exec();
        query.next();
        int inCount = query.value(0).toInt();

        query.prepare("SELECT COUNT(*) FROM attendance_event WHERE user_id = :userId AND type = 'Out'");
        query.bindValue(":userId", userId);
        query.exec();
        query.next();
        int outCount = query.value(0).toInt();

        // If there is no checkoutTime, just check the IN and OUT counts
        if (inCount == outCount) {
            return true; // No odd IN
        }
        else {
            qDebug() << "End";
            qDebug() << "Error: There is an orphan IN event without a corresponding OUT event.";
            return false; // Has odd IN
        }

    }

    bool AttendanceEventRepositorySQLite::isEventBeforeStartWorkingDate(const QString& userId, qint64 checkinTime, qint64 checkoutTime) {
        
        UserRepositorySQLite userRepoSQLite;
        User user = userRepoSQLite.selectById(userId);

        // Get the start date of work
        qint64 startWorkingDate = user.getStartWorkingDate();

        // If checkinTime or checkoutTime is passed, check them
        if ((checkinTime != -1 && checkinTime < startWorkingDate) ||
            (checkoutTime != -1 && checkoutTime < startWorkingDate)) {
            qDebug() << "Check-in or check-out time is before the start working date for user:" << userId;
            return true; // If there is time before the work start date, return true
        }

        // If there is no value before the start date, return false
        return false;
    }

    bool AttendanceEventRepositorySQLite::isCheckinBeforeExistingEvents(const QString& userId, qint64 checkinTime) {
        QSqlQuery query;
        query.prepare("SELECT date FROM attendance_event WHERE user_id = :userId ORDER BY date ASC");
        query.bindValue(":userId", userId);

        if (!query.exec()) {
            qDebug() << "Error checking for existing events:" << query.lastError().text();
            return false;// If an error occurs in the query, assume there were no previous events
        }

        // Browse all user events for inspection
        while (query.next()) {
            qint64 eventTime = query.value(0).toLongLong();
            if (checkinTime < eventTime) {
                qDebug() << "Check-in time is before an existing event at:" << QDateTime::fromSecsSinceEpoch(eventTime);
                return true; // If checkinTime is before any event, return true to report an error
            }
        }

        return false; // If there is no smaller time, return false
    }

    std::pair<AttendanceEvent, AttendanceEvent> AttendanceEventRepositorySQLite::getInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) {
        QSqlQuery query;
        AttendanceEvent inEvent, outEvent;

        
        query.prepare("SELECT type, date FROM attendance_event WHERE attendance_event_id = :attendanceEventId AND user_id = :userId");
        query.bindValue(":attendanceEventId", attendanceEventId);
        query.bindValue(":userId", userId);

        if (!query.exec()) {
            qDebug() << "Error fetching initial event by ID:" << query.lastError().text();
            return { inEvent, outEvent };
        }

  
        if (query.next()) {
            QString eventType = query.value("type").toString();
            qint64 eventDate = query.value("date").toLongLong();

            // Set the initial `inEvent` or `outEvent` event
            if (eventType == "In") {
                inEvent.setAttendanceEventId(attendanceEventId);
                inEvent.setUserId(userId);
                inEvent.setType("In");
                inEvent.setDate(eventDate);

                // Search for the corresponding `Out` event
                query.prepare("SELECT attendance_event_id, date FROM attendance_event WHERE user_id = :userId AND type = 'Out' AND date > :eventDate ORDER BY date ASC LIMIT 1");
                query.bindValue(":userId", userId);
                query.bindValue(":eventDate", eventDate);

                if (query.exec() && query.next()) {
                    outEvent.setAttendanceEventId(query.value("attendance_event_id").toInt());
                    outEvent.setUserId(userId);
                    outEvent.setType("Out");
                    outEvent.setDate(query.value("date").toLongLong());
                }
            }
            else if (eventType == "Out") {
                outEvent.setAttendanceEventId(attendanceEventId);
                outEvent.setUserId(userId);
                outEvent.setType("Out");
                outEvent.setDate(eventDate);

                // Search for the corresponding `Print` event
                query.prepare("SELECT attendance_event_id, date FROM attendance_event WHERE user_id = :userId AND type = 'In' AND date < :eventDate ORDER BY date DESC LIMIT 1");
                query.bindValue(":userId", userId);
                query.bindValue(":eventDate", eventDate);

                if (query.exec() && query.next()) {
                    inEvent.setAttendanceEventId(query.value("attendance_event_id").toInt());
                    inEvent.setUserId(userId);
                    inEvent.setType("In");
                    inEvent.setDate(query.value("date").toLongLong());
                }
            }
        }
        else {
            qDebug() << "No attendance event found with ID:" << attendanceEventId;
        }

        return { inEvent, outEvent };
    }
    bool AttendanceEventRepositorySQLite::deleteInOutPairByUserIdAndEventId(const QString& userId, int attendanceEventId) {
        qDebug() << "User ID: " << userId;
        qDebug() << "Attendance Event ID: " << attendanceEventId;
        QSqlQuery query;

        // Query to get events with attendanceEventId as input
        query.prepare("SELECT type, date FROM attendance_event WHERE attendance_event_id = :attendanceEventId AND user_id = :userId");
        query.bindValue(":attendanceEventId", attendanceEventId);
        query.bindValue(":userId", userId);

        if (!query.exec() || !query.next()) {
            qDebug() << "Error fetching initial event by ID:" << query.lastError().text();
            return false;
        }

        QString eventType = query.value("type").toString();
        qint64 eventDate = query.value("date").toLongLong();

        qDebug() << "Event Type:" << eventType;
        qDebug() << "Event Date:" << eventDate;

        // Remove input `attendanceEventId` event
        query.prepare("DELETE FROM attendance_event WHERE attendance_event_id = :attendanceEventId AND user_id = :userId");
        query.bindValue(":attendanceEventId", attendanceEventId);
        query.bindValue(":userId", userId);

        if (!query.exec()) {
            qDebug() << "Error deleting event with ID:" << query.lastError().text();
            return false;
        }

        // If it is an `In` event, find the corresponding `Out` event
        if (eventType == "In") {
            query.clear();
            query.prepare("SELECT attendance_event_id FROM attendance_event WHERE user_id = :userId AND type = 'Out' AND date > :eventDate ORDER BY date ASC LIMIT 1");
            query.bindValue(":userId", userId);
            query.bindValue(":eventDate", eventDate);

            if (query.exec() && query.next()) {
                int outEventId = query.value(0).toInt();
                query.clear();
                query.prepare("DELETE FROM attendance_event WHERE attendance_event_id = :outEventId");
                query.bindValue(":outEventId", outEventId);

                if (!query.exec()) {
                    qDebug() << "Error deleting corresponding Out event:" << query.lastError().text();
                    return false;
                }
            }
        }
        // If it is an `Out` event, find the corresponding `In` event
        else if (eventType == "Out") {
            query.clear();
            query.prepare("SELECT attendance_event_id FROM attendance_event WHERE user_id = :userId AND type = 'In' AND date < :eventDate ORDER BY date DESC LIMIT 1");
            query.bindValue(":userId", userId);
            query.bindValue(":eventDate", eventDate);

            if (query.exec() && query.next()) {
                int inEventId = query.value(0).toInt();
                query.clear();
                query.prepare("DELETE FROM attendance_event WHERE attendance_event_id = :inEventId");
                query.bindValue(":inEventId", inEventId);

                if (!query.exec()) {
                    qDebug() << "Error deleting corresponding In event:" << query.lastError().text();
                    return false;
                }
            }
        }

        return true;
    }

    QList<AttendanceEvent> AttendanceEventRepositorySQLite::selectAttendanceEventByUserId(const QString& userId, const QString& timeFilter) {
        QList<AttendanceEvent> events;
        QSqlQuery query;

        QString queryString = "SELECT * FROM attendance_event WHERE user_id = :userId";
        QDateTime startDateTime, endDateTime;
        QDate currentDate = QDate::currentDate();

        if (timeFilter == "This week" || timeFilter == "Tuần này") {
            QDate startOfWeek = currentDate.addDays(-(currentDate.dayOfWeek() - 1)); // Monday this week
            startDateTime = QDateTime(startOfWeek, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfWeek.addDays(6), QTime(23, 59, 59));
            queryString += " AND date >= :startDate AND date <= :endDate";
        }
        else if (timeFilter == "Last week" || timeFilter == "Tuần trước") {
            QDate startOfLastWeek = currentDate.addDays(-(currentDate.dayOfWeek() + 6));// Last Monday
            startDateTime = QDateTime(startOfLastWeek, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfLastWeek.addDays(6), QTime(23, 59, 59));
            queryString += " AND date >= :startDate AND date <= :endDate";
        }
        else if (timeFilter == "This month" || timeFilter == "Tháng này") {
            QDate startOfMonth(currentDate.year(), currentDate.month(), 1);// First day of the month
            startDateTime = QDateTime(startOfMonth, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfMonth.addMonths(1).addDays(-1), QTime(23, 59, 59));
            queryString += " AND date >= :startDate AND date <= :endDate";
        }
        else if (timeFilter == "Last month" || timeFilter == "Tháng trước") {
            QDate startOfLastMonth = currentDate.addMonths(-1);
            startOfLastMonth.setDate(startOfLastMonth.year(), startOfLastMonth.month(), 1); // First day of last month
            startDateTime = QDateTime(startOfLastMonth, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfLastMonth.addMonths(1).addDays(-1), QTime(23, 59, 59));
            queryString += " AND date >= :startDate AND date <= :endDate";
        }
        else if (timeFilter == "This year" || timeFilter == "Năm này") {
            QDate startOfYear(currentDate.year(), 1, 1); // New year's day
            startDateTime = QDateTime(startOfYear, QTime(0, 0, 0));
            endDateTime = QDateTime(QDate(currentDate.year(), 12, 31), QTime(23, 59, 59));
            queryString += " AND date >= :startDate AND date <= :endDate";
        }

        query.prepare(queryString);
        query.bindValue(":userId", userId);

        // Bind start date and end date if not "All events"
        if (timeFilter != "All event") {
            query.bindValue(":startDate", startDateTime.toSecsSinceEpoch());
            query.bindValue(":endDate", endDateTime.toSecsSinceEpoch());
        }

        if (!query.exec()) {
            qDebug() << "Error fetching attendance events:" << query.lastError().text();
            return events; 
        }

        while (query.next()) {
            AttendanceEvent event;
            event.setUserId(query.value("user_id").toString());
            event.setDate(query.value("date").toLongLong());
            event.setType(query.value("type").toString());
            event.setAttendanceEventId(query.value("attendance_event_id").toInt());
            events.append(event);
        }

        return events;
    }

    double AttendanceEventRepositorySQLite::calculateTotalHours(const QString& userId, const QString& timeFilter) {
        double totalHours = 0.0;
        QSqlQuery query;
        QString queryString = "SELECT type, date FROM attendance_event WHERE user_id = :userId ORDER BY date ASC";
        query.prepare(queryString);
        query.bindValue(":userId", userId);

        if (!query.exec()) {
            qDebug() << "Error calculating total hours:" << query.lastError().text();
            return totalHours;
        }

        QDate currentDate = QDate::currentDate();
        QDateTime startDateTime, endDateTime;

        if (timeFilter == "Today") {
            startDateTime = QDateTime(currentDate, QTime(0, 0, 0));
            endDateTime = QDateTime(currentDate, QTime(23, 59, 59));
        }
        else if (timeFilter == "This Week") {
            QDate startOfWeek = currentDate.addDays(-(currentDate.dayOfWeek() - 1)); // Monday this week
            startDateTime = QDateTime(startOfWeek, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfWeek.addDays(6), QTime(23, 59, 59));
        }
        else if (timeFilter == "This Month") {
            QDate startOfMonth(currentDate.year(), currentDate.month(), 1); // First day of the month
            startDateTime = QDateTime(startOfMonth, QTime(0, 0, 0));
            endDateTime = QDateTime(startOfMonth.addMonths(1).addDays(-1), QTime(23, 59, 59));
        }
        else if (timeFilter == "All Events") {
            startDateTime = QDateTime::fromSecsSinceEpoch(0);// Time starts from epoch
            endDateTime = QDateTime::currentDateTime();
        }

        QDateTime lastInTime;
        bool hasInTime = false;

        while (query.next()) {
            QString eventType = query.value("type").toString();
            QDateTime eventDateTime = QDateTime::fromSecsSinceEpoch(query.value("date").toLongLong());

            if (eventType == "In") {
                lastInTime = eventDateTime;
                hasInTime = true;
            }
            else if (eventType == "Out" && hasInTime) {
                if (eventDateTime >= startDateTime && eventDateTime <= endDateTime) {
                    double duration = lastInTime.secsTo(eventDateTime) / 3600.0; 
                    totalHours += duration;
                }
                hasInTime = false;
            }
        }

        return totalHours;
    }

    QString AttendanceEventRepositorySQLite::getLastAttendanceType(const QString& userId) {
        QString lastType;
        QSqlQuery query;

   
        QString queryString = "SELECT type FROM attendance_event WHERE user_id = :userId ORDER BY date DESC LIMIT 1";
        query.prepare(queryString);
        query.bindValue(":userId", userId);


        if (!query.exec()) {
            qDebug() << "Error fetching last attendance type:" << query.lastError().text();
            return lastType; 
        }

        if (query.next()) {
            lastType = query.value("type").toString();
        }

        return lastType; // Returns the value "In" or "Out" or empty if there is no record
    }

