// Statistics service implementation.
#include "StatisticsService.h"

#include <vector>

#include "BMIService.h"
#include "../models/Member.h"
#include "../storage/JsonDataStore.h"

// Stores references to shared data services.
StatisticsService::StatisticsService(JsonDataStore& dataStore, BMIService& bmiService)
    : storage(dataStore), bmi(bmiService) {}

// Returns total number of member profiles.
int StatisticsService::getTotalMembers() const {
    return static_cast<int>(storage.members.size());
}

// Computes average BMI across all members.
double StatisticsService::getAverageBMI() const {
    if (storage.members.empty()) {
        return 0.0;
    }

    double total = 0.0;
    for (const Member& member : storage.members) {
        total += bmi.calculateBMI(member.height, member.weight);
    }

    return total / storage.members.size();
}

// Builds BMI category distribution counts.
std::map<std::string, int> StatisticsService::getBMIDistribution() const {
    std::map<std::string, int> distribution;

    for (const Member& member : storage.members) {
        double value = bmi.calculateBMI(member.height, member.weight);
        std::string category = bmi.getBMICategory(value);
        distribution[category]++;
    }

    return distribution;
}
