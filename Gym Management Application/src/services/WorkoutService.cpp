// Workout service implementation.
#include "WorkoutService.h"

#include "../models/WorkoutItem.h"
#include "../storage/JsonDataStore.h"

// Stores reference to shared data store.
WorkoutService::WorkoutService(JsonDataStore& dataStore) : storage(dataStore) {}

// Generates a baseline template plan for the given member and goal.
WorkoutPlan WorkoutService::generatePlan(const Member& member, const std::string& goal) {
    WorkoutPlan plan;
    plan.planId = storage.generateNextPlanId();
    plan.memberId = member.memberId;
    plan.goal = goal;

    plan.items.push_back(WorkoutItem("Push-ups", 3, 12, "Bodyweight chest exercise"));
    plan.items.push_back(WorkoutItem("Squats", 3, 15, "Bodyweight leg exercise"));
    plan.items.push_back(WorkoutItem("Plank", 3, 1, "Hold 30-60 seconds"));

    return plan;
}

// Saves plan assignment for a member.
bool WorkoutService::assignPlan(int memberId, const WorkoutPlan& plan) {
    return storage.addOrUpdateWorkoutPlan(memberId, plan);
}

// Retrieves currently assigned workout plan.
WorkoutPlan* WorkoutService::viewAssignedPlan(int memberId) {
    return storage.findWorkoutPlanByMemberId(memberId);
}
