#ifndef USERREPOSITORYMYSQL_H
#define USERREPOSITORYMYSQL_H

#include "UserRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>
#include <QVariantList>

class UserRepositoryMySQL : public UserRepository {
public:
    UserRepositoryMySQL() = default;
    ~UserRepositoryMySQL() = default;

    bool insert(const User& user) override;
    bool update(const User& user) override;
    bool deleteItem(QString id) override;
    QList<User> selectAll(bool condition) override;
    User selectById(QString userId) override;
    QList<QVariantList> selectByDepartmentId(int departmentId) override;
    bool checkInOut(const QString& userId, const QString& password) override;
    User checkIfAdminExist() override;
    bool changePasswordAdmin(const QString& oldPassword, const QString& newPassword) override;
    QList<QPair<QString, QPair<QByteArray, QByteArray>>> selectAllEyes() override;
};

#endif // USERREPOSITORYMYSQL_H
