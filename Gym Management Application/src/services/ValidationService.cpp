// Validation service implementation.
#include "ValidationService.h"

// Validates menu choice falls in inclusive range.
bool ValidationService::validateMenuChoice(int value, int min, int max) const {
    return value >= min && value <= max;
}

// Validates acceptable member age bounds.
bool ValidationService::validateAge(int age) const {
    return age >= 10 && age <= 100;
}

// Validates acceptable height bounds in centimeters.
bool ValidationService::validateHeight(double height) const {
    return height >= 50.0 && height <= 250.0;
}

// Validates acceptable weight bounds in kilograms.
bool ValidationService::validateWeight(double weight) const {
    return weight > 20.0 && weight < 300.0;
}

// Validates non-empty text input.
bool ValidationService::validateText(const std::string& input) const {
    return !input.empty();
}
