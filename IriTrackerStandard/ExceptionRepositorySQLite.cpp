#include "ExceptionRepositorySQLite.h"
#include <QDebug>

bool ExceptionRepositorySQLite::selectByName(const QString& name) {
    // Chuẩn bị câu lệnh SQL để đếm số lượng bản ghi có tên trùng
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM exception WHERE name = :name");

    // Ràng buộc giá trị name vào tham số :name
    query.bindValue(":name", name);

    // Thực thi câu lệnh SQL
    if (!query.exec()) {
        qDebug() << "Error counting exception by name:" << query.lastError().text();
        return false;  // Trả về false nếu có lỗi trong quá trình truy vấn
    }

    // Kiểm tra kết quả trả về
    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;  // Trả về true nếu có ít nhất 1 bản ghi, ngược lại false
    }

    return false;  // Trả về false nếu không có kết quả
}


bool ExceptionRepositorySQLite::insert(const Exception& exception) {
    bool existingExceptions = selectByName(exception.getName());

    // Nếu đã tồn tại ít nhất 1 bản ghi với name trùng, không cho phép insert
    if (existingExceptions) {
        qDebug() << "Error: The exception with name" << exception.getName() << "already exists.";
        return false;  // Trả về false nếu đã tồn tại
    }
    // Chuẩn bị câu lệnh SQL để chèn dữ liệu
    QSqlQuery query;
    query.prepare("INSERT INTO exception (name, paid_hours, paid_coefficient, work_coefficient, is_overtime) "
        "VALUES (:name, :paid_hours, :paid_coefficient, :work_coefficient, :is_overtime)");

    // Ràng buộc các tham số vào câu lệnh SQL
    query.bindValue(":name", exception.getName());
    query.bindValue(":paid_hours", exception.getPaidHours());
    query.bindValue(":paid_coefficient", exception.getPaidCoefficient());
    query.bindValue(":work_coefficient", exception.getWorkCoefficient());
    query.bindValue(":is_overtime", exception.getIsOvertime());

    // Thực thi câu lệnh SQL
    if (!query.exec()) {
        qDebug() << "Error inserting exception:" << query.lastError().text();
        return false;  // Trả về false nếu có lỗi
    }

    return true;  // Trả về true nếu việc chèn thành công
}


bool ExceptionRepositorySQLite::update(const Exception& exception) {
    // Chuẩn bị câu lệnh SQL để cập nhật dữ liệu
    QSqlQuery query;
    query.prepare("UPDATE exception SET "
        "name = :name, "
        "paid_hours = :paid_hours, "
        "paid_coefficient = :paid_coefficient, "
        "work_coefficient = :work_coefficient, "
        "is_overtime = :is_overtime "
        "WHERE exception_id = :exception_id");

    // Ràng buộc các tham số vào câu lệnh SQL
    query.bindValue(":name", exception.getName());
    query.bindValue(":paid_hours", exception.getPaidHours());
    query.bindValue(":paid_coefficient", exception.getPaidCoefficient());
    query.bindValue(":work_coefficient", exception.getWorkCoefficient());
    query.bindValue(":is_overtime", exception.getIsOvertime());
    query.bindValue(":exception_id", exception.getExceptionId());  // Điều kiện WHERE

    // Thực thi câu lệnh SQL
    if (!query.exec()) {
        qDebug() << "Error updating exception:" << query.lastError().text();
        return false;  // Trả về false nếu có lỗi
    }

    return true;  // Trả về true nếu việc cập nhật thành công
}


bool ExceptionRepositorySQLite::deleteItem(int id) {
    // Chuẩn bị câu lệnh SQL để xóa một bản ghi
    QSqlQuery query;
    query.prepare("DELETE FROM exception WHERE exception_id = :exception_id");

    // Ràng buộc giá trị id vào tham số :exception_id
    query.bindValue(":exception_id", id);

    // Thực thi câu lệnh SQL
    if (!query.exec()) {
        qDebug() << "Error deleting exception:" << query.lastError().text();
        return false;  // Trả về false nếu có lỗi
    }

    return true;  // Trả về true nếu việc xóa thành công
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
    QSqlQuery query;

    // Chuẩn bị câu truy vấn
    query.prepare("SELECT name, paid_hours, paid_coefficient, work_coefficient, is_overtime FROM exception WHERE exception_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error selecting exception:" << query.lastError().text();
        return exception;
    }

    if (query.next()) {
        exception.setExceptionId(id);
        exception.setName(query.value(0).toString()); // Cột đầu tiên là name
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

