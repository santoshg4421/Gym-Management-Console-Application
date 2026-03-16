// JSON-backed data store implementation.
#include "JsonDataStore.h"

#include <direct.h>
#include <fstream>
#include <sstream>

namespace {
// Reads entire text file content into a string.
std::string readTextFile(const std::string& path) {
    std::ifstream input(path);
    if (!input.is_open()) {
        return "";
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

// Writes full string content to a text file.
bool writeTextFile(const std::string& path, const std::string& content) {
    std::ofstream output(path, std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output << content;
    return true;
}

// Extracts each JSON object block from a top-level JSON array text.
std::vector<std::string> extractObjects(const std::string& jsonArrayText) {
    std::vector<std::string> objects;
    int depth = 0;
    std::size_t objectStart = std::string::npos;

    for (std::size_t i = 0; i < jsonArrayText.size(); ++i) {
        if (jsonArrayText[i] == '{') {
            if (depth == 0) {
                objectStart = i;
            }
            depth++;
        } else if (jsonArrayText[i] == '}') {
            depth--;
            if (depth == 0 && objectStart != std::string::npos) {
                objects.push_back(jsonArrayText.substr(objectStart, i - objectStart + 1));
                objectStart = std::string::npos;
            }
        }
    }

    return objects;
}

// Escapes backslash and quotes for JSON string fields.
std::string escapeJson(const std::string& text) {
    std::string escaped;
    escaped.reserve(text.size());

    for (char character : text) {
        if (character == '\\') {
            escaped += "\\\\";
        } else if (character == '"') {
            escaped += "\\\"";
        } else {
            escaped += character;
        }
    }

    return escaped;
}

// Reads a string field value from a JSON object text.
std::string getStringField(const std::string& objectText, const std::string& key) {
    const std::string token = "\"" + key + "\"";
    std::size_t keyPos = objectText.find(token);
    if (keyPos == std::string::npos) {
        return "";
    }

    std::size_t colonPos = objectText.find(':', keyPos);
    std::size_t firstQuote = objectText.find('"', colonPos + 1);
    std::size_t secondQuote = objectText.find('"', firstQuote + 1);
    if (colonPos == std::string::npos || firstQuote == std::string::npos || secondQuote == std::string::npos) {
        return "";
    }

    return objectText.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

// Reads an integer field value from a JSON object text.
int getIntField(const std::string& objectText, const std::string& key) {
    const std::string token = "\"" + key + "\"";
    std::size_t keyPos = objectText.find(token);
    if (keyPos == std::string::npos) {
        return 0;
    }

    std::size_t colonPos = objectText.find(':', keyPos);
    std::size_t valueStart = objectText.find_first_of("-0123456789", colonPos + 1);
    std::size_t valueEnd = objectText.find_first_not_of("-0123456789", valueStart);
    if (colonPos == std::string::npos || valueStart == std::string::npos) {
        return 0;
    }

    return std::stoi(objectText.substr(valueStart, valueEnd - valueStart));
}

// Reads a floating-point field value from a JSON object text.
double getDoubleField(const std::string& objectText, const std::string& key) {
    const std::string token = "\"" + key + "\"";
    std::size_t keyPos = objectText.find(token);
    if (keyPos == std::string::npos) {
        return 0.0;
    }

    std::size_t colonPos = objectText.find(':', keyPos);
    std::size_t valueStart = objectText.find_first_of("-0123456789.", colonPos + 1);
    std::size_t valueEnd = objectText.find_first_not_of("-0123456789.", valueStart);
    if (colonPos == std::string::npos || valueStart == std::string::npos) {
        return 0.0;
    }

    return std::stod(objectText.substr(valueStart, valueEnd - valueStart));
}

// Serializes user records to JSON array text.
std::string buildUsersJson(const std::vector<User>& users) {
    std::ostringstream json;
    json << "[\n";

    for (std::size_t i = 0; i < users.size(); ++i) {
        const User& user = users[i];
        json << "  {"
             << "\"userId\":" << user.userId << ","
             << "\"username\":\"" << escapeJson(user.username) << "\","
             << "\"password\":\"" << escapeJson(user.password) << "\","
             << "\"role\":\"" << escapeJson(user.role) << "\""
             << "}";
        if (i + 1 < users.size()) {
            json << ",";
        }
        json << "\n";
    }

    json << "]\n";
    return json.str();
}

// Serializes member records to JSON array text.
std::string buildMembersJson(const std::vector<Member>& members) {
    std::ostringstream json;
    json << "[\n";

    for (std::size_t i = 0; i < members.size(); ++i) {
        const Member& member = members[i];
        json << "  {"
             << "\"memberId\":" << member.memberId << ","
               << "\"username\":\"" << escapeJson(member.username) << "\","
             << "\"name\":\"" << escapeJson(member.name) << "\","
             << "\"age\":" << member.age << ","
             << "\"gender\":\"" << escapeJson(member.gender) << "\","
             << "\"height\":" << member.height << ","
             << "\"weight\":" << member.weight << ","
             << "\"membershipType\":\"" << escapeJson(member.membershipType) << "\","
             << "\"startDate\":\"" << escapeJson(member.startDate) << "\","
             << "\"expiryDate\":\"" << escapeJson(member.expiryDate) << "\""
             << "}";
        if (i + 1 < members.size()) {
            json << ",";
        }
        json << "\n";
    }

    json << "]\n";
    return json.str();
}

// Serializes workout plan records to JSON array text.
std::string buildWorkoutPlansJson(const std::vector<WorkoutPlan>& plans) {
    std::ostringstream json;
    json << "[\n";

    for (std::size_t i = 0; i < plans.size(); ++i) {
        const WorkoutPlan& plan = plans[i];
        json << "  {"
             << "\"planId\":" << plan.planId << ","
             << "\"memberId\":" << plan.memberId << ","
             << "\"goal\":\"" << escapeJson(plan.goal) << "\""
             << "}";
        if (i + 1 < plans.size()) {
            json << ",";
        }
        json << "\n";
    }

    json << "]\n";
    return json.str();
}

// Serializes progress entry records to JSON array text.
std::string buildProgressJson(const std::vector<ProgressEntry>& entries) {
    std::ostringstream json;
    json << "[\n";

    for (std::size_t i = 0; i < entries.size(); ++i) {
        const ProgressEntry& entry = entries[i];
        json << "  {"
             << "\"memberId\":" << entry.memberId << ","
             << "\"date\":\"" << escapeJson(entry.date) << "\","
             << "\"weight\":" << entry.weight << ","
             << "\"bmi\":" << entry.bmi << ","
             << "\"achievement\":\"" << escapeJson(entry.achievement) << "\""
             << "}";
        if (i + 1 < entries.size()) {
            json << ",";
        }
        json << "\n";
    }

    json << "]\n";
    return json.str();
}
}  // namespace

// Loads all JSON data files into in-memory collections.
bool JsonDataStore::loadAll() {
    _mkdir("data");

    users.clear();
    members.clear();
    workoutPlans.clear();
    progressEntries.clear();

    const std::string usersText = readTextFile("data/users.json");
    if (!usersText.empty()) {
        for (const std::string& objectText : extractObjects(usersText)) {
            User user;
            user.userId = getIntField(objectText, "userId");
            user.username = getStringField(objectText, "username");
            user.password = getStringField(objectText, "password");
            user.role = getStringField(objectText, "role");
            users.push_back(user);
        }
    }

    const std::string membersText = readTextFile("data/members.json");
    if (!membersText.empty()) {
        for (const std::string& objectText : extractObjects(membersText)) {
            Member member;
            member.memberId = getIntField(objectText, "memberId");
            member.username = getStringField(objectText, "username");
            member.name = getStringField(objectText, "name");
            member.age = getIntField(objectText, "age");
            member.gender = getStringField(objectText, "gender");
            member.height = getDoubleField(objectText, "height");
            member.weight = getDoubleField(objectText, "weight");
            member.membershipType = getStringField(objectText, "membershipType");
            member.startDate = getStringField(objectText, "startDate");
            member.expiryDate = getStringField(objectText, "expiryDate");
            members.push_back(member);
        }
    }

    const std::string plansText = readTextFile("data/workout_plans.json");
    if (!plansText.empty()) {
        for (const std::string& objectText : extractObjects(plansText)) {
            WorkoutPlan plan;
            plan.planId = getIntField(objectText, "planId");
            plan.memberId = getIntField(objectText, "memberId");
            plan.goal = getStringField(objectText, "goal");
            workoutPlans.push_back(plan);
        }
    }

    const std::string progressText = readTextFile("data/progress_entries.json");
    if (!progressText.empty()) {
        for (const std::string& objectText : extractObjects(progressText)) {
            ProgressEntry entry;
            entry.memberId = getIntField(objectText, "memberId");
            entry.date = getStringField(objectText, "date");
            entry.weight = getDoubleField(objectText, "weight");
            entry.bmi = getDoubleField(objectText, "bmi");
            entry.achievement = getStringField(objectText, "achievement");
            progressEntries.push_back(entry);
        }
    }

    return true;
}

// Saves all in-memory collections to JSON data files.
bool JsonDataStore::saveAll() const {
    _mkdir("data");

    bool usersSaved = writeTextFile("data/users.json", buildUsersJson(users));
    bool membersSaved = writeTextFile("data/members.json", buildMembersJson(members));
    bool plansSaved = writeTextFile("data/workout_plans.json", buildWorkoutPlansJson(workoutPlans));
    bool progressSaved = writeTextFile("data/progress_entries.json", buildProgressJson(progressEntries));

    return usersSaved && membersSaved && plansSaved && progressSaved;
}

// Adds a new user if username is not duplicated.
bool JsonDataStore::addUser(const User& user) {
    if (findUserByUsername(user.username) != nullptr) {
        return false;
    }
    users.push_back(user);
    return true;
}

// Adds a new member if member ID is not duplicated.
bool JsonDataStore::addMember(const Member& member) {
    if (findMemberById(member.memberId) != nullptr) {
        return false;
    }
    members.push_back(member);
    return true;
}

// Updates an existing member while preserving the target member ID.
bool JsonDataStore::updateMember(int memberId, const Member& member) {
    Member* existing = findMemberById(memberId);
    if (existing == nullptr) {
        return false;
    }

    *existing = member;
    existing->memberId = memberId;
    return true;
}

// Updates existing plan or inserts a new plan for a member.
bool JsonDataStore::addOrUpdateWorkoutPlan(int memberId, const WorkoutPlan& plan) {
    WorkoutPlan* existing = findWorkoutPlanByMemberId(memberId);
    if (existing != nullptr) {
        *existing = plan;
        existing->memberId = memberId;
        return true;
    }

    workoutPlans.push_back(plan);
    workoutPlans.back().memberId = memberId;
    return true;
}

// Appends a progress entry.
bool JsonDataStore::addProgressEntry(const ProgressEntry& entry) {
    progressEntries.push_back(entry);
    return true;
}

// Mutable user lookup by username.
User* JsonDataStore::findUserByUsername(const std::string& username) {
    for (User& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

// Const user lookup by username.
const User* JsonDataStore::findUserByUsername(const std::string& username) const {
    for (const User& user : users) {
        if (user.username == username) {
            return &user;
        }
    }
    return nullptr;
}

// Mutable member lookup by member ID.
Member* JsonDataStore::findMemberById(int memberId) {
    for (Member& member : members) {
        if (member.memberId == memberId) {
            return &member;
        }
    }
    return nullptr;
}

// Mutable member lookup by linked username.
Member* JsonDataStore::findMemberByUsername(const std::string& username) {
    for (Member& member : members) {
        if (member.username == username) {
            return &member;
        }
    }
    return nullptr;
}

// Mutable workout plan lookup by member ID.
WorkoutPlan* JsonDataStore::findWorkoutPlanByMemberId(int memberId) {
    for (WorkoutPlan& plan : workoutPlans) {
        if (plan.memberId == memberId) {
            return &plan;
        }
    }
    return nullptr;
}

// Returns all progress entries for a given member ID.
std::vector<ProgressEntry> JsonDataStore::getProgressByMemberId(int memberId) const {
    std::vector<ProgressEntry> result;
    for (const ProgressEntry& entry : progressEntries) {
        if (entry.memberId == memberId) {
            result.push_back(entry);
        }
    }
    return result;
}

// Returns next available user ID.
int JsonDataStore::generateNextUserId() const {
    int maxId = 0;
    for (const User& user : users) {
        if (user.userId > maxId) {
            maxId = user.userId;
        }
    }
    return maxId + 1;
}

// Returns next available member ID.
int JsonDataStore::generateNextMemberId() const {
    int maxId = 0;
    for (const Member& member : members) {
        if (member.memberId > maxId) {
            maxId = member.memberId;
        }
    }
    return maxId + 1;
}

// Returns next available workout plan ID.
int JsonDataStore::generateNextPlanId() const {
    int maxId = 0;
    for (const WorkoutPlan& plan : workoutPlans) {
        if (plan.planId > maxId) {
            maxId = plan.planId;
        }
    }
    return maxId + 1;
}
