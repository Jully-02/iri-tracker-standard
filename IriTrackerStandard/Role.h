#ifndef ROLE_H
#define ROLE_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

class Role {
public:
    // Constructors
    Role();
    Role(int roleId, const QString& name, const QString& desc, const QDateTime& createdAt, const QDateTime& updatedAt);

    // Getters and Setters
    int getRoleId() const;
    void setRoleId(int roleId);

    QString getName() const;
    void setName(const QString& name);

    QString getDesc() const;
    void setDesc(const QString& desc);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& createdAt);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& updatedAt);

private:
    int m_roleId;
    QString m_name;
    QString m_desc;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // ROLE_H
