// Member service implementation.
#include "MemberService.h"

#include "../storage/JsonDataStore.h"

// Stores reference to shared data store.
MemberService::MemberService(JsonDataStore& dataStore) : storage(dataStore) {}

// Adds a new member profile.
bool MemberService::registerMember(const Member& member) {
    return storage.addMember(member);
}

// Updates an existing member profile.
bool MemberService::updateMember(int memberId, const Member& member) {
    return storage.updateMember(memberId, member);
}

// Returns member profile by identifier.
Member* MemberService::viewMember(int memberId) {
    return storage.findMemberById(memberId);
}
