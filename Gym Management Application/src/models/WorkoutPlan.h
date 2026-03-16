#pragma once

#include <string>
#include <vector>

#include "WorkoutItem.h"

// Represents an assigned workout plan for a specific member.
class WorkoutPlan {
public:
    // Unique numeric identifier of the plan.
    int planId;
    // Member profile identifier that owns this plan.
    int memberId;
    // Primary fitness goal of the plan.
    std::string goal;
    // Exercise list included in the plan.
    std::vector<WorkoutItem> items;

    // Creates an empty plan object.
    WorkoutPlan();

    // Builds a readable text version of the plan.
    std::string getPlanText() const;
};
