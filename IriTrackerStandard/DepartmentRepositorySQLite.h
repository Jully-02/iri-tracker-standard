#ifndef DEPARTMENTREPOSITORYSQLITE_H
#define DEPARTMENTREPOSITORYSQLITE_H

#include "DepartmentRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class DepartmentRepositorySQLite : public DepartmentRepository {
public:
    DepartmentRepositorySQLite() = default;
    ~DepartmentRepositorySQLite() = default;

    bool insert(const Department& department) override;
    bool update(const Department& department) override;
    Department selectById(int id) override;
    bool deleteItem(int id) override;
    QList<Department> selectAll(bool condition) override;
    Department existByName(const QString& name) override;
};

#endif // DEPARTMENTREPOSITORYSQLITE_H
