#include "UserBulletinBoard.h"
#include <QDebug>

// Constructors
UserBulletinBoard::UserBulletinBoard()
    : m_userBulletinBoardId(0) {}

UserBulletinBoard::UserBulletinBoard(int userBulletinBoardId, const QString& userId, int bulletinBoardId)
    : m_userBulletinBoardId(userBulletinBoardId), m_userId(userId), m_bulletinBoardId(bulletinBoardId) {}

// Getters and Setters
int UserBulletinBoard::getUserBulletinBoardId() const { return m_userBulletinBoardId; }
void UserBulletinBoard::setUserBulletinBoardId(int userBulletinBoardId) { m_userBulletinBoardId = userBulletinBoardId; }

QString UserBulletinBoard::getUserId() const { return m_userId; }
void UserBulletinBoard::setUserId(const QString& userId) { m_userId = userId; }

int UserBulletinBoard::getBulletinBoardId() const { return m_bulletinBoardId; }
void UserBulletinBoard::setBulletinBoardId(int bulletinBoardId) { m_bulletinBoardId = bulletinBoardId; }
