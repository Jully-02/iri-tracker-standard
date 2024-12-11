#ifndef USERBULLETINBOARDREPOSITORYMYSQL_H
#define USERBULLETINBOARDREPOSITORYMYSQL_H

#include "UserBulletinBoardRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class UserBulletinBoardRepositoryMySQL : public UserBulletinBoardRepository {
public:
    UserBulletinBoardRepositoryMySQL() = default;
    ~UserBulletinBoardRepositoryMySQL() = default;

    bool insert(const UserBulletinBoard& userBulletinBoard) override;
    bool update(const UserBulletinBoard& userBulletinBoard) override;
    bool deleteByBulletinBoardId(int bulletinBoardId) override;
    // QList<UserBulletinBoard> selectAll() override;
    QList<UserBulletinBoard> selectByUserId(QString userId) override;
    QList<UserBulletinBoard> selectByBulletinBoardId(int bulletinBoardId) override;
};

#endif // USERBULLETINBOARDREPOSITORYMYSQL_H
