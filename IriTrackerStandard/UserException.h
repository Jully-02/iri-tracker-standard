#ifndef USEREXCEPTION_H
#define USEREXCEPTION_H

#include <QString>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

class UserException {
public:
    // Constructors
    UserException();
    UserException(const QString& userExceptionId, qint64 startDate, qint64 endDate,
        bool isAllDate, int dayOfWeek, const QString& userId, const QString& exceptionId);

    // Getters and Setters
    QString getUserExceptionId() const;
    void setUserExceptionId(const QString& userExceptionId);

    qint64 getStartDate() const;
    void setStartDate(qint64 startDate);

    qint64 getEndDate() const;
    void setEndDate(qint64 endDate);

    bool getIsAllDate() const;
    void setIsAllDate(bool isAllDate);

    int getDayOfWeek() const;
    void setDayOfWeek(int dayOfWeek);

    QString getUserId() const;
    void setUserId(const QString& userId);

    QString getExceptionId() const;
    void setExceptionId(const QString& exceptionId);

private:
    QString m_userExceptionId; // ID của ngoại lệ người dùng
    qint64 m_startDate;     // Ngày bắt đầu
    qint64 m_endDate;       // Ngày kết thúc
    bool m_isAllDate;          // Cờ cho biết có phải là tất cả các ngày không
    int m_dayOfWeek;           // Ngày trong tuần
    QString m_userId;          // ID của người dùng
    QString m_exceptionId;     // ID của ngoại lệ
};

#endif // USEREXCEPTION_H
