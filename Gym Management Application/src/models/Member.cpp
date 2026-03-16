// Member model implementation.
#include "Member.h"

// Initializes numeric fields to safe defaults.
Member::Member()
    : memberId(0), age(0), height(0.0), weight(0.0) {}

// Replaces editable profile values with new inputs.
void Member::updateProfile(const std::string& newName, int newAge, const std::string& newGender, double newHeight, double newWeight) {
    name = newName;
    age = newAge;
    gender = newGender;
    height = newHeight;
    weight = newWeight;
}

// Produces a compact membership information string.
std::string Member::getMembershipDetails() const {
    return "Type: " + membershipType + " | Start: " + startDate + " | Expiry: " + expiryDate;
}
