#ifndef DEPARTMENTREPOSITORYMYSQL_H
#define DEPARTMENTREPOSITORYMYSQL_H

#include "DepartmentRepository.h"
#include "Department.h" 
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class DepartmentRepositoryMySQL : public DepartmentRepository {
public:
    bool insert(const Department& department) override;
    bool update(const Department& department) override;
    Department selectById(int id) override;
    bool deleteItem(int id) override;
    QList<Department> selectAll(bool condition) override;
    Department existByName(const QString& name) override;
};

#endif // DEPARTMENTREPOSITORYMYSQL_H
