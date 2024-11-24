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

    virtual bool insert(const Exception& exception) override;
    virtual bool update(const Exception& exception) override;
    virtual bool deleteItem(int id) override;
    virtual QList<Exception> selectAll() override;
    virtual Exception selectById(int id) override;
};

#endif // EXCEPTIONREPOSITORYSQLITE_H
