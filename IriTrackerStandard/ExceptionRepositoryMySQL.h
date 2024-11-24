#ifndef EXCEPTIONREPOSITORYMYSQL_H
#define EXCEPTIONREPOSITORYMYSQL_H

#include "ExceptionRepository.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QList>

class ExceptionRepositoryMySQL : public ExceptionRepository {
public:
    ExceptionRepositoryMySQL() = default;
    ~ExceptionRepositoryMySQL() = default;

    virtual bool insert(const Exception& exception) override;
    virtual bool update(const Exception& exception) override;
    virtual bool deleteItem(int id) override;
    virtual QList<Exception> selectAll() override;
    virtual Exception selectById(int id) override;
};

#endif // EXCEPTIONREPOSITORYMYSQL_H
