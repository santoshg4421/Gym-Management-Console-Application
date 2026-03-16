#pragma once

#include <string>

#include "../models/Member.h"
#include "../models/WorkoutPlan.h"

class JsonDataStore;

// Generates, assigns, and retrieves workout plans.
class WorkoutService {
public:
    // Creates the service with storage dependency.
    explicit WorkoutService(JsonDataStore& dataStore);

    // Generates a baseline plan for a member and goal.
    WorkoutPlan generatePlan(const Member& member, const std::string& goal);
    // Assigns or updates a plan for a member.
    bool assignPlan(int memberId, const WorkoutPlan& plan);
    // Returns the currently assigned plan for a member.
    WorkoutPlan* viewAssignedPlan(int memberId);

private:
    // Shared data access layer.
    JsonDataStore& storage;
};
