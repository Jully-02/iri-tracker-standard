#ifndef USERBULLETINBOARDREPOSITORY_H
#define USERBULLETINBOARDREPOSITORY_H

#include "UserBulletinBoard.h"
class UserBulletinBoardRepository {
public:
    virtual ~UserBulletinBoardRepository() {}

    virtual bool insert(const UserBulletinBoard& userBulletinBoard) = 0;
    virtual bool update(const UserBulletinBoard& userBulletinBoard) = 0;
    virtual bool deleteByBulletinBoardId(int bulletinBoardId) = 0;
    //virtual QList<UserBulletinBoard> selectAll() = 0;
    virtual QList<UserBulletinBoard> selectByUserId(QString userId) = 0;
    virtual QList<UserBulletinBoard> selectByBulletinBoardId(int bulletinBoardId) = 0;
};

#endif // USERBULLETINBOARDREPOSITORY_H
