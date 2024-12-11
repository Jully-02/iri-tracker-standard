#ifndef DEPARTMENTREPOSITORY_H
#define DEPARTMENTREPOSITORY_H

#include "Department.h"

class DepartmentRepository {
public:
    virtual ~DepartmentRepository() {}

    virtual bool insert(const Department& department) = 0;  // Phương thức thuần ảo
    virtual bool update(const Department& department) = 0;  // Phương thức thuần ảo
    virtual bool deleteItem(int id) = 0;  // Phương thức thuần ảo
    virtual QList<Department> selectAll(bool condition = false) = 0;  // Phương thức thuần ảo
    virtual Department selectById(int id) = 0;  // Phương thức thuần ảo
    virtual Department existByName(const QString& name) = 0;
};

#endif // DEPARTMENTREPOSITORY_H
