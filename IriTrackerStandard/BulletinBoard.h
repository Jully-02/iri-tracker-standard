#ifndef BULLETINBOARD_H
#define BULLETINBOARD_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

class BulletinBoard {
public:
    // Constructors
    BulletinBoard();
    BulletinBoard(int bulletinBoardId, const QString& title, const QString& content, qint64 startDate, qint64 endDate, bool isActive, bool isHighPriority, const QDateTime& createdAt, const QDateTime& updatedAt, QString userId);

    // Getters and Setters
    int getBulletinBoardId() const;
    void setBulletinBoardId(int bulletinBoardId);

    QString getTitle() const;
    void setTitle(const QString& title);

    QString getContent() const;
    void setContent(const QString& content);

    qint64 getStartDate() const;
    void setStartDate(qint64 startDate);

    qint64 getEndDate() const;
    void setEndDate(qint64 endDate);

    bool getIsActive() const;
    void setIsActive(bool isActive);

    bool getIsHighPriority() const;
    void setIsHighPriority(bool isHighPriority);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& createdAt);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& updatedAt);

    QString getUserId() const;
    void setUserId(QString userId);

private:
    int m_bulletinBoardId;
    QString m_title;
    QString m_content;
    qint64 m_startDate;
    qint64 m_endDate;
    bool m_isActive;
    bool m_isHighPriority;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
    QString m_userId;
};

#endif // BULLETINBOARD_H
