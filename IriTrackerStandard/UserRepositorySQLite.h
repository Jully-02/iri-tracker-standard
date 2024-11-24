#ifndef USERREPOSITORYSQLITE_H
#define USERREPOSITORYSQLITE_H

#include "UserRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include <QVariantList>

class UserRepositorySQLite : public UserRepository {
public:
    UserRepositorySQLite() = default;
    ~UserRepositorySQLite() = default;

    bool insert(const User& user) override;
    bool update(const User& user) override;
    bool deleteItem(QString id) override;
    QList<User> selectAll(bool condition) override;
    User selectById(QString userId) override;
    QList<QVariantList> selectByDepartmentId(int departmentId) override;
    bool checkInOut(const QString& userId, const QString& password) override;
    User checkIfAdminExist() override;
};

#endif // USERREPOSITORYSQLITE_H
