// SingletonManager.h
#ifndef SINGLETON_MANAGER_H
#define SINGLETON_MANAGER_H

#include <memory>
#include "IriTracker.h"
#include "EmployeeForm.h"
#include <QByteArray>

class SingletonManager {
public:
    static SingletonManager& getInstance();
    static QByteArray ms;

    IriTracker* getIriTracker();
    IriTracker* getIriTrackerForm();
    IriTracker* getIriTrackerInOut();
    EmployeeForm* getEmployeeForm();

private:
    SingletonManager();
    ~SingletonManager();

    SingletonManager(const SingletonManager&) = delete;
    SingletonManager& operator=(const SingletonManager&) = delete;

    std::unique_ptr<IriTracker> iriTracker;
    std::unique_ptr<IriTracker> iriTrackerForm;
    std::unique_ptr<IriTracker> iriTrackerInOut;
    std::unique_ptr<EmployeeForm> employeeForm;
};

#endif // SINGLETON_MANAGER_H