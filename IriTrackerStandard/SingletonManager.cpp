#include "SingletonManager.h"

QByteArray SingletonManager::ms = NULL;

SingletonManager& SingletonManager::getInstance() {
	static SingletonManager instance;
	return instance;
}

SingletonManager::SingletonManager() = default;
SingletonManager::~SingletonManager() = default;

IriTracker* SingletonManager::getIriTracker() {
	if (!iriTracker) {
		iriTracker = std::make_unique<IriTracker>();
	}
	return iriTracker.get();
}

IriTracker* SingletonManager::getIriTrackerForm() {
	if (!iriTrackerForm) {
		iriTrackerForm = std::make_unique<IriTracker>();
	}
	return iriTrackerForm.get();
}

IriTracker* SingletonManager::getIriTrackerInOut() {
	if (!iriTrackerInOut) {
		iriTrackerInOut = std::make_unique<IriTracker>();
	}
	return iriTrackerInOut.get();
}

EmployeeForm* SingletonManager::getEmployeeForm() {
	if (!employeeForm) {
		employeeForm = std::make_unique<EmployeeForm>();
	}
	return employeeForm.get();
}