#ifndef BULLETINBOARDREPOSITORYSQLITE_H
#define BULLETINBOARDREPOSITORYSQLITE_H

#include "BulletinBoardRepository.h"
#include "BulletinBoard.h" 
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QList>
#include <QDebug>

class BulletinBoardRepositorySQLite : public BulletinBoardRepository {
public:
    BulletinBoard insert(const BulletinBoard& bulletinBoard) override;
    bool update(const BulletinBoard& bulletinBoard) override;
    BulletinBoard selectById(int id) override;
    bool deleteItem(int id) override;
    QList<BulletinBoard> selectAll() override;
    BulletinBoard existByName(const QString& name) override;
    QList<BulletinBoard> selectBulletinsForUser(QString userId) override;
};

#endif // BULLETINBOARDREPOSITORYSQLITE_H
