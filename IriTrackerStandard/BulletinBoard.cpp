#include "BulletinBoard.h"
#include <QDebug>

// Constructors
BulletinBoard::BulletinBoard()
    : m_bulletinBoardId(0), m_isActive(false), m_isHighPriority(false), m_userId("") {}

BulletinBoard::BulletinBoard(int bulletinBoardId, const QString& title, const QString& content, qint64 startDate, qint64 endDate, bool isActive, bool isHighPriority, const QDateTime& createdAt, const QDateTime& updatedAt, QString userId)
    : m_bulletinBoardId(bulletinBoardId), m_title(title), m_content(content), m_startDate(startDate), m_endDate(endDate), m_isActive(isActive), m_isHighPriority(isHighPriority), m_createdAt(createdAt), m_updatedAt(updatedAt), m_userId(userId) {}

// Getters and Setters
int BulletinBoard::getBulletinBoardId() const { return m_bulletinBoardId; }
void BulletinBoard::setBulletinBoardId(int bulletinBoardId) { m_bulletinBoardId = bulletinBoardId; }

QString BulletinBoard::getTitle() const { return m_title; }
void BulletinBoard::setTitle(const QString& title) { m_title = title; }

QString BulletinBoard::getContent() const { return m_content; }
void BulletinBoard::setContent(const QString& content) { m_content = content; }

qint64 BulletinBoard::getStartDate() const { return m_startDate; }
void BulletinBoard::setStartDate(qint64 startDate) { m_startDate = startDate; }

qint64 BulletinBoard::getEndDate() const { return m_endDate; }
void BulletinBoard::setEndDate(qint64 endDate) { m_endDate = endDate; }

bool BulletinBoard::getIsActive() const { return m_isActive; }
void BulletinBoard::setIsActive(bool isActive) { m_isActive = isActive; }

bool BulletinBoard::getIsHighPriority() const { return m_isHighPriority; }
void BulletinBoard::setIsHighPriority(bool isHighPriority) { m_isHighPriority = isHighPriority; }

QDateTime BulletinBoard::getCreatedAt() const { return m_createdAt; }
void BulletinBoard::setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

QDateTime BulletinBoard::getUpdatedAt() const { return m_updatedAt; }
void BulletinBoard::setUpdatedAt(const QDateTime& updatedAt) { m_updatedAt = updatedAt; }

QString BulletinBoard::getUserId() const { return m_userId; }
void BulletinBoard::setUserId(QString userId) { m_userId = userId; }
