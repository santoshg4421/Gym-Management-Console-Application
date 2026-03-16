#pragma once

#include <string>

// Provides BMI calculations and BMI category mapping.
class BMIService {
public:
    // Calculates BMI from height (cm) and weight (kg).
    double calculateBMI(double height, double weight) const;
    // Converts BMI value to category label.
    std::string getBMICategory(double bmi) const;
};
