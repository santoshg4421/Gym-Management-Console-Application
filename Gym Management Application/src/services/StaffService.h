#pragma once

#include <string>

class JsonDataStore;

// Creates trainer and admin user accounts.
class StaffService {
public:
    // Creates the service with storage dependency.
    explicit StaffService(JsonDataStore& dataStore);

    // Creates a trainer account.
    bool createTrainerAccount(const std::string& username, const std::string& password);
    // Creates an admin account.
    bool createAdminAccount(const std::string& username, const std::string& password);

private:
    // Shared data access layer.
    JsonDataStore& storage;
};
