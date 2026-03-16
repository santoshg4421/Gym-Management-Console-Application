// User model implementation.
#include "User.h"

// Default constructor for safe initialization.
User::User() : userId(0) {}

// Value constructor used when creating accounts.
User::User(int id, const std::string& uname, const std::string& pwd, const std::string& userRole)
    : userId(id), username(uname), password(pwd), role(userRole) {}

// Returns true when passwords match.
bool User::checkPassword(const std::string& inputPassword) const {
    return password == inputPassword;
}
