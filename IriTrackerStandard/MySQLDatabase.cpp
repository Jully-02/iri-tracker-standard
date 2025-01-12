#include "MySQLDatabase.h"
#include "UserRepositoryMySQL.h"
#include "DepartmentRepositoryMySQL.h"
#include "AttendanceEventRepositoryMySQL.h"
#include "ExceptionRepositoryMySQL.h"
#include "BulletinBoardRepositoryMySQL.h"
#include "UserBulletinBoardRepositoryMySQL.h"

MySQLDatabase::MySQLDatabase() {
    userRepository = new UserRepositoryMySQL();
    departmentRepository = new DepartmentRepositoryMySQL();
    attendanceEventRepository = new AttendanceEventRepositoryMySQL();
    exceptionRepository = new ExceptionRepositoryMySQL();
    bulletinBoardRepository = new BulletinBoardRepositoryMySQL();
    userBulletinBoardRepository = new UserBulletinBoardRepositoryMySQL();
}

MySQLDatabase::~MySQLDatabase() {
    delete userRepository;
    delete departmentRepository;
    delete attendanceEventRepository;
    delete exceptionRepository;
    delete bulletinBoardRepository;
    delete userBulletinBoardRepository;
}

UserRepository* MySQLDatabase::getUserRepository() {
    return userRepository;
}

DepartmentRepository* MySQLDatabase::getDepartmentRepository() {
    return departmentRepository;
}

AttendanceEventRepository* MySQLDatabase::getAttendanceEventRepository() {
    return attendanceEventRepository;
}

ExceptionRepository* MySQLDatabase::getExceptionRepository() {
    return exceptionRepository;
}

BulletinBoardRepository* MySQLDatabase::getBulletinBoardRepository() {
    return bulletinBoardRepository;
}

UserBulletinBoardRepository* MySQLDatabase::getUserBulletinBoardRepository() {
    return userBulletinBoardRepository;
}