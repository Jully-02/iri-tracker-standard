#ifndef MYSQLDATABASE_H
#define MYSQLDATABASE_H

#include "IDatabase.h"

class MySQLDatabase : public IDatabase {
public:
    MySQLDatabase();
    ~MySQLDatabase();

    UserRepository* getUserRepository() override;
    DepartmentRepository* getDepartmentRepository() override;
    AttendanceEventRepository* getAttendanceEventRepository() override;
    ExceptionRepository* getExceptionRepository() override;

private:
    UserRepository* userRepository;
    DepartmentRepository* departmentRepository;
    AttendanceEventRepository* attendanceEventRepository;
    ExceptionRepository* exceptionRepository;
};

#endif // MYSQLDATABASE_H
