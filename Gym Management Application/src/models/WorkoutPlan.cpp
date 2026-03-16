// Workout plan model implementation.
#include "WorkoutPlan.h"

#include <sstream>

// Initializes IDs to default values.
WorkoutPlan::WorkoutPlan() : planId(0), memberId(0) {}

// Converts plan data into a multi-line display format.
std::string WorkoutPlan::getPlanText() const {
    std::ostringstream output;
    output << "Goal: " << goal << "\n";

    for (size_t i = 0; i < items.size(); ++i) {
        output << i + 1 << ". " << items[i].exerciseName
               << " (Sets: " << items[i].sets
               << ", Reps: " << items[i].reps << ")"
               << " - " << items[i].description << "\n";
    }

    return output.str();
}
