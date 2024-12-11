#include "DepartmentRepositoryMySQL.h"

Department DepartmentRepositoryMySQL::existByName(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT department_id, name, description, is_active FROM department WHERE name = :name");
    query.bindValue(":name", name);
    Department department;
    if (!query.exec()) {
        qDebug() << "Error checking if department exists:" << query.lastError().text();
        return department;
    }

    if (query.next()) {
        department.setDepartmentId(query.value("department_id").toInt());
        department.setName(query.value("name").toString());
        department.setDesc(query.value("description").toString());
        department.setIsActive(query.value("is_active").toInt());
    }

    return department; 
}



    bool DepartmentRepositoryMySQL::insert(const Department& department) {
        Department existingDepartment = existByName(department.getName());

        if (existingDepartment.getDepartmentId() != 0) {
            if (existingDepartment.getIsActive() == 1) {
                return false;
            }
            else {
                existingDepartment.setIsActive(1);
                return update(existingDepartment);
            }
        }
        QSqlQuery query;
        query.prepare("INSERT INTO department (name, description, is_active, created_at) VALUES (:name, :desc, :is_active, :created_at)");
        query.bindValue(":name", department.getName());
        query.bindValue(":desc", department.getDesc());
        query.bindValue(":is_active", 1);
        query.bindValue(":created_at", QDateTime::currentDateTime());

        if (!query.exec()) {
            qDebug() << "Error inserting department:" << query.lastError().text();
            return false;
        }
        return true;
    }

    bool DepartmentRepositoryMySQL::update(const Department& department) {
        Department existingDepartment = selectById(department.getDepartmentId());

        QSqlQuery query;
        query.prepare("UPDATE department SET name = :name, description = :desc, is_active = :is_active, updated_at = :updated_at WHERE department_id = :id");
        query.bindValue(":id", department.getDepartmentId());
        query.bindValue(":name", department.getName().isEmpty() ? existingDepartment.getName() : department.getName());
        query.bindValue(":desc", department.getDesc().isEmpty() ? existingDepartment.getDesc() : department.getDesc());
        query.bindValue(":is_active", department.getIsActive() == -1 ? existingDepartment.getIsActive() : department.getIsActive());
        query.bindValue(":updated_at", QDateTime::currentDateTime());

        if (!query.exec()) {
            qDebug() << "Error updating department:" << query.lastError().text();
            return false;
        }
        return true;
    }

    Department DepartmentRepositoryMySQL::selectById(int id) {
        QSqlQuery query;
        query.prepare("SELECT department_id, name, description, is_active, created_at, updated_at FROM department WHERE department_id = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            qDebug() << "Error executing query:" << query.lastError().text();
            return Department(); // Trả về một đối tượng rỗng nếu có lỗi
        }

        Department department;
        if (query.next()) {
            department.setDepartmentId(id);
            department.setName(query.value("name").toString());
            department.setDesc(query.value("description").toString());
            department.setIsActive(query.value("is_active").toInt());
            department.setCreatedAt(query.value("created_at").toDateTime());
            department.setUpdatedAt(query.value("updated_at").toDateTime());
        }
        else {
            qDebug() << "No department found with ID:" << id;
        }

        return department;
    }

    bool DepartmentRepositoryMySQL::deleteItem(int id) {
        QSqlQuery query;
        query.prepare("UPDATE department SET is_active = false WHERE department_id = :id");
        query.bindValue(":id", id);

        if (!query.exec()) {
            qDebug() << "Error deleting department:" << query.lastError().text();
            return false;
        }
        return true;
    }

    QList<Department> DepartmentRepositoryMySQL::selectAll(bool condition) {
        QList<Department> departments;
        QSqlQuery query;

        if (condition) {
            query.prepare("SELECT department_id, name, description, is_active, created_at, updated_at FROM department WHERE department_id != 1 AND is_active = true");
        }
        else {
            query.prepare("SELECT department_id, name, description, is_active, created_at, updated_at FROM department WHERE is_active = true");
        }

        if (!query.exec()) {
            qDebug() << "Error selecting departments:" << query.lastError().text();
            return departments;
        }

        while (query.next()) {
            int departmentId = query.value("department_id").toInt();
            QString name = query.value("name").toString();
            QString desc = query.value("description").toString();
            int isActive = query.value("is_active").toInt();
            QDateTime createdAt = query.value("created_at").toDateTime();
            QDateTime updatedAt = query.value("updated_at").toDateTime();

            departments.append(Department(departmentId, name, desc, isActive, createdAt, updatedAt));
        }

        return departments;
    }