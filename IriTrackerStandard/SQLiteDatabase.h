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
    BulletinBoardRepository* getBulletinBoardRepository();
    UserBulletinBoardRepository* getUserBulletinBoardRepository();

private:
    UserRepository* userRepository;
    DepartmentRepository* departmentRepository;
    AttendanceEventRepository* attendanceEventRepository;
    ExceptionRepository* exceptionRepository;
    BulletinBoardRepository* bulletinBoardRepository;
    UserBulletinBoardRepository* userBulletinBoardRepository;
};

#endif // SQLITEDATABASE_H
