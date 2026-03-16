#pragma once

#include <string>
#include <vector>

#include "../models/ProgressEntry.h"

class JsonDataStore;
class BMIService;

// Handles progress entry creation and progress history retrieval.
class ProgressService {
public:
    // Creates the service with storage and BMI dependencies.
    ProgressService(JsonDataStore& dataStore, BMIService& bmiService);

    // Adds a new progress entry and updates the member's current weight.
    bool addProgress(int memberId, const std::string& date, double weight, const std::string& achievement);
    // Returns all progress entries for a member.
    std::vector<ProgressEntry> viewProgressHistory(int memberId) const;

private:
    // Shared data access layer.
    JsonDataStore& storage;
    // BMI helper for recalculating entry BMI values.
    BMIService& bmi;
};
