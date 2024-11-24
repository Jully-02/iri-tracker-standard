#include "MySQLDatabase.h"
#include "UserRepositoryMySQL.h"
#include "DepartmentRepositoryMySQL.h"
#include "AttendanceEventRepositoryMySQL.h"
#include "ExceptionRepositoryMySQL.h"

MySQLDatabase::MySQLDatabase() {
    // Khởi tạo các repository cụ thể cho MySQL
    userRepository = new UserRepositoryMySQL();
    departmentRepository = new DepartmentRepositoryMySQL();
    attendanceEventRepository = new AttendanceEventRepositoryMySQL();
    exceptionRepository = new ExceptionRepositoryMySQL();
}

MySQLDatabase::~MySQLDatabase() {
    delete userRepository;
    delete departmentRepository;
    delete attendanceEventRepository;
    delete exceptionRepository;
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
