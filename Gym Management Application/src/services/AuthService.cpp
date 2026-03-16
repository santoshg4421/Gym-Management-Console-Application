// Authentication service implementation.
#include "AuthService.h"

#include "../storage/JsonDataStore.h"

// Stores reference to shared data store.
AuthService::AuthService(JsonDataStore& dataStore) : storage(dataStore) {}

// Validates user credentials.
bool AuthService::login(const std::string& username, const std::string& password) {
    const User* user = storage.findUserByUsername(username);
    return user != nullptr && user->checkPassword(password);
}

// Fetches role for a known username.
std::string AuthService::getRole(const std::string& username) const {
    const User* user = storage.findUserByUsername(username);
    if (user == nullptr) {
        return "";
    }
    return user->role;
}
