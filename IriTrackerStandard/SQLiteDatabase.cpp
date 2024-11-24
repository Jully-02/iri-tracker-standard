#include "SQLiteDatabase.h"
#include "UserRepositorySQLite.h"
#include "DepartmentRepositorySQLite.h"
#include "AttendanceEventRepositorySQLite.h"
#include "ExceptionRepositorySQLite.h"

SQLiteDatabase::SQLiteDatabase() {
    userRepository = new UserRepositorySQLite();
    departmentRepository = new DepartmentRepositorySQLite();
    attendanceEventRepository = new AttendanceEventRepositorySQLite();
    exceptionRepository = new ExceptionRepositorySQLite();
}

SQLiteDatabase::~SQLiteDatabase() {
    delete userRepository;
    delete departmentRepository;
    delete attendanceEventRepository;
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
