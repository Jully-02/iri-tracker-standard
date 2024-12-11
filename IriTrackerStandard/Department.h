#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

class Department {
public:
    // Constructors
    Department();
    Department(int departmentId, const QString& name, const QString& desc, int isActive);
    Department(int departmentId, const QString& name, const QString& desc, int isActive, const QDateTime& createdAt, const QDateTime& updatedAt);

    // Getters and Setters
    int getDepartmentId() const;
    void setDepartmentId(int departmentId);

    QString getName() const;
    void setName(const QString& name);

    QString getDesc() const;
    void setDesc(const QString& desc);

    int getIsActive() const;
    void setIsActive(int isActive);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& createdAt);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& updatedAt);

private:
    int m_departmentId;
    QString m_name;
    QString m_desc;
    int m_isActive;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // DEPARTMENT_H
