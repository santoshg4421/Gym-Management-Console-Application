// Workout item model implementation.
#include "WorkoutItem.h"

// Default constructor used for temporary objects.
WorkoutItem::WorkoutItem() : sets(0), reps(0) {}

// Value constructor used for plan generation.
WorkoutItem::WorkoutItem(const std::string& name, int setCount, int repCount, const std::string& desc)
    : exerciseName(name), sets(setCount), reps(repCount), description(desc) {}
