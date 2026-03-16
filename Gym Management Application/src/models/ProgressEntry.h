#pragma once

#include <string>

// Stores one tracked progress snapshot for a member.
class ProgressEntry {
public:
    // Member profile identifier this entry belongs to.
    int memberId;
    // Entry date in YYYY-MM-DD format.
    std::string date;
    // Recorded weight in kilograms.
    double weight;
    // BMI value at the time of entry.
    double bmi;
    // Achievement note or short remark.
    std::string achievement;

    // Creates an empty progress entry with defaults.
    ProgressEntry();
};
