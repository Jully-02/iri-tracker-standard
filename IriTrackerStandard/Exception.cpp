#include "Exception.h"
#include <QDebug>

// Constructors
Exception::Exception() : m_exceptionId(0), m_paidHours(0), m_paidCoefficient(0.0), m_workCoefficient(0.0) {}

Exception::Exception(int exceptionId, const QString& name, qint64 paidHours, double paidCoefficient, double workCoefficient, int isOvertime, const QDateTime& createdAt, const QDateTime& updatedAt)
    : m_exceptionId(exceptionId), m_name(name), m_paidHours(paidHours), m_paidCoefficient(paidCoefficient), m_workCoefficient(workCoefficient), m_isOvertime(isOvertime), m_createdAt(createdAt), m_updatedAt(updatedAt) {}

// Getters and Setters
int Exception::getExceptionId() const { return m_exceptionId; }
void Exception::setExceptionId(int exceptionId) { m_exceptionId = exceptionId; }

QString Exception::getName() const { return m_name; }
void Exception::setName(const QString& name) { m_name = name; }

qint64 Exception::getPaidHours() const { return m_paidHours; }
void Exception::setPaidHours(qint64 paidHours) { m_paidHours = paidHours; }

double Exception::getPaidCoefficient() const { return m_paidCoefficient; }
void Exception::setPaidCoefficient(double paidCoefficient) { m_paidCoefficient = paidCoefficient; }

double Exception::getWorkCoefficient() const { return m_workCoefficient; }
void Exception::setWorkCoefficient(double workCoefficient) { m_workCoefficient = workCoefficient; }

QDateTime Exception::getCreatedAt() const { return m_createdAt; }
void Exception::setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

QDateTime Exception::getUpdatedAt() const { return m_updatedAt; }
void Exception::setUpdatedAt(const QDateTime& updatedAt) { m_updatedAt = updatedAt; }

int Exception::getIsOvertime() const { return m_isOvertime; }
void Exception::setIsOvertime(int isOvertime) { m_isOvertime = isOvertime; }
