#include "ExceptionRepositoryMySQL.h"

bool ExceptionRepositoryMySQL::selectByName(const QString& name) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM exception WHERE name = :name");

    query.bindValue(":name", name);

    if (!query.exec()) {
        qDebug() << "Error counting exception by name:" << query.lastError().text();
        return false; 
    }

    
    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0; 
    }

    return false; 
}



bool ExceptionRepositoryMySQL::insert(const Exception& exception) {
    bool existingExceptions = selectByName(exception.getName());

 
    if (existingExceptions) {
        qDebug() << "Error: The exception with name" << exception.getName() << "already exists.";
        return false; 
    }

    QSqlQuery query;
    query.prepare("INSERT INTO exception (name, paid_hours, paid_coefficient, work_coefficient, is_overtime) "
        "VALUES (:name, :paid_hours, :paid_coefficient, :work_coefficient, :is_overtime)");

   
    query.bindValue(":name", exception.getName());
    query.bindValue(":paid_hours", exception.getPaidHours());
    query.bindValue(":paid_coefficient", exception.getPaidCoefficient());
    query.bindValue(":work_coefficient", exception.getWorkCoefficient());
    query.bindValue(":is_overtime", exception.getIsOvertime());


    if (!query.exec()) {
        qDebug() << "Error inserting exception:" << query.lastError().text();
        return false;  
    }

    return true;  
}


bool ExceptionRepositoryMySQL::update(const Exception& exception) {
    
    QSqlQuery query;
    query.prepare("UPDATE exception SET "
        "name = :name, "
        "paid_hours = :paid_hours, "
        "paid_coefficient = :paid_coefficient, "
        "work_coefficient = :work_coefficient, "
        "is_overtime = :is_overtime "
        "WHERE exception_id = :exception_id");

    query.bindValue(":name", exception.getName());
    query.bindValue(":paid_hours", exception.getPaidHours());
    query.bindValue(":paid_coefficient", exception.getPaidCoefficient());
    query.bindValue(":work_coefficient", exception.getWorkCoefficient());
    query.bindValue(":is_overtime", exception.getIsOvertime());
    query.bindValue(":exception_id", exception.getExceptionId()); 

  
    if (!query.exec()) {
        qDebug() << "Error updating exception:" << query.lastError().text();
        return false;  
    }

    return true; 
}


bool ExceptionRepositoryMySQL::deleteItem(int id) {
   
    QSqlQuery query;
    query.prepare("DELETE FROM exception WHERE exception_id = :exception_id");

   
    query.bindValue(":exception_id", id);

  
    if (!query.exec()) {
        qDebug() << "Error deleting exception:" << query.lastError().text();
        return false;  
    }

    return true;  
}

QList<Exception> ExceptionRepositoryMySQL::selectAll() {
    QList<Exception> exceptions;
    QSqlQuery query("SELECT exception_id, name, paid_hours, paid_coefficient, work_coefficient, is_overtime FROM exception");

    if (!query.exec()) {
        qDebug() << "Error selecting exceptions:" << query.lastError().text();
        return exceptions;
    }

    while (query.next()) {
        Exception exception;
        exception.setExceptionId(query.value(0).toInt());
        exception.setName(query.value(1).toString());
        exception.setPaidHours(query.value(2).toLongLong());
        exception.setPaidCoefficient(query.value(3).toDouble());
        exception.setWorkCoefficient(query.value(4).toDouble());
        exception.setIsOvertime(query.value(5).toInt());

        exceptions.append(exception);
    }

    return exceptions;
}

Exception ExceptionRepositoryMySQL::selectById(int id) {
    Exception exception;
    QSqlQuery query;

   
    query.prepare("SELECT name, paid_hours, paid_coefficient, work_coefficient, is_overtime FROM exception WHERE exception_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error selecting exception:" << query.lastError().text();
        return exception;
    }

    if (query.next()) {
        exception.setExceptionId(id);
        exception.setName(query.value(0).toString()); 
        exception.setPaidHours(query.value(1).toLongLong());
        exception.setPaidCoefficient(query.value(2).toDouble());
        exception.setWorkCoefficient(query.value(3).toDouble());
        exception.setIsOvertime(query.value(4).toInt());
    }
    else {
        qDebug() << "No exception found with exception_id:" << id;
    }

    return exception;
}