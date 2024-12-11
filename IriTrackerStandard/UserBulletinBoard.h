#ifndef USERBULLETINBOARD_H
#define USERBULLETINBOARD_H

#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class UserBulletinBoard {
public:
    // Constructors
    UserBulletinBoard();
    UserBulletinBoard(int userBulletinBoardId, const QString& userId, int bulletinBoardId);

    // Getters and Setters
    int getUserBulletinBoardId() const;
    void setUserBulletinBoardId(int userBulletinBoardId);

    QString getUserId() const;
    void setUserId(const QString& userId);

    int getBulletinBoardId() const;
    void setBulletinBoardId(int bulletinBoardId);

private:
    int m_userBulletinBoardId;
    QString m_userId;
    int m_bulletinBoardId;
};

#endif // USERBULLETINBOARD_H
