#pragma once

#include <string>

// Represents one exercise row inside a workout plan.
class WorkoutItem {
public:
    // Exercise name shown to users.
    std::string exerciseName;
    // Number of sets to perform.
    int sets;
    // Number of reps per set.
    int reps;
    // Additional guidance/instructions.
    std::string description;

    // Creates an empty workout item.
    WorkoutItem();
    // Creates a fully initialized workout item.
    WorkoutItem(const std::string& name, int setCount, int repCount, const std::string& desc);
};
