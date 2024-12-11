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

    bool insert(const Exception& exception) override;
    bool update(const Exception& exception) override;
    bool deleteItem(int id) override;
    QList<Exception> selectAll() override;
    Exception selectById(int id) override;
    bool selectByName(const QString& name) override;
};

#endif // EXCEPTIONREPOSITORYMYSQL_H
