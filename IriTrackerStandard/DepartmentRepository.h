#ifndef DEPARTMENTREPOSITORY_H
#define DEPARTMENTREPOSITORY_H

#include "Department.h"

class DepartmentRepository {
public:
    virtual ~DepartmentRepository() {}

    virtual bool insert(const Department& department) = 0;  
    virtual bool update(const Department& department) = 0;  
    virtual bool deleteItem(int id) = 0;  
    virtual QList<Department> selectAll(bool condition = false) = 0;  
    virtual Department selectById(int id) = 0;  
    virtual Department existByName(const QString& name) = 0;
};

#endif // DEPARTMENTREPOSITORY_H
