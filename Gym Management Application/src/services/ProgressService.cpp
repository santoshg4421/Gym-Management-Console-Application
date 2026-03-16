// Progress service implementation.
#include "ProgressService.h"

#include "BMIService.h"
#include "../models/Member.h"
#include "../storage/JsonDataStore.h"

// Stores references to shared data services.
ProgressService::ProgressService(JsonDataStore& dataStore, BMIService& bmiService)
    : storage(dataStore), bmi(bmiService) {}

// Adds a progress entry and syncs latest weight into member profile.
bool ProgressService::addProgress(int memberId, const std::string& date, double weight, const std::string& achievement) {
    Member* member = storage.findMemberById(memberId);
    if (member == nullptr) {
        return false;
    }

    member->weight = weight;

    ProgressEntry entry;
    entry.memberId = memberId;
    entry.date = date;
    entry.weight = weight;
    entry.bmi = bmi.calculateBMI(member->height, weight);
    entry.achievement = achievement;

    return storage.addProgressEntry(entry);
}

// Returns all progress entries for a member.
std::vector<ProgressEntry> ProgressService::viewProgressHistory(int memberId) const {
    return storage.getProgressByMemberId(memberId);
}
