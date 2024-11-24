#ifndef USERREPOSITORY_H
#define USERREPOSITORY_H

#include "User.h"
class UserRepository {
public:
    virtual ~UserRepository() {}

    virtual bool insert(const User& user) = 0;
    virtual bool update(const User& user) = 0;
    virtual bool deleteItem(QString id) = 0;
    virtual QList<User> selectAll(bool condition = false) = 0;
    virtual User selectById(QString id) = 0;

    virtual QList<QVariantList> selectByDepartmentId(int departmentId) = 0;
    virtual bool checkInOut(const QString& userId, const QString& password) = 0;
    virtual User checkIfAdminExist() = 0;
};

#endif // USERREPOSITORY_H
