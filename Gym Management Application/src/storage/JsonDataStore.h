#pragma once

#include <string>
#include <vector>

#include "../models/Member.h"
#include "../models/ProgressEntry.h"
#include "../models/User.h"
#include "../models/WorkoutPlan.h"

// In-memory data repository with JSON load/save helpers.
class JsonDataStore {
public:
    // User account records.
    std::vector<User> users;
    // Member profile records.
    std::vector<Member> members;
    // Workout plan assignments.
    std::vector<WorkoutPlan> workoutPlans;
    // Member progress history entries.
    std::vector<ProgressEntry> progressEntries;

    // Loads all data files into memory.
    bool loadAll();
    // Persists all in-memory data files.
    bool saveAll() const;

    // Inserts a user record if username is unique.
    bool addUser(const User& user);
    // Inserts a member record if ID is unique.
    bool addMember(const Member& member);
    // Updates existing member record by ID.
    bool updateMember(int memberId, const Member& member);
    // Adds new plan or updates existing plan for member.
    bool addOrUpdateWorkoutPlan(int memberId, const WorkoutPlan& plan);
    // Appends a progress entry.
    bool addProgressEntry(const ProgressEntry& entry);

    // Finds user by username (mutable pointer).
    User* findUserByUsername(const std::string& username);
    // Finds user by username (const pointer).
    const User* findUserByUsername(const std::string& username) const;
    // Finds member by member ID.
    Member* findMemberById(int memberId);
    // Finds member by linked username.
    Member* findMemberByUsername(const std::string& username);
    // Finds workout plan by member ID.
    WorkoutPlan* findWorkoutPlanByMemberId(int memberId);
    // Returns all progress entries for a member.
    std::vector<ProgressEntry> getProgressByMemberId(int memberId) const;

    // Computes the next available user ID.
    int generateNextUserId() const;
    // Computes the next available member ID.
    int generateNextMemberId() const;
    // Computes the next available workout plan ID.
    int generateNextPlanId() const;
};
