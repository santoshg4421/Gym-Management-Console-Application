// BMI service implementation.
#include "BMIService.h"

// Computes BMI using metric units (height in cm, weight in kg).
double BMIService::calculateBMI(double height, double weight) const {
    if (height <= 0.0) {
        return 0.0;
    }

    double heightInMeters = height / 100.0;
    if (heightInMeters <= 0.0) {
        return 0.0;
    }

    return weight / (heightInMeters * heightInMeters);
}

// Maps BMI value to a standard category.
std::string BMIService::getBMICategory(double bmi) const {
    if (bmi < 18.5) {
        return "Underweight";
    }
    if (bmi < 25.0) {
        return "Normal";
    }
    if (bmi < 30.0) {
        return "Overweight";
    }
    return "Obese";
}
