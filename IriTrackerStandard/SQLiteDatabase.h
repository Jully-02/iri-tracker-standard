#ifndef SQLITEDATABASE_H
#define SQLITEDATABASE_H

#include "IDatabase.h"

class SQLiteDatabase : public IDatabase {
public:
    SQLiteDatabase();
    ~SQLiteDatabase();

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

#endif // SQLITEDATABASE_H
