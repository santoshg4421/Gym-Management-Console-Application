// Main UI controller implementation.
#include "GymApp.h"

#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <ctime>
#include <vector>

#include "../models/WorkoutItem.h"
#include "../models/User.h"

namespace {
// Prints the common two-column table border.
void printTableRule() {
    std::cout << "  +----------------------+-------------------------------------+\n";
}

// Prints a centered boxed title for each screen.
void printHeader(const std::string& title) {
    const int inner = 60;
    int pad = (inner - static_cast<int>(title.size())) / 2;
    if (pad < 0) pad = 0;
    std::string bar = "  +" + std::string(inner, '-') + "+";
    std::cout << "\n" << bar << "\n";
    std::cout << "  |" << std::string(pad, ' ') << title
              << std::string(inner - pad - static_cast<int>(title.size()), ' ') << "|\n";
    std::cout << bar << "\n\n";
}

// Prints a standardized success message.
void printSuccess(const std::string& message) {
    std::cout << "\n  [  OK  ]  " << message << "\n\n";
}

// Prints a standardized error message.
void printError(const std::string& message) {
    std::cout << "\n  [ FAIL ]  " << message << "\n\n";
}

// Reads and validates an integer menu choice.
int readMenuChoice(int minValue, int maxValue) {
    int choice = 0;
    std::cout << "  >> ";

    if (!(std::cin >> choice)) {
        if (std::cin.eof()) {
            return -2;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return -1;
    }

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (choice < minValue || choice > maxValue) {
        return -1;
    }

    return choice;
}

// Reads a full text line (supports spaces).
bool readTextPrompt(const std::string& prompt, std::string& value) {
    std::cout << prompt;
    if (!std::getline(std::cin, value)) {
        return false;
    }
    if (!value.empty() && value.back() == '\r') {
        value.pop_back();
    }
    return true;
}

// Reads an integer prompt safely.
bool readIntPrompt(const std::string& prompt, int& value) {
    std::cout << prompt;
    if (!(std::cin >> value)) {
        if (std::cin.eof()) {
            return false;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}

// Reads a floating-point prompt safely.
bool readDoublePrompt(const std::string& prompt, double& value) {
    std::cout << prompt;
    if (!(std::cin >> value)) {
        if (std::cin.eof()) {
            return false;
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return true;
}
}  // namespace

// Wires all services with the shared storage instance.
GymApp::GymApp()
    : authService(storage),
      memberService(storage),
      staffService(storage),
      workoutService(storage),
      progressService(storage, bmiService),
    statisticsService(storage, bmiService),
    currentMemberId(0) {}

// Runs the full application lifecycle.
void GymApp::run() {
    storage.loadAll();

    if (storage.users.empty()) {
        storage.addUser(User(1, "admin", "admin123", "admin"));
        printSuccess("Default admin created.  Username: admin   Password: admin123");
    }

    bool running = true;
    while (running) {
        showMainMenu();
        int choice = readMenuChoice(1, 3);

        if (choice == -2) {
            running = false;
            continue;
        }

        if (choice == -1) {
            printError("Invalid choice. Please try again.");
            continue;
        }

        switch (choice) {
            case 1:
                handleLogin();
                break;
            case 2:
                handleRegisterMember();
                break;
            case 3:
                running = false;
                break;
        }
    }

    storage.saveAll();
    std::cout << "\n  Goodbye!\n\n";
}

// Displays the top-level application menu.
void GymApp::showMainMenu() const {
    printHeader("GYM MANAGEMENT SYSTEM");
    std::cout << "  [1]  Login\n";
    std::cout << "  [2]  Register Member\n";
    std::cout << "  [3]  Exit\n\n";
}

// Handles login and role-based submenu routing.
void GymApp::handleLogin() {
    printHeader("LOGIN");

    std::string username;
    std::string password;

    if (!readTextPrompt("  Username  : ", username) ||
        !readTextPrompt("  Password  : ", password)) {
        printError("Input cancelled.");
        return;
    }

    if (!authService.login(username, password)) {
        printError("Invalid username or password.");
        return;
    }

    std::string role = authService.getRole(username);
    currentUsername = username;
    currentRole = role;
    currentMemberId = 0;

    if (role == "member") {
        Member* member = storage.findMemberByUsername(username);
        if (member == nullptr) {
            printError("No member profile is linked to this account.");
            resetSession();
            return;
        }
        currentMemberId = member->memberId;
    }

    printSuccess("Welcome!  Role: " + role);

    if (role == "member") {
        runMemberSubMenu();
    } else if (role == "trainer") {
        runTrainerSubMenu();
    } else if (role == "admin") {
        runAdminSubMenu();
    } else {
        printError("Unknown role. Contact admin.");
    }
}

// Handles registration of a member profile and linked user account.
void GymApp::handleRegisterMember() {
    printHeader("REGISTER MEMBER");

    Member member;
    std::string username;
    std::string password;

    if (!readTextPrompt("  Username  : ", username)) {
        printError("Input cancelled.");
        return;
    }
    if (!validationService.validateText(username)) {
        printError("Invalid username.");
        return;
    }

    if (storage.findUserByUsername(username) != nullptr) {
        printError("Username already taken.");
        return;
    }

    if (!readTextPrompt("  Password  : ", password)) {
        printError("Input cancelled.");
        return;
    }
    if (!validationService.validateText(password)) {
        printError("Invalid password.");
        return;
    }

    if (!readTextPrompt("  Name      : ", member.name) ||
        !readIntPrompt("  Age       : ", member.age)) {
        printError("Input cancelled.");
        return;
    }
    member.gender = chooseGender();
    if (member.gender.empty()) {
        printError("Invalid gender choice.");
        return;
    }
    if (!readDoublePrompt("  Height(cm): ", member.height) ||
        !readDoublePrompt("  Weight(kg): ", member.weight)) {
        printError("Input cancelled.");
        return;
    }
    member.membershipType = chooseMembershipType();
    if (member.membershipType.empty()) {
        printError("Invalid membership type choice.");
        return;
    }
    member.startDate = getTodayDate();
    member.expiryDate = calculateExpiryDate(member.membershipType);

    if (!validationService.validateAge(member.age) ||
        !validationService.validateHeight(member.height) ||
        !validationService.validateWeight(member.weight) ||
        !validationService.validateText(member.name) ||
        !validationService.validateText(member.gender) ||
        !validationService.validateText(member.membershipType)) {
        printError("Invalid member data. Check ranges and required fields.");
        return;
    }

    member.memberId = storage.generateNextMemberId();
    member.username = username;

    if (!memberService.registerMember(member)) {
        printError("Could not register member profile.");
        return;
    }

    User user(storage.generateNextUserId(), username, password, "member");
    if (!storage.addUser(user)) {
        printError("Member created, but user account creation failed.");
        return;
    }

    storage.saveAll();

    double bmi = bmiService.calculateBMI(member.height, member.weight);
    std::string category = bmiService.getBMICategory(bmi);

    printSuccess("Member registered successfully.");
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Field" << " | " << std::setw(35) << "Value" << " |\n";
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Member ID" << " | " << std::setw(35) << member.memberId << " |\n";
    std::ostringstream bmiText;
    bmiText << std::fixed << std::setprecision(2) << bmi << "  (" << category << ")";
    std::cout << "  | " << std::left << std::setw(20) << "Initial BMI" << " | " << std::setw(35) << bmiText.str() << " |\n";
    printTableRule();
}

// Handles BMI calculator workflow.
void GymApp::handleCalculateBMI() const {
    printHeader("BMI CALCULATOR");

    double height = 0.0;
    double weight = 0.0;

    if (!readDoublePrompt("  Height(cm): ", height) ||
        !readDoublePrompt("  Weight(kg): ", weight)) {
        printError("Input cancelled.");
        return;
    }

    if (!validationService.validateHeight(height) || !validationService.validateWeight(weight)) {
        printError("Invalid values. Height must be 50-250 cm and weight must be 20-300 kg.");
        return;
    }

    double bmi = bmiService.calculateBMI(height, weight);
    std::string category = bmiService.getBMICategory(bmi);

    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Metric" << " | " << std::setw(35) << "Value" << " |\n";
    printTableRule();
    std::ostringstream bmiValue;
    bmiValue << std::fixed << std::setprecision(2) << bmi;
    std::cout << "  | " << std::left << std::setw(20) << "BMI" << " | " << std::setw(35) << bmiValue.str() << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Category" << " | " << std::setw(35) << category << " |\n";
    printTableRule();
}

// Handles admin-driven member profile updates.
void GymApp::handleUpdateMemberProfile() {
    printHeader("UPDATE MEMBER PROFILE");

    int memberId = 0;
    if (!readIntPrompt("  Member ID : ", memberId)) {
        printError("Input cancelled.");
        return;
    }

    Member* existing = memberService.viewMember(memberId);
    if (existing == nullptr) {
        printError("Member not found.");
        return;
    }

    Member updated = *existing;

    if (!readTextPrompt("  Name      : ", updated.name) ||
        !readIntPrompt("  Age       : ", updated.age)) {
        printError("Input cancelled.");
        return;
    }
    updated.gender = chooseGender();
    if (updated.gender.empty()) {
        printError("Invalid gender choice.");
        return;
    }
    if (!readDoublePrompt("  Height(cm): ", updated.height) ||
        !readDoublePrompt("  Weight(kg): ", updated.weight)) {
        printError("Input cancelled.");
        return;
    }
    updated.membershipType = chooseMembershipType();
    if (updated.membershipType.empty()) {
        printError("Invalid membership type choice.");
        return;
    }
    updated.startDate = getTodayDate();
    updated.expiryDate = calculateExpiryDate(updated.membershipType);

    if (!validationService.validateAge(updated.age) ||
        !validationService.validateHeight(updated.height) ||
        !validationService.validateWeight(updated.weight) ||
        !validationService.validateText(updated.name) ||
        !validationService.validateText(updated.gender) ||
        !validationService.validateText(updated.membershipType)) {
        printError("Invalid member data. Update failed.");
        return;
    }

    if (memberService.updateMember(memberId, updated)) {
        storage.saveAll();
        printSuccess("Member profile updated successfully.");
    } else {
        printError("Could not update member profile.");
    }
}

// Displays membership details by member ID.
void GymApp::handleViewMembershipDetails() {
    printHeader("VIEW MEMBERSHIP DETAILS");

    int memberId = 0;
    if (!readIntPrompt("  Member ID : ", memberId)) {
        printError("Input cancelled.");
        return;
    }

    Member* member = storage.findMemberById(memberId);
    if (member == nullptr) {
        printError("Member not found.");
        return;
    }

    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Field" << " | " << std::setw(35) << "Value" << " |\n";
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Member ID" << " | " << std::setw(35) << member->memberId << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Name" << " | " << std::setw(35) << member->name << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Membership" << " | " << std::setw(35) << member->getMembershipDetails() << " |\n";
    printTableRule();
}

// Adds a progress entry for a selected member.
void GymApp::handleAddProgressEntry() {
    printHeader("ADD PROGRESS ENTRY");

    int memberId = 0;
    std::string date;
    double weight = 0.0;
    std::string achievement;

    if (!readIntPrompt("  Member ID : ", memberId) ||
        !readTextPrompt("  Date      : ", date) ||
        !readDoublePrompt("  Weight(kg): ", weight) ||
        !readTextPrompt("  Note      : ", achievement)) {
        printError("Input cancelled.");
        return;
    }

    if (!validationService.validateWeight(weight) ||
        !validationService.validateText(date) ||
        !validationService.validateText(achievement)) {
        printError("Invalid progress data.");
        return;
    }

    if (progressService.addProgress(memberId, date, weight, achievement)) {
        storage.saveAll();
        printSuccess("Progress entry added successfully.");
    } else {
        printError("Could not add progress entry. Check member ID.");
    }
}

// Displays progress history table by member ID.
void GymApp::handleViewProgressHistory() const {
    printHeader("VIEW PROGRESS HISTORY");

    int memberId = 0;
    if (!readIntPrompt("  Member ID : ", memberId)) {
        printError("Input cancelled.");
        return;
    }

    std::vector<ProgressEntry> history = progressService.viewProgressHistory(memberId);
    if (history.empty()) {
        printError("No progress history found for this member.");
        return;
    }

    std::cout << "  +------------+------------+--------+--------------------------------+\n";
    std::cout << "  | " << std::left << std::setw(10) << "Date" << " | " << std::setw(10) << "Weight(kg)" << " | " << std::setw(6) << "BMI" << " | " << std::setw(30) << "Achievement" << " |\n";
    std::cout << "  +------------+------------+--------+--------------------------------+\n";
    for (const ProgressEntry& entry : history) {
        std::cout << "  | " << std::left << std::setw(10) << entry.date
                  << " | " << std::left << std::setw(10) << std::fixed << std::setprecision(2) << entry.weight
                  << " | " << std::left << std::setw(6) << std::fixed << std::setprecision(2) << entry.bmi
                  << " | " << std::left << std::setw(30) << entry.achievement << " |\n";
    }
    std::cout << "  +------------+------------+--------+--------------------------------+\n";
}

// Generates and assigns a workout plan for a selected member.
void GymApp::handleGenerateAndAssignWorkoutPlan() {
    printHeader("GENERATE & ASSIGN WORKOUT PLAN");

    int memberId = 0;
    std::string goal;

        if (!readIntPrompt("  Member ID : ", memberId)) {
        printError("Input cancelled.");
        return;
    }

    Member* member = memberService.viewMember(memberId);
    if (member == nullptr) {
        printError("Member not found.");
        return;
    }

        if (!readTextPrompt("  Goal      : ", goal)) {
        printError("Input cancelled.");
        return;
    }

    if (!validationService.validateText(goal)) {
        printError("Invalid goal.");
        return;
    }

    WorkoutPlan plan = workoutService.generatePlan(*member, goal);
    if (workoutService.assignPlan(memberId, plan)) {
        storage.saveAll();
        printSuccess("Workout plan assigned successfully.");
            std::cout << "\n  Goal: " << plan.goal << "\n\n";
            std::cout << "  +------------------------+---------------+------------------------+\n";
            std::cout << "  | " << std::left << std::setw(22) << "Exercise" << " | " << std::setw(13) << "Sets x Reps" << " | " << std::setw(22) << "Description" << " |\n";
            std::cout << "  +------------------------+---------------+------------------------+\n";
            for (const WorkoutItem& item : plan.items) {
                std::ostringstream sr;
                sr << item.sets << " x " << item.reps;
                std::cout << "  | " << std::left << std::setw(22) << item.exerciseName
                          << " | " << std::left << std::setw(13) << sr.str()
                          << " | " << std::left << std::setw(22) << item.description << " |\n";
            }
            std::cout << "  +------------------------+---------------+------------------------+\n";
    } else {
        printError("Could not assign workout plan.");
    }
}

// Displays assigned workout plan by member ID.
void GymApp::handleViewAssignedWorkoutPlan() {
    printHeader("VIEW ASSIGNED WORKOUT PLAN");

    int memberId = 0;
    if (!readIntPrompt("  Member ID : ", memberId)) {
        printError("Input cancelled.");
        return;
    }

    WorkoutPlan* plan = workoutService.viewAssignedPlan(memberId);
    if (plan == nullptr) {
        printError("No workout plan assigned for this member.");
        return;
    }

    std::cout << "\n  Goal: " << plan->goal << "\n\n";
    std::cout << "  +------------------------+---------------+------------------------+\n";
    std::cout << "  | " << std::left << std::setw(22) << "Exercise" << " | " << std::setw(13) << "Sets x Reps" << " | " << std::setw(22) << "Description" << " |\n";
    std::cout << "  +------------------------+---------------+------------------------+\n";
    for (const WorkoutItem& item : plan->items) {
        std::ostringstream setsReps;
        setsReps << item.sets << " x " << item.reps;
        std::cout << "  | " << std::left << std::setw(22) << item.exerciseName
                  << " | " << std::left << std::setw(13) << setsReps.str()
                  << " | " << std::left << std::setw(22) << item.description << " |\n";
    }
    std::cout << "  +------------------------+---------------+------------------------+\n";
}

// Displays aggregate gym statistics.
void GymApp::handleViewGymStatistics() const {
    printHeader("GYM STATISTICS");

    int totalMembers = statisticsService.getTotalMembers();
    double averageBmi = statisticsService.getAverageBMI();
    std::map<std::string, int> distribution = statisticsService.getBMIDistribution();

    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Metric" << " | " << std::setw(35) << "Value" << " |\n";
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Total Members" << " | " << std::setw(35) << totalMembers << " |\n";
    std::ostringstream avgBmiText;
    avgBmiText << std::fixed << std::setprecision(2) << averageBmi;
    std::cout << "  | " << std::left << std::setw(20) << "Average BMI" << " | " << std::setw(35) << avgBmiText.str() << " |\n";
    printTableRule();

    std::cout << "\n  BMI Category Breakdown:\n";
    std::cout << "  +------------------------------+--------+\n";
    std::cout << "  | " << std::left << std::setw(28) << "Category" << " | " << std::setw(6) << "Count" << " |\n";
    std::cout << "  +------------------------------+--------+\n";
    if (distribution.empty()) {
        std::cout << "  | " << std::left << std::setw(28) << "No data available" << " | " << std::setw(6) << "-" << " |\n";
    } else {
        for (const auto& kv : distribution) {
            std::cout << "  | " << std::left << std::setw(28) << kv.first << " | " << std::setw(6) << kv.second << " |\n";
        }
    }
    std::cout << "  +------------------------------+--------+\n";
}

// Creates trainer or admin staff accounts.
void GymApp::handleCreateStaffAccount() {
    printHeader("CREATE STAFF ACCOUNT (ADMIN)");

    std::cout << "  [1]  Trainer Account\n";
    std::cout << "  [2]  Admin Account\n\n";
    int choice = readMenuChoice(1, 2);

    if (choice == -1 || choice == -2) {
        printError("Invalid choice.");
        return;
    }

    std::string username;
    std::string password;

    if (!readTextPrompt("  Username  : ", username) ||
        !readTextPrompt("  Password  : ", password)) {
        printError("Input cancelled.");
        return;
    }

    if (!validationService.validateText(username) || !validationService.validateText(password)) {
        printError("Invalid username/password.");
        return;
    }

    if (storage.findUserByUsername(username) != nullptr) {
        printError("Username already exists.");
        return;
    }

    bool created = (choice == 1)
                       ? staffService.createTrainerAccount(username, password)
                       : staffService.createAdminAccount(username, password);

    if (created) {
        storage.saveAll();
        printSuccess("Staff account created successfully.");
    } else {
        printError("Could not create staff account.");
    }
}

void GymApp::showMemberSubMenu() const {
    printHeader("MEMBER MENU");
    std::cout << "  [1]  View Membership Details\n";
    std::cout << "  [2]  Update My Profile\n";
    std::cout << "  [3]  View Assigned Workout Plan\n";
    std::cout << "  [4]  Add Progress Entry\n";
    std::cout << "  [5]  View Progress History\n";
    std::cout << "  [6]  BMI Calculator\n";
    std::cout << "  [7]  Logout\n\n";
}

void GymApp::showTrainerSubMenu() const {
    printHeader("TRAINER MENU");
    std::cout << "  [1]  Generate & Assign Workout Plan\n";
    std::cout << "  [2]  View Assigned Workout Plan\n";
    std::cout << "  [3]  Add Progress Entry\n";
    std::cout << "  [4]  View Progress History\n";
    std::cout << "  [5]  Logout\n\n";
}

void GymApp::showAdminSubMenu() const {
    printHeader("ADMIN MENU");
    std::cout << "  [1]  Register Member\n";
    std::cout << "  [2]  Create Staff Account\n";
    std::cout << "  [3]  View Gym Statistics\n";
    std::cout << "  [4]  View Membership Details\n";
    std::cout << "  [5]  Update Member Profile\n";
    std::cout << "  [6]  Logout\n\n";
}

void GymApp::runMemberSubMenu() {
    bool active = true;
    while (active) {
        showMemberSubMenu();
        int choice = readMenuChoice(1, 7);

        if (choice == -2) {
            resetSession();
            active = false;
            continue;
        }

        if (choice == -1) {
            printError("Invalid choice.");
            continue;
        }

        switch (choice) {
            case 1: handleViewMyMembershipDetails(); break;
            case 2: handleUpdateMyProfile(); break;
            case 3: handleViewMyAssignedWorkoutPlan(); break;
            case 4: handleAddMyProgressEntry(); break;
            case 5: handleViewMyProgressHistory(); break;
            case 6: handleCalculateBMI(); break;
            case 7: resetSession(); active = false; break;
        }
    }
}

void GymApp::runTrainerSubMenu() {
    bool active = true;
    while (active) {
        showTrainerSubMenu();
        int choice = readMenuChoice(1, 5);

        if (choice == -2) {
            resetSession();
            active = false;
            continue;
        }

        if (choice == -1) {
            printError("Invalid choice.");
            continue;
        }

        switch (choice) {
            case 1: handleGenerateAndAssignWorkoutPlan(); break;
            case 2: handleViewAssignedWorkoutPlan(); break;
            case 3: handleAddProgressEntry(); break;
            case 4: handleViewProgressHistory(); break;
            case 5: resetSession(); active = false; break;
        }
    }
}

void GymApp::runAdminSubMenu() {
    bool active = true;
    while (active) {
        showAdminSubMenu();
        int choice = readMenuChoice(1, 6);

        if (choice == -2) {
            resetSession();
            active = false;
            continue;
        }

        if (choice == -1) {
            printError("Invalid choice.");
            continue;
        }

        switch (choice) {
            case 1: handleRegisterMember(); break;
            case 2: handleCreateStaffAccount(); break;
            case 3: handleViewGymStatistics(); break;
            case 4: handleViewMembershipDetails(); break;
            case 5: handleUpdateMemberProfile(); break;
            case 6: resetSession(); active = false; break;
        }
    }
}

void GymApp::handleViewMyMembershipDetails() {
    Member* member = getCurrentMember();
    if (member == nullptr) {
        printError("No member is linked to this session.");
        return;
    }

    printHeader("MY MEMBERSHIP DETAILS");
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Field" << " | " << std::setw(35) << "Value" << " |\n";
    printTableRule();
    std::cout << "  | " << std::left << std::setw(20) << "Member ID" << " | " << std::setw(35) << member->memberId << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Username" << " | " << std::setw(35) << member->username << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Name" << " | " << std::setw(35) << member->name << " |\n";
    std::cout << "  | " << std::left << std::setw(20) << "Membership" << " | " << std::setw(35) << member->getMembershipDetails() << " |\n";
    printTableRule();
}

void GymApp::handleUpdateMyProfile() {
    Member* member = getCurrentMember();
    if (member == nullptr) {
        printError("No member is linked to this session.");
        return;
    }

    printHeader("UPDATE MY PROFILE");

    Member updated = *member;
    if (!readTextPrompt("  Name      : ", updated.name) ||
        !readIntPrompt("  Age       : ", updated.age)) {
        printError("Input cancelled.");
        return;
    }
    updated.gender = chooseGender();
    if (updated.gender.empty()) {
        printError("Invalid gender choice.");
        return;
    }
    if (!readDoublePrompt("  Height(cm): ", updated.height) ||
        !readDoublePrompt("  Weight(kg): ", updated.weight)) {
        printError("Input cancelled.");
        return;
    }
    updated.membershipType = chooseMembershipType();
    if (updated.membershipType.empty()) {
        printError("Invalid membership type choice.");
        return;
    }
    updated.startDate = getTodayDate();
    updated.expiryDate = calculateExpiryDate(updated.membershipType);

    if (!validationService.validateAge(updated.age) ||
        !validationService.validateHeight(updated.height) ||
        !validationService.validateWeight(updated.weight) ||
        !validationService.validateText(updated.name)) {
        printError("Invalid profile data.");
        return;
    }

    if (memberService.updateMember(updated.memberId, updated)) {
        storage.saveAll();
        printSuccess("Your profile was updated successfully.");
    } else {
        printError("Could not update your profile.");
    }
}

void GymApp::handleAddMyProgressEntry() {
    Member* member = getCurrentMember();
    if (member == nullptr) {
        printError("No member is linked to this session.");
        return;
    }

    printHeader("ADD MY PROGRESS ENTRY");

    std::string date;
    double weight = 0.0;
    std::string achievement;

    if (!readTextPrompt("  Date      : ", date) ||
        !readDoublePrompt("  Weight(kg): ", weight) ||
        !readTextPrompt("  Note      : ", achievement)) {
        printError("Input cancelled.");
        return;
    }

    if (!validationService.validateWeight(weight) || !validationService.validateText(date) || !validationService.validateText(achievement)) {
        printError("Invalid progress data.");
        return;
    }

    if (progressService.addProgress(member->memberId, date, weight, achievement)) {
        storage.saveAll();
        printSuccess("Progress entry added successfully.");
    } else {
        printError("Could not add progress entry.");
    }
}

void GymApp::handleViewMyProgressHistory() {
    Member* member = getCurrentMember();
    if (member == nullptr) {
        printError("No member is linked to this session.");
        return;
    }

    printHeader("MY PROGRESS HISTORY");
    std::vector<ProgressEntry> history = progressService.viewProgressHistory(member->memberId);
    if (history.empty()) {
        printError("No progress history found.");
        return;
    }

    std::cout << "  +------------+------------+--------+--------------------------------+\n";
    std::cout << "  | " << std::left << std::setw(10) << "Date" << " | " << std::setw(10) << "Weight(kg)" << " | " << std::setw(6) << "BMI" << " | " << std::setw(30) << "Achievement" << " |\n";
    std::cout << "  +------------+------------+--------+--------------------------------+\n";
    for (const ProgressEntry& entry : history) {
        std::cout << "  | " << std::left << std::setw(10) << entry.date
                  << " | " << std::left << std::setw(10) << std::fixed << std::setprecision(2) << entry.weight
                  << " | " << std::left << std::setw(6) << std::fixed << std::setprecision(2) << entry.bmi
                  << " | " << std::left << std::setw(30) << entry.achievement << " |\n";
    }
    std::cout << "  +------------+------------+--------+--------------------------------+\n";
}

void GymApp::handleViewMyAssignedWorkoutPlan() {
    Member* member = getCurrentMember();
    if (member == nullptr) {
        printError("No member is linked to this session.");
        return;
    }

    printHeader("MY ASSIGNED WORKOUT PLAN");
    WorkoutPlan* plan = workoutService.viewAssignedPlan(member->memberId);
    if (plan == nullptr) {
        printError("No workout plan assigned.");
        return;
    }

    std::cout << "\n  Goal: " << plan->goal << "\n\n";
    std::cout << "  +------------------------+---------------+------------------------+\n";
    std::cout << "  | " << std::left << std::setw(22) << "Exercise" << " | " << std::setw(13) << "Sets x Reps" << " | " << std::setw(22) << "Description" << " |\n";
    std::cout << "  +------------------------+---------------+------------------------+\n";
    for (const WorkoutItem& item : plan->items) {
        std::ostringstream setsReps;
        setsReps << item.sets << " x " << item.reps;
        std::cout << "  | " << std::left << std::setw(22) << item.exerciseName
                  << " | " << std::left << std::setw(13) << setsReps.str()
                  << " | " << std::left << std::setw(22) << item.description << " |\n";
    }
    std::cout << "  +------------------------+---------------+------------------------+\n";
}

void GymApp::resetSession() {
    currentUsername.clear();
    currentRole.clear();
    currentMemberId = 0;
}

std::string GymApp::chooseGender() {
    std::cout << "\n  Gender:\n";
    std::cout << "    [1]  Male\n";
    std::cout << "    [2]  Female\n";
    std::cout << "    [3]  Other\n";
    int choice = readMenuChoice(1, 3);
    if (choice == 1) return "male";
    if (choice == 2) return "female";
    if (choice == 3) return "other";
    return "";
}

std::string GymApp::chooseMembershipType() {
    std::cout << "\n  Membership Type:\n";
    std::cout << "    [1]  Monthly\n";
    std::cout << "    [2]  Quarterly\n";
    std::cout << "    [3]  Annual\n";
    int choice = readMenuChoice(1, 3);
    if (choice == 1) return "monthly";
    if (choice == 2) return "quarterly";
    if (choice == 3) return "annual";
    return "";
}

std::string GymApp::getTodayDate() const {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream output;
    output << (local->tm_year + 1900) << '-'
           << std::setw(2) << std::setfill('0') << (local->tm_mon + 1) << '-'
           << std::setw(2) << std::setfill('0') << local->tm_mday;
    return output.str();
}

std::string GymApp::calculateExpiryDate(const std::string& membershipType) const {
    std::time_t now = std::time(nullptr);
    std::tm expiry = *std::localtime(&now);

    if (membershipType == "monthly") {
        expiry.tm_mon += 1;
    } else if (membershipType == "quarterly") {
        expiry.tm_mon += 3;
    } else {
        expiry.tm_mon += 12;
    }

    std::mktime(&expiry);

    std::ostringstream output;
    output << (expiry.tm_year + 1900) << '-'
           << std::setw(2) << std::setfill('0') << (expiry.tm_mon + 1) << '-'
           << std::setw(2) << std::setfill('0') << expiry.tm_mday;
    return output.str();
}

Member* GymApp::getCurrentMember() {
    if (currentMemberId != 0) {
        return storage.findMemberById(currentMemberId);
    }
    if (!currentUsername.empty()) {
        return storage.findMemberByUsername(currentUsername);
    }
    return nullptr;
}
