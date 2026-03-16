# Gym Management Console Application - Implementation Details (Current)

## 1) Project Status Summary

This document describes the **current implemented codebase** in detail, including:

- What each module/class does
- What each coded function does
- How each function works internally
- Data flow and control flow across layers

Application type: C++ console app with simple layered structure.

Layers:
- UI (`GymApp`) for menus and user interaction
- Services for business logic
- Storage (`JsonDataStore`) for in-memory collections + JSON persistence
- Models for domain objects

Persistence files:
- `data/users.json`
- `data/members.json`
- `data/workout_plans.json`
- `data/progress_entries.json`

---

## 2) Current Functional Scope Implemented

### 2.1 Authentication and role routing
- Login via username/password
- Role lookup (`member`, `trainer`, `admin`)
- Post-login routing to role-specific submenu

### 2.2 Member registration and profile
- Self-registration from main menu
- Registration creates both:
  - a `Member` record
  - a linked `User` record with role `member`
- Profile update support:
  - member can update **own** profile from member menu
  - admin can update any member profile by member ID

### 2.3 Member self-access restriction
- Logged-in member is linked to `Member.username`
- Member submenu actions use session-linked member only
- No member-side prompt for arbitrary member ID in self-actions

### 2.4 BMI
- Input in centimeters and kilograms
- BMI formula calculation
- Category mapping (`Underweight`, `Normal`, `Overweight`, `Obese`)

### 2.5 Progress tracking
- Add progress entries
- View progress history by member
- When adding progress, member weight is updated and BMI in the entry is recalculated

### 2.6 Workout planning
- Generate basic canned plan by goal
- Assign plan to member
- View assigned plan by member

### 2.7 Statistics
- Total members
- Average BMI
- BMI category distribution

### 2.8 Staff account management
- Admin can create trainer/admin accounts

### 2.9 Input robustness
- Menu input handles invalid values
- EOF-safe behavior added so app exits/returns safely instead of looping forever
- Non-menu prompts also use safe readers to avoid stuck fail states
- Text prompts use `std::getline` so names, notes, and other fields can include spaces
- Numeric readers (`readMenuChoice`, `readIntPrompt`, `readDoublePrompt`) flush the trailing newline after each successful read so subsequent `getline` calls are not skipped

### 2.10 Immediate JSON persistence
- `storage.saveAll()` is called immediately after every successful write operation:
  - Member registration (`handleRegisterMember`)
  - Member profile update — admin path (`handleUpdateMemberProfile`)
  - Member profile update — self path (`handleUpdateMyProfile`)
  - Add progress entry — admin/trainer path (`handleAddProgressEntry`)
  - Add progress entry — member self path (`handleAddMyProgressEntry`)
  - Generate and assign workout plan (`handleGenerateAndAssignWorkoutPlan`)
  - Create staff account (`handleCreateStaffAccount`)
- Data is no longer lost if the application terminates unexpectedly
- `saveAll()` in `run()` on clean exit is retained as a final flush

### 2.11 Membership UX improvements
- Gender and membership type selected via numeric options
- Membership start date auto-set to today
- Expiry date auto-calculated from membership type

---

## 3) Runtime Flow (End-to-End)

1. `main()` creates `GymApp` and calls `run()`.
2. `run()` calls `storage.loadAll()`.
3. If `users` is empty, default admin is auto-created.
4. Main loop shows:
   - Login
   - Register Member
   - Exit
5. After login, role decides submenu:
   - Member menu (self-only actions)
   - Trainer menu
   - Admin menu
6. After every successful write operation, `storage.saveAll()` is called immediately to persist changes.
7. On clean app shutdown, `storage.saveAll()` is called once more as a final flush.

---

## 4) Detailed File and Function Reference

## 4.1 Entry Point

### File: `src/main.cpp`

#### `int main()`
Purpose:
- Application entry point.

How it works:
- Instantiates `GymApp app;`
- Calls `app.run();`
- Returns `0`.

---

## 4.2 Models

### File: `src/models/User.h`, `src/models/User.cpp`

#### `User::User()`
Purpose:
- Default constructor.

How it works:
- Initializes `userId` to `0`.

#### `User::User(int id, const std::string& uname, const std::string& pwd, const std::string& userRole)`
Purpose:
- Value constructor for full user creation.

How it works:
- Assigns all fields directly (`userId`, `username`, `password`, `role`).

#### `bool User::checkPassword(const std::string& inputPassword) const`
Purpose:
- Verifies user password during login.

How it works:
- Compares plain text `password == inputPassword`.

Note:
- Passwords are currently plain text (no hashing in current implementation).

---

### File: `src/models/Member.h`, `src/models/Member.cpp`

#### Data fields
- `memberId`
- `username` (critical for member account linkage)
- `name`, `age`, `gender`
- `height` (cm), `weight` (kg)
- `membershipType`, `startDate`, `expiryDate`

#### `Member::Member()`
Purpose:
- Default constructor.

How it works:
- Initializes numeric fields (`memberId`, `age`, `height`, `weight`) to zero values.

#### `void Member::updateProfile(...)`
Purpose:
- Utility mutator for profile data fields.

How it works:
- Overwrites `name`, `age`, `gender`, `height`, `weight`.

#### `std::string Member::getMembershipDetails() const`
Purpose:
- Generates display-friendly membership text.

How it works:
- Concatenates membership type, start date, and expiry date in one line.

---

### File: `src/models/WorkoutItem.h`, `src/models/WorkoutItem.cpp`

#### `WorkoutItem::WorkoutItem()`
Purpose:
- Default constructor.

How it works:
- Initializes `sets` and `reps` to `0`.

#### `WorkoutItem::WorkoutItem(const std::string& name, int setCount, int repCount, const std::string& desc)`
Purpose:
- Value constructor for one exercise item.

How it works:
- Assigns `exerciseName`, `sets`, `reps`, `description`.

---

### File: `src/models/WorkoutPlan.h`, `src/models/WorkoutPlan.cpp`

#### Fields
- `planId`
- `memberId`
- `goal`
- `items` (`std::vector<WorkoutItem>`)

#### `WorkoutPlan::WorkoutPlan()`
Purpose:
- Default constructor.

How it works:
- Initializes `planId` and `memberId` to `0`.

#### `std::string WorkoutPlan::getPlanText() const`
Purpose:
- Text rendering of workout plan content.

How it works:
- Creates stream output with goal and each exercise line
- Iterates all `items` and formats sets/reps/description.

---

### File: `src/models/ProgressEntry.h`, `src/models/ProgressEntry.cpp`

#### Fields
- `memberId`
- `date`
- `weight`
- `bmi`
- `achievement`

#### `ProgressEntry::ProgressEntry()`
Purpose:
- Default constructor.

How it works:
- Initializes numeric fields (`memberId`, `weight`, `bmi`) to zero values.

---

## 4.3 Services

### File: `src/services/AuthService.h`, `src/services/AuthService.cpp`

#### `AuthService::AuthService(JsonDataStore& dataStore)`
Purpose:
- Inject storage dependency.

How it works:
- Stores reference in `storage`.

#### `bool AuthService::login(const std::string& username, const std::string& password)`
Purpose:
- Credential check.

How it works:
- `findUserByUsername(username)`
- Returns true only if user exists and `checkPassword(password)` is true.

#### `std::string AuthService::getRole(const std::string& username) const`
Purpose:
- Fetch role for routing after successful login.

How it works:
- Reads user from storage
- Returns `""` if not found, otherwise returns `user->role`.

---

### File: `src/services/MemberService.h`, `src/services/MemberService.cpp`

#### `MemberService::MemberService(JsonDataStore& dataStore)`
Purpose:
- Inject storage dependency.

#### `bool MemberService::registerMember(const Member& member)`
Purpose:
- Add member profile.

How it works:
- Delegates to `storage.addMember(member)`.

#### `bool MemberService::updateMember(int memberId, const Member& member)`
Purpose:
- Update existing member profile.

How it works:
- Delegates to `storage.updateMember(memberId, member)`.

#### `Member* MemberService::viewMember(int memberId)`
Purpose:
- Retrieve member pointer for read/update flows.

How it works:
- Delegates to `storage.findMemberById(memberId)`.

---

### File: `src/services/StaffService.h`, `src/services/StaffService.cpp`

#### `StaffService::StaffService(JsonDataStore& dataStore)`
Purpose:
- Inject storage dependency.

#### `bool StaffService::createTrainerAccount(const std::string& username, const std::string& password)`
Purpose:
- Create trainer account.

How it works:
- Gets new ID via `generateNextUserId()`
- Creates `User(..., "trainer")`
- Calls `storage.addUser(...)`.

#### `bool StaffService::createAdminAccount(const std::string& username, const std::string& password)`
Purpose:
- Create admin account.

How it works:
- Same pattern, role set to `"admin"`.

---

### File: `src/services/BMIService.h`, `src/services/BMIService.cpp`

#### `double BMIService::calculateBMI(double height, double weight) const`
Purpose:
- Calculate BMI from height and weight.

How it works:
- Current input expectation: height in centimeters
- Converts cm to meters (`height / 100.0`)
- Returns `weight / (meters^2)`
- Returns `0.0` on non-positive height.

#### `std::string BMIService::getBMICategory(double bmi) const`
Purpose:
- Map BMI number to category.

How it works:
- `< 18.5` -> `Underweight`
- `< 25.0` -> `Normal`
- `< 30.0` -> `Overweight`
- else -> `Obese`.

---

### File: `src/services/WorkoutService.h`, `src/services/WorkoutService.cpp`

#### `WorkoutService::WorkoutService(JsonDataStore& dataStore)`
Purpose:
- Inject storage dependency.

#### `WorkoutPlan WorkoutService::generatePlan(const Member& member, const std::string& goal)`
Purpose:
- Build a basic workout plan.

How it works:
- Allocates new plan
- Assigns new plan ID via `generateNextPlanId()`
- Assigns member ID and goal
- Adds three predefined exercises:
  - Push-ups
  - Squats
  - Plank
- Returns populated plan object.

#### `bool WorkoutService::assignPlan(int memberId, const WorkoutPlan& plan)`
Purpose:
- Persist/replace assigned plan for member.

How it works:
- Delegates to `storage.addOrUpdateWorkoutPlan(memberId, plan)`.

#### `WorkoutPlan* WorkoutService::viewAssignedPlan(int memberId)`
Purpose:
- Retrieve assigned plan by member ID.

How it works:
- Delegates to `storage.findWorkoutPlanByMemberId(memberId)`.

---

### File: `src/services/ProgressService.h`, `src/services/ProgressService.cpp`

#### `ProgressService::ProgressService(JsonDataStore& dataStore, BMIService& bmiService)`
Purpose:
- Inject storage and BMI dependencies.

#### `bool ProgressService::addProgress(int memberId, const std::string& date, double weight, const std::string& achievement)`
Purpose:
- Add progress entry and synchronize member weight.

How it works:
1. Finds member by ID
2. Fails if member not found
3. Updates `member->weight` with new weight
4. Creates `ProgressEntry`
5. Recalculates BMI using member height + new weight
6. Persists with `storage.addProgressEntry(entry)`.

#### `std::vector<ProgressEntry> ProgressService::viewProgressHistory(int memberId) const`
Purpose:
- Retrieve all progress entries for member.

How it works:
- Delegates to `storage.getProgressByMemberId(memberId)`.

---

### File: `src/services/StatisticsService.h`, `src/services/StatisticsService.cpp`

#### `StatisticsService::StatisticsService(JsonDataStore& dataStore, BMIService& bmiService)`
Purpose:
- Inject storage and BMI dependencies.

#### `int StatisticsService::getTotalMembers() const`
Purpose:
- Get member count.

How it works:
- Returns `storage.members.size()` cast to int.

#### `double StatisticsService::getAverageBMI() const`
Purpose:
- Compute average BMI across all members.

How it works:
- Returns `0.0` when no members
- Loops through members, computes BMI for each, sums values
- Returns sum / count.

#### `std::map<std::string, int> StatisticsService::getBMIDistribution() const`
Purpose:
- Count members in each BMI category.

How it works:
- For each member: compute BMI -> map to category -> increment category counter.

---

### File: `src/services/ValidationService.h`, `src/services/ValidationService.cpp`

#### `bool validateMenuChoice(int value, int min, int max) const`
Purpose:
- Generic integer range check.

#### `bool validateAge(int age) const`
Purpose:
- Age bounds check.

How it works:
- Valid when `10 <= age <= 100`.

#### `bool validateHeight(double height) const`
Purpose:
- Height bounds check (cm).

How it works:
- Valid when `50.0 <= height <= 250.0`.

#### `bool validateWeight(double weight) const`
Purpose:
- Weight bounds check (kg).

How it works:
- Valid when `20.0 < weight < 300.0`.

#### `bool validateText(const std::string& input) const`
Purpose:
- Required field check.

How it works:
- Returns true when string is non-empty.

---

## 4.4 Storage

### File: `src/storage/JsonDataStore.h`, `src/storage/JsonDataStore.cpp`

### 4.4.1 Internal helper functions (anonymous namespace)

#### `readTextFile(path)`
- Reads full text file content into string.

#### `writeTextFile(path, content)`
- Writes text in truncate mode.

#### `extractObjects(jsonArrayText)`
- Lightweight parser that scans `{...}` blocks by brace depth.
- Returns vector of object substrings.

#### `escapeJson(text)`
- Escapes `\` and `"` for safe JSON string output.

#### `getStringField(objectText, key)` / `getIntField(...)` / `getDoubleField(...)`
- Lightweight key-based value extraction using string search.

#### `buildUsersJson(...)`, `buildMembersJson(...)`, `buildWorkoutPlansJson(...)`, `buildProgressJson(...)`
- Serialize in-memory vectors into JSON arrays.

Important detail:
- `buildMembersJson(...)` includes `username` field to preserve member-user linkage.

### 4.4.2 Public storage API

#### `bool loadAll()`
Purpose:
- Load all persisted data into memory.

How it works:
1. Ensures `data` folder exists via `_mkdir("data")`
2. Clears all in-memory vectors
3. Loads/parses users
4. Loads/parses members (including `username`)
5. Loads/parses workout plans
6. Loads/parses progress entries
7. Returns true.

#### `bool saveAll() const`
Purpose:
- Persist all in-memory vectors to JSON files.

How it works:
- Ensures `data` folder exists
- Writes all four JSON outputs
- Returns `true` only when all writes succeed.

#### `bool addUser(const User& user)`
- Rejects duplicate usernames
- Pushes user if unique.

#### `bool addMember(const Member& member)`
- Rejects duplicate member IDs
- Pushes member if unique.

#### `bool updateMember(int memberId, const Member& member)`
- Finds existing member by ID
- Overwrites entire record with supplied member object
- Re-applies original `memberId`.

#### `bool addOrUpdateWorkoutPlan(int memberId, const WorkoutPlan& plan)`
- If existing plan for member exists -> replace it
- Else push new plan
- Ensures `memberId` on stored plan matches input parameter.

#### `bool addProgressEntry(const ProgressEntry& entry)`
- Appends progress entry.

#### `User* findUserByUsername(...)` and `const User* findUserByUsername(...) const`
- Linear search over `users`.

#### `Member* findMemberById(int memberId)`
- Linear search over `members`.

#### `Member* findMemberByUsername(const std::string& username)`
- Linear search over `members` by `member.username`.
- Used to enforce member self-profile binding after login.

#### `WorkoutPlan* findWorkoutPlanByMemberId(int memberId)`
- Linear search over plans.

#### `std::vector<ProgressEntry> getProgressByMemberId(int memberId) const`
- Returns copied filtered vector for target member.

#### ID generators
- `generateNextUserId()`
- `generateNextMemberId()`
- `generateNextPlanId()`

How they work:
- Scan max existing ID, return `max + 1`.

---

## 4.5 UI and Control Layer

### File: `src/ui/GymApp.h`, `src/ui/GymApp.cpp`

This is the application orchestrator (menus + workflow + service coordination).

### 4.5.1 Internal UI helper functions (anonymous namespace)

#### `printLine()`, `printTableRule()`, `printHeader(title)`, `printSuccess(msg)`, `printError(msg)`
- Console formatting and status messaging.

#### `int readMenuChoice(int min, int max)`
Purpose:
- Safe menu input parser.

How it works:
- Reads integer
- Returns:
  - valid choice in range
  - `-1` for invalid format/range
  - `-2` for EOF (critical for graceful shutdown)

#### `readTextPrompt(...)`, `readIntPrompt(...)`, `readDoublePrompt(...)`
Purpose:
- Safe non-menu prompt readers.

How they work:
- `readTextPrompt`: uses `std::getline` to read the full input line, supporting spaces in names, notes, and other text fields. Trims trailing `\r` for Windows line-ending compatibility. Returns false on EOF.
- `readIntPrompt` / `readDoublePrompt`: use `std::cin >>` for numeric extraction, then call `cin.ignore(max, '\n')` on success to flush the trailing newline so the next `readTextPrompt` getline call reads correctly.
- `readMenuChoice`: same newline flush after a valid in-range integer read.
- All three: return false on EOF or persistent stream failure; clear and ignore on transient bad input.

### 4.5.2 Constructor and root loop

#### `GymApp::GymApp()`
- Wires service objects with shared `storage`
- Initializes `currentMemberId` to `0`.

#### `void run()`
Purpose:
- Primary app lifecycle.

How it works:
1. `storage.loadAll()`
2. Bootstrap default admin if no users
3. Main menu loop with EOF-safe exit
4. Dispatches to login/register/exit
5. On finish: final `storage.saveAll()` flush.

### 4.5.3 Main menu and auth

#### `showMainMenu()`
- Displays 3 options: Login, Register Member, Exit.

#### `handleLogin()`
Purpose:
- Authenticate and route by role.

How it works:
1. Reads username/password safely
2. Calls `authService.login`
3. Reads role via `getRole`
4. Stores session fields: `currentUsername`, `currentRole`, `currentMemberId`
5. For role `member`, resolves linked profile by `storage.findMemberByUsername(username)`
6. If not linked, aborts with error and resets session
7. Routes to role submenu.

### 4.5.4 Registration and generic actions

#### `handleRegisterMember()`
Purpose:
- Register new member + member user account.

How it works:
1. Reads username/password with validation
2. Ensures username uniqueness in users table
3. Reads profile fields (name supports spaces via `getline`)
4. Uses `chooseGender()` and `chooseMembershipType()` numeric selectors
5. Auto-sets dates: today + computed expiry
6. Validates values (age/height/weight/text)
7. Generates member ID and sets `member.username`
8. Persists member via `memberService.registerMember`
9. Persists user via `storage.addUser(User(..., "member"))`
10. Calls `storage.saveAll()` immediately after successful creation
11. Calculates and prints initial BMI summary.

#### `handleCalculateBMI() const`
- Reads height/weight safely
- Validates ranges
- Calculates BMI and category and prints table output.

#### `handleUpdateMemberProfile()` (admin path)
- Prompts for target member ID
- Loads member
- Reads new values (name supports spaces)
- Uses numeric gender/membership selectors
- Auto-updates start/expiry dates
- Validates and persists via `memberService.updateMember`
- Calls `storage.saveAll()` immediately on success.

#### `handleViewMembershipDetails()` (admin/trainer utility path)
- Prompts member ID and shows basic membership details.

#### `handleAddProgressEntry()` (trainer/admin utility path)
- Prompts member ID/date/weight/achievement (achievement supports spaces)
- Validates and adds progress through `progressService`
- Calls `storage.saveAll()` immediately on success.

#### `handleViewProgressHistory() const` (trainer/admin utility path)
- Prompts member ID, fetches history, renders table.

#### `handleGenerateAndAssignWorkoutPlan()`
- Prompts member ID + goal
- Generates plan through `workoutService.generatePlan`
- Assigns via `assignPlan`
- Calls `storage.saveAll()` immediately on successful assignment
- Prints textual plan.

#### `handleViewAssignedWorkoutPlan()`
- Prompts member ID
- Retrieves and prints assigned plan in table format.

#### `handleViewGymStatistics() const`
- Calls statistics service for totals/average/distribution
- Renders metric table + category list.

#### `handleCreateStaffAccount()`
- Menu: trainer vs admin account
- Reads username/password
- Validates uniqueness and creates via `StaffService`
- Calls `storage.saveAll()` immediately on success.

### 4.5.5 Menu renderers and submenu loops

#### `showMemberSubMenu()`, `showTrainerSubMenu()`, `showAdminSubMenu()`
- Print per-role options.

#### `runMemberSubMenu()`
- Loop + EOF-safe exit
- Dispatches member self-actions only
- Calls `resetSession()` on logout/EOF.

#### `runTrainerSubMenu()`
- Loop + dispatch trainer actions
- EOF/logout resets session.

#### `runAdminSubMenu()`
- Loop + dispatch admin actions
- EOF/logout resets session.

### 4.5.6 Member self-scoped handlers

#### `handleViewMyMembershipDetails()`
- Resolves current member from session
- Displays linked member ID/username/name/membership.

#### `handleUpdateMyProfile()`
- Uses current session-linked member only
- Updates own profile with validation (name supports spaces)
- Uses numeric gender/membership selectors
- Auto-updates membership dates
- Calls `storage.saveAll()` immediately on success.

#### `handleAddMyProgressEntry()`
- Adds progress for current member only (achievement note supports spaces)
- Calls `storage.saveAll()` immediately on success.

#### `handleViewMyProgressHistory()`
- Shows progress for current member only.

#### `handleViewMyAssignedWorkoutPlan()`
- Shows assigned plan for current member only.

### 4.5.7 Session and selection utilities

#### `resetSession()`
- Clears `currentUsername`, `currentRole`, and resets `currentMemberId`.

#### `chooseGender()`
- Numeric selector (1 male, 2 female, 3 other)
- Returns mapped string or empty on invalid.

#### `chooseMembershipType()`
- Numeric selector (1 monthly, 2 quarterly, 3 annual)
- Returns mapped string or empty on invalid.

#### `getTodayDate() const`
- Uses `std::time` and `std::localtime`
- Formats `YYYY-MM-DD`.

#### `calculateExpiryDate(const std::string& membershipType) const`
- Starts from today
- Adds months by type:
  - monthly +1
  - quarterly +3
  - annual +12
- Uses `std::mktime` normalization
- Formats `YYYY-MM-DD`.

#### `getCurrentMember()`
- Returns pointer to session member using:
  1. `currentMemberId` if available
  2. fallback `currentUsername` lookup
- Returns `nullptr` if not resolvable.

---

## 5) Current Menus (Actual Behavior)

### Main Menu
1. Login
2. Register Member
3. Exit

### Member Menu
1. View Membership Details (self)
2. Update My Profile
3. View Assigned Workout Plan (self)
4. Add Progress Entry (self)
5. View Progress History (self)
6. BMI Calculator
7. Logout

### Trainer Menu
1. Generate & Assign Workout Plan
2. View Assigned Workout Plan
3. Add Progress Entry
4. View Progress History
5. Logout

### Admin Menu
1. Register Member
2. Create Staff Account
3. View Gym Statistics
4. View Membership Details
5. Update Member Profile
6. Logout

---

## 6) Data Model and Persistence Details

### 6.1 User record
- `userId`, `username`, `password`, `role`

### 6.2 Member record
- `memberId`, `username`, `name`, `age`, `gender`, `height`, `weight`, `membershipType`, `startDate`, `expiryDate`

### 6.3 Workout plan record
- `planId`, `memberId`, `goal`

Note:
- Current JSON persistence stores plan-level data only; `WorkoutItem` exercise list is generated in service logic and displayed in-memory.

### 6.4 Progress entry record
- `memberId`, `date`, `weight`, `bmi`, `achievement`

---

## 7) Validation Rules Implemented

- Age: `10..100`
- Height(cm): `50.0..250.0`
- Weight(kg): `(20.0, 300.0)`
- Text: non-empty
- Menu choices: bounded range with invalid/EOF handling

---

## 8) Security and Access Constraints (Current)

Implemented:
- Member self-access binding by username link
- Member menu operates only on current session member
- Username uniqueness enforced for user accounts

Not implemented yet:
- Password hashing/salting
- Account lockout / login attempt throttling
- Deep input sanitization beyond current checks

---

## 9) Build and Run

From project root:

```powershell
g++ -std=c++17 -I src src/main.cpp src/models/User.cpp src/models/Member.cpp src/models/WorkoutItem.cpp src/models/WorkoutPlan.cpp src/models/ProgressEntry.cpp src/services/AuthService.cpp src/services/MemberService.cpp src/services/StaffService.cpp src/services/BMIService.cpp src/services/WorkoutService.cpp src/services/ProgressService.cpp src/services/StatisticsService.cpp src/services/ValidationService.cpp src/storage/JsonDataStore.cpp src/ui/GymApp.cpp -o gym_app.exe

.\gym_app.exe
```

---

## 10) Current Simplifications / Known Gaps

1. Date is format-expected (`YYYY-MM-DD`) but not regex/calendar validated.
2. Passwords are plain text.
3. Workout exercise items are not persisted in workout JSON.
4. JSON parser is intentionally lightweight and not a full spec-compliant parser.

---

## 11) Suggested Next Technical Improvements

1. Add strict date validation utility.
2. Hash passwords.
3. Persist workout `items` to JSON.
4. Add unit tests for services (BMI, Progress, Stats, Validation).
5. Add consistency checks for `User(role=member)` <-> `Member(username)` linkage at load time.
