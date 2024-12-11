#include "SQLiteDatabase.h"
#include "UserRepositorySQLite.h"
#include "DepartmentRepositorySQLite.h"
#include "AttendanceEventRepositorySQLite.h"
#include "ExceptionRepositorySQLite.h"
#include "BulletinBoardRepositorySQLite.h"
#include "UserBulletinBoardRepositorySQLite.h"

SQLiteDatabase::SQLiteDatabase() {
    userRepository = new UserRepositorySQLite();
    departmentRepository = new DepartmentRepositorySQLite();
    attendanceEventRepository = new AttendanceEventRepositorySQLite();
    exceptionRepository = new ExceptionRepositorySQLite();
    bulletinBoardRepository = new BulletinBoardRepositorySQLite();
    userBulletinBoardRepository = new UserBulletinBoardRepositorySQLite();
}

SQLiteDatabase::~SQLiteDatabase() {
    delete userRepository;
    delete departmentRepository;
    delete attendanceEventRepository;
    delete bulletinBoardRepository;
    delete userBulletinBoardRepository;
}

UserRepository* SQLiteDatabase::getUserRepository() {
    return userRepository;
}

DepartmentRepository* SQLiteDatabase::getDepartmentRepository() {
    return departmentRepository;
}

AttendanceEventRepository* SQLiteDatabase::getAttendanceEventRepository() {
    return attendanceEventRepository;
}

ExceptionRepository* SQLiteDatabase::getExceptionRepository() {
    return exceptionRepository;
}

BulletinBoardRepository* SQLiteDatabase::getBulletinBoardRepository() {
    return bulletinBoardRepository;
}

UserBulletinBoardRepository* SQLiteDatabase::getUserBulletinBoardRepository() {
    return userBulletinBoardRepository;
}
