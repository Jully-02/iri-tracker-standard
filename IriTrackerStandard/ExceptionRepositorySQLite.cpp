#include "ExceptionRepositorySQLite.h"
#include <QDebug>

bool ExceptionRepositorySQLite::insert(const Exception& exception) {
    return true;
}

bool ExceptionRepositorySQLite::update(const Exception& exception) {
    return true;
}

bool ExceptionRepositorySQLite::deleteItem(int id) {
    return true;
}

QList<Exception> ExceptionRepositorySQLite::selectAll() {
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

Exception ExceptionRepositorySQLite::selectById(int id) {
    Exception exception;
    QSqlQuery query("SELECT name, paid_hours, paid_coefficient, work_coefficient, is_overtime FROM exception WHERE exception_id = :id");
    query.bindValue(":exception_id", id);

    if (!query.exec()) {
        qDebug() << "Error selecting exception:" << query.lastError().text();
        return exception;
    }

    if (query.next()) {
        exception.setExceptionId(id);
        exception.setName(query.value(1).toString());
        exception.setPaidHours(query.value(2).toLongLong());
        exception.setPaidCoefficient(query.value(3).toDouble());
        exception.setWorkCoefficient(query.value(4).toDouble());
        exception.setIsOvertime(query.value(5).toInt());
    }
    else {
        qDebug() << "No exception found with user_id:" << id;
    }

    return exception;
}