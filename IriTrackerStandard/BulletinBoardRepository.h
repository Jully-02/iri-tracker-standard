#ifndef BULLETINBOARDREPOSITORY_H
#define BULLETINBOARDREPOSITORY_H

#include "BulletinBoard.h"

class BulletinBoardRepository {
public:
    virtual ~BulletinBoardRepository() {}

    virtual BulletinBoard insert(const BulletinBoard& bullentinBoard) = 0;
    virtual bool update(const BulletinBoard& bullentinBoard) = 0;  
    virtual bool deleteItem(int id) = 0;  // Phương thức thuần ảo
    virtual QList<BulletinBoard> selectAll() = 0;  
    virtual BulletinBoard selectById(int id) = 0; 
    virtual BulletinBoard existByName(const QString& name) = 0;
    virtual QList<BulletinBoard> selectBulletinsForUser(QString userId) = 0;
};

#endif // BULLETINBOARDREPOSITORY_H
