#pragma once

#include <map>
#include <string>

class JsonDataStore;
class BMIService;

// Computes aggregate gym statistics from persisted member data.
class StatisticsService {
public:
    // Creates the service with storage and BMI dependencies.
    StatisticsService(JsonDataStore& dataStore, BMIService& bmiService);

    // Returns total number of members.
    int getTotalMembers() const;
    // Returns average BMI across members.
    double getAverageBMI() const;
    // Returns count of members in each BMI category.
    std::map<std::string, int> getBMIDistribution() const;

private:
    // Shared data access layer.
    JsonDataStore& storage;
    // BMI helper used for calculation and category mapping.
    BMIService& bmi;
};
