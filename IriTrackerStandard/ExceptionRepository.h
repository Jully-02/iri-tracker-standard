#ifndef EXCEPTIONREPOSITORY_H
#define EXCEPTIONREPOSITORY_H

#include "Exception.h"
class ExceptionRepository {
public:
    virtual ~ExceptionRepository() {}

    virtual bool insert(const Exception& exception) = 0;
    virtual bool update(const Exception& exception) = 0;
    virtual bool deleteItem(int id) = 0;
    virtual QList<Exception> selectAll() = 0;
    virtual Exception selectById(int id) = 0;
};

#endif // EXCEPTIONREPOSITORY_H
