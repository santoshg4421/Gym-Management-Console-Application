#pragma once

#include <string>

// Provides centralized input and domain validation rules.
class ValidationService {
public:
    // Validates an integer menu choice is within bounds.
    bool validateMenuChoice(int value, int min, int max) const;
    // Validates age range.
    bool validateAge(int age) const;
    // Validates height range in centimeters.
    bool validateHeight(double height) const;
    // Validates weight range in kilograms.
    bool validateWeight(double weight) const;
    // Validates non-empty text input.
    bool validateText(const std::string& input) const;
};
