#ifndef USERBULLETINBOARDREPOSITORYSQLITE_H
#define USERBULLETINBOARDREPOSITORYSQLITE_H

#include "UserBulletinBoardRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class UserBulletinBoardRepositorySQLite : public UserBulletinBoardRepository {
public:
    UserBulletinBoardRepositorySQLite() = default;
    ~UserBulletinBoardRepositorySQLite() = default;

    bool insert(const UserBulletinBoard& userBulletinBoard) override;
    bool update(const UserBulletinBoard& userBulletinBoard) override;
    bool deleteByBulletinBoardId(int bulletinBoardId) override;
    // QList<UserBulletinBoard> selectAll() override;
    QList<UserBulletinBoard> selectByUserId(QString userId) override;
    QList<UserBulletinBoard> selectByBulletinBoardId(int bulletinBoardId) override;
};

#endif // USERBULLETINBOARDREPOSITORYSQLITE_H
