#pragma once

#include "../services/AuthService.h"
#include "../services/BMIService.h"
#include "../services/MemberService.h"
#include "../services/ProgressService.h"
#include "../services/StaffService.h"
#include "../services/StatisticsService.h"
#include "../services/ValidationService.h"
#include "../services/WorkoutService.h"
#include "../storage/JsonDataStore.h"

// Main UI/controller class that orchestrates menus, flows, and service calls.
class GymApp {
public:
    // Constructs the app and wires service dependencies.
    GymApp();
    // Starts the application lifecycle loop.
    void run();

private:
    // Shared data storage and persistence layer.
    JsonDataStore storage;
    // BMI computation service.
    BMIService bmiService;
    // Authentication and role resolution service.
    AuthService authService;
    // Member profile management service.
    MemberService memberService;
    // Staff account creation service.
    StaffService staffService;
    // Workout generation/assignment service.
    WorkoutService workoutService;
    // Progress tracking service.
    ProgressService progressService;
    // Statistics aggregation service.
    StatisticsService statisticsService;
    // Input/domain validation service.
    ValidationService validationService;
    // Logged-in username for current session.
    std::string currentUsername;
    // Logged-in role for current session.
    std::string currentRole;
    // Linked member ID for current session (if role is member).
    int currentMemberId;

    // Renders top-level menu options.
    void showMainMenu() const;
    // Handles login and role-based routing.
    void handleLogin();
    // Registers a new member and linked member user account.
    void handleRegisterMember();
    // Runs BMI calculator flow.
    void handleCalculateBMI() const;
    // Updates any member profile (admin path).
    void handleUpdateMemberProfile();
    // Views membership details by member ID.
    void handleViewMembershipDetails();
    // Adds progress for selected member (trainer/admin path).
    void handleAddProgressEntry();
    // Views progress history by member ID.
    void handleViewProgressHistory() const;
    // Generates and assigns workout plan to selected member.
    void handleGenerateAndAssignWorkoutPlan();
    // Views assigned plan by selected member ID.
    void handleViewAssignedWorkoutPlan();
    // Displays aggregated gym statistics.
    void handleViewGymStatistics() const;
    // Creates trainer/admin user accounts.
    void handleCreateStaffAccount();

    // Renders member menu options.
    void showMemberSubMenu() const;
    // Renders trainer menu options.
    void showTrainerSubMenu() const;
    // Renders admin menu options.
    void showAdminSubMenu() const;
    // Executes member submenu loop.
    void runMemberSubMenu();
    // Executes trainer submenu loop.
    void runTrainerSubMenu();
    // Executes admin submenu loop.
    void runAdminSubMenu();

    // Views membership details for currently logged-in member.
    void handleViewMyMembershipDetails();
    // Updates profile for currently logged-in member.
    void handleUpdateMyProfile();
    // Adds progress entry for currently logged-in member.
    void handleAddMyProgressEntry();
    // Views progress history for currently logged-in member.
    void handleViewMyProgressHistory();
    // Views assigned workout plan for currently logged-in member.
    void handleViewMyAssignedWorkoutPlan();

    // Clears all session-tracking fields.
    void resetSession();
    // Prompts user to select gender and returns mapped value.
    std::string chooseGender();
    // Prompts user to select membership type and returns mapped value.
    std::string chooseMembershipType();
    // Returns today's date in YYYY-MM-DD.
    std::string getTodayDate() const;
    // Calculates expiry date based on membership type.
    std::string calculateExpiryDate(const std::string& membershipType) const;
    // Resolves current session's member pointer.
    Member* getCurrentMember();
};
