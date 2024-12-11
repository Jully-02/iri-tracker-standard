#ifndef EXCEPTIONREPOSITORYSQLITE_H
#define EXCEPTIONREPOSITORYSQLITE_H

#include "ExceptionRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>

class ExceptionRepositorySQLite : public ExceptionRepository {
public:
    ExceptionRepositorySQLite() = default;
    ~ExceptionRepositorySQLite() = default;

    bool insert(const Exception& exception) override;
    bool update(const Exception& exception) override;
    bool deleteItem(int id) override;
    QList<Exception> selectAll() override;
    Exception selectById(int id) override;
    bool selectByName(const QString& name) override;
};

#endif // EXCEPTIONREPOSITORYSQLITE_H
