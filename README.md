# Gym Management Application (C++ Console)

A role-based Gym Management System built in C++ with JSON persistence.

## Overview

This project manages gym operations for three roles:
- **Member**
- **Trainer**
- **Admin**

It supports authentication, member profile management, BMI calculation, progress tracking, workout planning, and gym statistics.

## Key Features

- Login with role-based menu routing
- Member registration with linked user account
- Member self-access restriction (members can only view/update their own data)
- Profile update and membership management
- BMI calculator (cm + kg)
- Progress entry + progress history
- Workout plan generation and assignment
- Gym statistics (total members, average BMI, BMI distribution)
- Staff account creation (admin)
- Immediate JSON persistence after successful write operations
- EOF-safe and robust input handling (including text with spaces)

## Tech Stack

- **Language:** C++17
- **Compiler:** g++
- **Storage:** JSON files (custom parser/serializer)
- **UI:** Console menus

## Project Structure

```text
src/
  main.cpp
  models/
  services/
  storage/
  ui/

data/
  users.json
  members.json
  workout_plans.json
  progress_entries.json

Design_Documents/
  Use_Case_Model.md
  Class_Design_CPP.md
  IMPLEMENTATION_DETAILS.md
  TEAM_LEAD_PRESENTATION.md
```

## Architecture

The system uses a layered architecture:

1. **UI Layer** (`GymApp`)  
   Handles menus, input prompts, and flow routing.

2. **Service Layer**  
   Business logic in dedicated services:
   - `AuthService`
   - `MemberService`
   - `StaffService`
   - `WorkoutService`
   - `ProgressService`
   - `StatisticsService`
   - `ValidationService`
   - `BMIService`

3. **Storage Layer** (`JsonDataStore`)  
   In-memory collections + JSON load/save.

4. **Model Layer**  
   Domain entities:
   - `User`
   - `Member`
   - `WorkoutPlan`
   - `WorkoutItem`
   - `ProgressEntry`

## Data Persistence Flow

- On startup: `loadAll()` loads all JSON files into memory.
- During runtime: services update in-memory collections.
- After successful write operations: `saveAll()` is called immediately.
- On exit: final `saveAll()` flush.

Data files:
- `data/users.json`
- `data/members.json`
- `data/workout_plans.json`
- `data/progress_entries.json`

## Build and Run

From the project root:

```powershell
g++ -std=c++17 -I src src/main.cpp src/models/User.cpp src/models/Member.cpp src/models/WorkoutItem.cpp src/models/WorkoutPlan.cpp src/models/ProgressEntry.cpp src/services/AuthService.cpp src/services/MemberService.cpp src/services/StaffService.cpp src/services/BMIService.cpp src/services/WorkoutService.cpp src/services/ProgressService.cpp src/services/StatisticsService.cpp src/services/ValidationService.cpp src/storage/JsonDataStore.cpp src/ui/GymApp.cpp -o gym_app.exe

.\gym_app.exe
```

## Default Behavior

- If no users exist, a default admin is auto-created:
  - **Username:** `admin`
  - **Password:** `admin123`

## Validation Rules

- Age: `10..100`
- Height (cm): `50..250`
- Weight (kg): `>20 and <300`
- Text fields: non-empty
- Menu choices: range-validated

## Current Limitations

- Passwords are stored in plain text
- Date input expects `YYYY-MM-DD` but is not strict calendar-validated
- JSON parser is lightweight and tailored to current data format

## Future Improvements

- Password hashing and stronger auth security
- Strict date validation
- Persist richer workout item details
- Unit tests for service layer
- Optional DB-backed storage migration

## Author

Gym Management Application Team
