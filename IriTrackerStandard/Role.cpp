#include "Role.h"
#include <QDebug>

// Constructors
Role::Role() : m_roleId(0) {}

Role::Role(int roleId, const QString& name, const QString& desc, const QDateTime& createdAt, const QDateTime& updatedAt)
    : m_roleId(roleId), m_name(name), m_desc(desc), m_createdAt(createdAt), m_updatedAt(updatedAt) {}

// Getters and Setters
int Role::getRoleId() const { return m_roleId; }
void Role::setRoleId(int roleId) { m_roleId = roleId; }

QString Role::getName() const { return m_name; }
void Role::setName(const QString& name) { m_name = name; }

QString Role::getDesc() const { return m_desc; }
void Role::setDesc(const QString& desc) { m_desc = desc; }

QDateTime Role::getCreatedAt() const { return m_createdAt; }
void Role::setCreatedAt(const QDateTime& createdAt) { m_createdAt = createdAt; }

QDateTime Role::getUpdatedAt() const { return m_updatedAt; }
void Role::setUpdatedAt(const QDateTime& updatedAt) { m_updatedAt = updatedAt; }

