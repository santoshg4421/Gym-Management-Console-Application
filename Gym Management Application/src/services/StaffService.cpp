// Staff account service implementation.
#include "StaffService.h"

#include "../models/User.h"
#include "../storage/JsonDataStore.h"

// Stores reference to shared data store.
StaffService::StaffService(JsonDataStore& dataStore) : storage(dataStore) {}

// Creates a trainer user account.
bool StaffService::createTrainerAccount(const std::string& username, const std::string& password) {
    int nextId = storage.generateNextUserId();
    return storage.addUser(User(nextId, username, password, "trainer"));
}

// Creates an admin user account.
bool StaffService::createAdminAccount(const std::string& username, const std::string& password) {
    int nextId = storage.generateNextUserId();
    return storage.addUser(User(nextId, username, password, "admin"));
}
