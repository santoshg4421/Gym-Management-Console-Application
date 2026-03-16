#pragma once

#include "../models/Member.h"

class JsonDataStore;

// Manages member profile create/read/update operations.
class MemberService {
public:
    // Creates the service with storage dependency.
    explicit MemberService(JsonDataStore& dataStore);

    // Registers a new member profile.
    bool registerMember(const Member& member);
    // Updates an existing member profile.
    bool updateMember(int memberId, const Member& member);
    // Retrieves a member profile by ID.
    Member* viewMember(int memberId);

private:
    // Shared data access layer.
    JsonDataStore& storage;
};
