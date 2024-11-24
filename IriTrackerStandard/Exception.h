#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlDatabase>

class Exception {
public:
    // Constructors
    Exception();
    Exception(int exceptionId, const QString& name, qint64 paidHours, double paidCoefficient, double workCoefficient, int isOvertime, const QDateTime& createdAt, const QDateTime& updatedAt);

    // Getters and Setters
    int getExceptionId() const;
    void setExceptionId(int exceptionId);

    QString getName() const;
    void setName(const QString& name);

    qint64 getPaidHours() const;
    void setPaidHours(qint64 paidHours);

    double getPaidCoefficient() const;
    void setPaidCoefficient(double paidCoefficient);

    double getWorkCoefficient() const;
    void setWorkCoefficient(double workCoefficient);

    QDateTime getCreatedAt() const;
    void setCreatedAt(const QDateTime& createdAt);

    QDateTime getUpdatedAt() const;
    void setUpdatedAt(const QDateTime& updatedAt);

    int getIsOvertime() const;
    void setIsOvertime(int isOvertitme);

private:
    int m_exceptionId;
    QString m_name;
    qint64 m_paidHours;
    double m_paidCoefficient;
    double m_workCoefficient;
    int m_isOvertime;
    QDateTime m_createdAt;
    QDateTime m_updatedAt;
};

#endif // EXCEPTION_H
