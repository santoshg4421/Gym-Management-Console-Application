#pragma once

#include <string>

class JsonDataStore;

// Handles credential validation and role lookup.
class AuthService {
public:
    // Creates the service with storage dependency.
    explicit AuthService(JsonDataStore& dataStore);

    // Returns true when username/password are valid.
    bool login(const std::string& username, const std::string& password);
    // Returns role string for a user, or empty string if not found.
    std::string getRole(const std::string& username) const;

private:
    // Shared data access layer.
    JsonDataStore& storage;
};
