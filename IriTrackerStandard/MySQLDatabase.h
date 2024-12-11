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
    BulletinBoardRepository* getBulletinBoardRepository() override;
    UserBulletinBoardRepository* getUserBulletinBoardRepository() override;

private:
    UserRepository* userRepository;
    DepartmentRepository* departmentRepository;
    AttendanceEventRepository* attendanceEventRepository;
    ExceptionRepository* exceptionRepository;
    BulletinBoardRepository* bulletinBoardRepository;
    UserBulletinBoardRepository* userBulletinBoardRepository;
};

#endif // MYSQLDATABASE_H
