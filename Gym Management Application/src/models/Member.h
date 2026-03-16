#pragma once

#include <string>

// Represents a gym member profile and membership details.
class Member {
public:
    // Unique numeric identifier for the member profile.
    int memberId;
    // Username linking this profile to a User account.
    std::string username;
    // Full member display name.
    std::string name;
    // Member age in years.
    int age;
    // Gender value selected in UI.
    std::string gender;
    // Height in centimeters.
    double height;
    // Weight in kilograms.
    double weight;
    // Membership plan type (monthly/quarterly/annual).
    std::string membershipType;
    // Membership start date (YYYY-MM-DD).
    std::string startDate;
    // Membership expiry date (YYYY-MM-DD).
    std::string expiryDate;

    // Creates a member object with default values.
    Member();

    // Updates editable profile fields.
    void updateProfile(const std::string& newName, int newAge, const std::string& newGender, double newHeight, double newWeight);
    // Returns a formatted one-line membership summary.
    std::string getMembershipDetails() const;
};
