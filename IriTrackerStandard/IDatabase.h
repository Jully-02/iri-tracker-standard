#ifndef IDATABASE_H
#define IDATABASE_H

#include "UserRepository.h"
#include "DepartmentRepository.h"
#include "AttendanceEventRepository.h"
#include "ExceptionRepository.h"
#include "BulletinBoardRepository.h"
#include "UserBulletinBoardRepository.h"

class IDatabase {
public:
    virtual ~IDatabase() {}

    // Phương thức khởi tạo các repository cụ thể
    virtual UserRepository* getUserRepository() = 0;
    virtual DepartmentRepository* getDepartmentRepository() = 0;
    virtual AttendanceEventRepository* getAttendanceEventRepository() = 0;
    virtual ExceptionRepository* getExceptionRepository() = 0;
    virtual BulletinBoardRepository* getBulletinBoardRepository() = 0;
    virtual UserBulletinBoardRepository* getUserBulletinBoardRepository() = 0;
    // Thêm các repository khác nếu cần
};

#endif // IDATABASE_H
