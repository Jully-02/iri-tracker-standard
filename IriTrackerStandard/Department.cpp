#include "Department.h"
#include <QDebug>

// Constructors
Department::Department() : m_departmentId(0) {}

Department::Department(int departmentId, const QString& name, const QString& desc, const QDateTime& createdAt, const QDateTime& updatedAt)
    : m_departmentId(departmentId), m_name(name), m_desc(desc), m_createdAt(createdAt), m_updatedAt(updatedAt) {}

Department::Department(int departmentId, const QString& name, const QString& desc)
    : m_departmentId(departmentId), m_name(name), m_desc(desc) {}
// Getters and Setters
int Department::getDepartmentId() const { return m_departmentId; }
void Department::setDepartmentId(int departmentId) { m_departmentId = departmentId; }

QString Department::getName() const { return m_name; }
void Department::setName(const QString& name) { m_name = name; }

QString Department::getDesc() const { return m_desc; }
void Department::setDesc(const QString& desc) { m_desc = desc; }

QDateTime Department::getCreatedAt() const { return m_createdAt; }
void Department::setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

QDateTime Department::getUpdatedAt() const { return m_updatedAt; }
void Department::setUpdatedAt(const QDateTime& updatedAt) { m_updatedAt = updatedAt; }
