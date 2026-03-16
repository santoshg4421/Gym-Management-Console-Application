#pragma once

#include <string>

// Represents a login account used by admin, trainer, or member roles.
class User {
public:
    // Unique numeric identifier for the user account.
    int userId;
    // Login name used for authentication.
    std::string username;
    // Plain-text password (current implementation).
    std::string password;
    // Access role: member, trainer, or admin.
    std::string role;

    // Creates an empty user object with default values.
    User();
    // Creates a fully initialized user object.
    User(int id, const std::string& uname, const std::string& pwd, const std::string& userRole);

    // Compares the provided password with the stored password.
    bool checkPassword(const std::string& inputPassword) const;
};
