# CanUC-src

## Overview
The LEB2 (Learning Environment and Backpack) application is a comprehensive learning management system that provides course management, calendar scheduling, assignment tracking, and user authentication capabilities. Built in C, it follows a modular architecture with memory-efficient data structures and robust error handling.

## Project Structure
```
src/
├── app/                  # User interface components
│   ├── calendar-page.c/h # Calendar functionality
│   ├── class-operation.c/h # Class management operations
│   ├── home-page.c/h     # Main application entry point
│   ├── leb2-page.c/h     # Learning environment interface
│   ├── login-page.c/h    # Authentication screens
│   └── notification-page.c/h # Notification system
│
├── service/              # Business logic layer
│   ├── authenticate.c/h  # User authentication
│   ├── calendar-service.c/h # Calendar management
│   ├── leb2-service.c/h  # Learning environment services
│   ├── load-user-data.c/h # Data loading utilities
│   └── update-db.c/h     # Database management
│
├── utils/                # Core utilities
│   ├── common/           # Common utilities
│   │   ├── log.h/c       # Logging functionality
│   │   ├── request.h/c   # User input handling
│   │   └── status.h/c    # Status and error codes
│   │
│   ├── datatype/         # Data type definitions
│   │   ├── char-type.h/c # Character utilities
│   │   ├── date-type.h/c # Date/time handling
│   │   ├── float-type.h/c # Floating point types
│   │   ├── int-type.h/c  # Integer type definitions
│   │   └── string-type.h/c # String handling
│   │
│   └── struct/           # Core data structures
│       ├── auth.h/c      # Authentication structure
│       ├── calendar.h/c  # Calendar structure
│       ├── file.h/c      # File operations
│       ├── leb2.h/c      # Learning environment
│       ├── notification.h/c # Notification structure
│       └── path.h/c      # Path management
│
└── main.c                # Application entry point
```

## Features

### User Authentication
- User registration and login
- Secure password handling
- User-specific data management

### Calendar Management
- Add, view, and remove tasks
- Date-based calendar navigation
- Task scheduling with time conflict detection

### Learning Environment (LEB2)
- Semester and course organization
- Assignment management and tracking
- Learning materials organization
- Attendance tracking
- Grade/score management

### File System
- Cross-platform file operations
- Hierarchical data storage
- User data persistence

## Code Walkthrough

This section explains the execution flow and interactions between different components of the application with example inputs and outputs.

### Authentication Flow

### Signup Process (Success Case)

**Input:**
```
--------------------------------------------------------

Authentication

  [1] Login
  [2] Sign up

  [e] Exit

  If you already have an account please login... please 🥺

command: 2

--------------------------------------------------------

  Sign up page

Please enter your student ID and password (nothing default to exit)

  Student ID: 12345678
  Password: password123
```

**Code Execution:**
1. In `AuthenPage()` (login-page.c), user selects option '2'
2. `SignupPage()` is called, which prompts for credentials
3. `AuthInputField()` collects student ID and password
4. `Signup()` calls `SetUpDataBase()` to create directory structure
5. `SetupAuthenticate()` creates a password file with the hashed password

**Output (Success):**
```
--------------------------------------------------------

Home

  [1] LEB2
  [2] Calendar
  [3] Notification

  [e] Exit
```

### Login Process (Error Case)

**Input:**
```
--------------------------------------------------------

Authentication

  [1] Login
  [2] Sign up

  [e] Exit

  If you already have an account please login... please 🥺

command: 1

--------------------------------------------------------

  Login

Please enter your student ID and password (nothing default to exit)

  Student ID: 12345678
  Password: wrong_password
```

**Code Execution:**
1. In `AuthenPage()`, user selects option '1'
2. `LoginPage()` is called, which prompts for credentials
3. `AuthInputField()` collects student ID and password
4. `Login()` calls `Authenticate()` to verify credentials
5. Password comparison fails in `Authenticate()`

**Output (Error):**
```
Login failed. Please try again.

--------------------------------------------------------

  Login

Please enter your student ID and password (nothing default to exit)
```

### Calendar Management Flow

### Adding a Task (Success Case)

**Input:**
```
--------------------------------------------------------

Calendar

  [1] Today's Task
  [2] Add Task
  [3] Remove Task
  [4] Search by Date

  [b] Back

command: 2

--------------------------------------------------------

Add New Task

Enter Task Title: Math Homework
Enter Location: Home
Enter Date (YYYY-MM-DD): 2025-05-10
Enter Start Time (HH MM): 14 00
Enter End Time (HH MM): 16 00
```

**Code Execution:**
1. In `CalendarPage()` (calendar-page.c), user selects option '2'
2. `addTask()` is called, collecting task details
3. `createTaskEntry()` creates a Task structure
4. `addTaskToMemoryCalendar()` checks for time conflicts and adds to memory
5. `updateCalendarFile()` persists the task to a CSV file

**Output (Success):**
```
Task 'Math Homework' added successfully.

--------------------------------------------------------

Calendar

  [1] Today's Task
  [2] Add Task
  [3] Remove Task
  [4] Search by Date

  [b] Back
```

### Adding a Task (Error Case - Time Conflict)

**Input:**
```
--------------------------------------------------------

Calendar

  [1] Today's Task
  [2] Add Task
  [3] Remove Task
  [4] Search by Date

  [b] Back

command: 2

--------------------------------------------------------

Add New Task

Enter Task Title: Team Meeting
Enter Location: Conference Room
Enter Date (YYYY-MM-DD): 2025-05-10
Enter Start Time (HH MM): 15 00
Enter End Time (HH MM): 17 00
```

**Code Execution:**
1. In `CalendarPage()`, user selects option '2'
2. `addTask()` collects task details
3. `addTaskToMemoryCalendar()` detects time conflict with existing "Math Homework" task
4. Error status is returned

**Output (Error):**
```
Failed to add task: Time slot (2025-05-10 15:00:00 - 2025-05-10 17:00:00) overlaps with existing task "Math Homework" (2025-05-10 14:00:00 - 2025-05-10 16:00:00)

--------------------------------------------------------

Calendar

  [1] Today's Task
  [2] Add Task
  [3] Remove Task
  [4] Search by Date

  [b] Back
```

### LEB2 Learning Environment Flow

### Navigating Semesters

**Input:**
```
--------------------------------------------------------

Home

  [1] LEB2
  [2] Calendar
  [3] Notification

  [e] Exit

command: 1

--------------------------------------------------------

LEB2

Semester 1/1

Available Classes: 

  [1] MTH101
  [2] CPE101

  [n] Next  [p] Previous  [s] Select  [b] Back

command: n
```

**Code Execution:**
1. In `HomePage()` (home-page.c), user selects option '1'
2. `LEB2Page()` is called in leb2-page.c
3. `ShowSemester()` displays the current semester
4. When user presses 'n', the next semester is shown if available

**Output (Success - Next Semester):**
```
--------------------------------------------------------

LEB2

Semester 1/2

Available Classes: 

  [1] MTH102
  [2] CPE112

  [n] Next  [p] Previous  [s] Select  [b] Back
```

**Output (Error - No Next Semester):**
```
No next semester available

--------------------------------------------------------

LEB2

Semester 1/2

Available Classes: 

  [1] MTH102
  [2] CPE112

  [n] Next  [p] Previous  [s] Select  [b] Back
```

### Adding an Assignment (Success Case)

**Input:**
```
--------------------------------------------------------

Class: MTH101

  [1] Dashboard
  [2] Syllabus
  [3] Assignments
  [4] Learning Activities
  [5] Attendance
  [6] ScoreBook
  [7] LearnIt
  [8] Files
  [9] Survey
  [0] Class Members

  [b] Back

command: 3

--------------------------------------------------------

Class Assignments - MTH101

  [1] View Assignment List
  [2] Add Assignment
  [3] Remove Assignment

  [b] Back

command: 2

--------------------------------------------------------

Add Assignment

Enter Assignment Title: Calculus Homework
Enter Description: Solve problems 1-10 from Chapter 5
Enter Assign Date (YYYY-MM-DD HH:MM:SS): 2025-05-06 10:00:00
Enter Due Date (YYYY-MM-DD HH:MM:SS): 2025-05-13 23:59:59
```

**Code Execution:**
1. In `ClassDetailsPage()` (leb2-page.c), user selects option '3'
2. `ClassAssignmentPage()` is called from class-operation.c
3. User selects option '2' to add an assignment
4. `AddAssignment()` collects assignment details
5. Creates an Assignment structure and adds it to the class
6. `UpdateAssignmentFile()` persists to file

**Output (Success):**
```
Assignment 'Calculus Homework' added successfully.

--------------------------------------------------------

Class Assignments - MTH101

  [1] View Assignment List
  [2] Add Assignment
  [3] Remove Assignment

  [b] Back
```

## Memory Management Example

The following example demonstrates the pattern used throughout the codebase for memory management:

```c
// Allocation: Creating a new task
Task* newTask = NULL;
error err = allocateTask(&newTask);  // Allocate memory
if (err != NULL) {
    return SetStatus(0, "Failed to allocate task", err);
}

// Initialization: Setting task data
err = createTask(newTask, title, location, startTime, endTime, dueTime);
if (err != NULL) {
    free(newTask);  // Free on error
    return SetStatus(0, "Failed to create task", err);
}

// Usage: Add to list, perform operations

// Cleanup: When done with the task
FreeTaskContent(newTask);  // Free structure content (strings, etc.)
free(newTask);  // Free the structure itself
```

## Error Handling Example

Error handling follows this pattern throughout the codebase:

```c
// Function returns error string or NULL for success
error err = someFunction();
if (err != NULL) {
    // Handle error, possibly propagating it up
    LogMsg(err);
    FreeString(&err);
    return SetStatus(0, "Operation failed", err);
}

// Function returns Status structure
Status status = someOtherFunction();
if (status.code != 1) {
    // Handle failure (0) or warning (-1)
    LogMsg(status.msg);
    // Clean up and return
    return status;
}

// Success path continues
```

## Data Structures Flow

### Linked List Traversal Example

The following shows how linked lists (like TaskList) are traversed:

```c
// Print all tasks in a calendar
TaskList* current = calendar->taskList;
while (current) {
    printf("Task: %s\n", current->task.title);
    printf("  Location: %s\n", current->task.location);
    
    // Convert DateTime to string for display
    string timeStr = NULL;
    dateTimeToString(&timeStr, current->task.setBegin);
    printf("  Time: %s\n", timeStr);
    FreeString(&timeStr);
    
    current = current->next;  // Move to next task
}
```

## Usage Guide

### Authentication

When you start the application, you will be presented with the authentication screen:

```
--------------------------------------------------------

Authentication

  [1] Login
  [2] Sign up

  [e] Exit

  If you already have an account please login... please 🥺
```

#### Sign Up
1. Select option `2` to sign up
2. Enter your student ID and password
3. The system will create necessary directories for your user data

#### Login
1. Select option `1` to login
2. Enter your student ID and password
3. After successful login, you'll be taken to the home page

### Main Navigation

The home page provides access to the main features:

```
--------------------------------------------------------

Home

  [1] LEB2
  [2] Calendar
  [3] Notification

  [e] Exit
```

### LEB2 (Learning Environment)

The LEB2 section allows you to manage your courses:

1. Navigate between semesters using `n` (next) and `p` (previous)
2. Select a class with `s` and enter the class number
3. Access various class components:
   - Dashboard
   - Syllabus
   - Assignments
   - Learning Activities
   - Attendance
   - ScoreBook
   - LearnIt
   - Files
   - Survey
   - Class Members

#### Class Assignments

In the assignments section, you can:
- View all assignments for a class
- Add new assignments with title, description, and due dates
- Remove existing assignments

#### Learning Activities

In the learning activities section, you can:
- View all learning activities for a class
- Add new activities with title, description, and URLs
- Remove existing activities

### Calendar Management

The calendar section allows you to manage your tasks:

```
--------------------------------------------------------

Calendar

  [1] Today's Task
  [2] Add Task
  [3] Remove Task
  [4] Search by Date

  [b] Back
```

#### Add Task
1. Select option `2` to add a task
2. Enter task title, location, date, and time
3. The system will check for time conflicts and add the task

#### View Tasks
1. Select option `1` to view today's tasks or option `4` to search by date
2. Tasks will be displayed in a table format with title, date, time, and location

#### Remove Task
1. Select option `3` to remove a task
2. Enter the date and select the task to remove

## Data Management

The application creates a hierarchical directory structure for user data:

```
DB/
└── {studentId}/
    ├── Calendar/
    │   └── {YYYY-MM-DD}.csv  # Calendar data by date
    ├── LEB2/
    │   └── {year}-{term}/    # Semester directories
    │       └── {classId}/    # Class directories
    │           ├── AssignmentActivity/
    │           ├── Attendance/
    │           ├── Dashboard/
    │           ├── File/
    │           ├── LearnIt/
    │           ├── LearningActivity/
    │           ├── Member/
    │           ├── ScoreBook/
    │           ├── Survey/
    │           └── Syllabus/
    └── Notification/
```

## Memory Management

The application follows strict memory management practices:
- All structures are initialized using `init*()` functions before use
- Memory is allocated with `allocate*()` functions
- Objects are created with `create*()` functions
- Memory is freed with `Free*()` functions when no longer needed

## Error Handling

The application uses a comprehensive status system for error handling:
- `Status` structures contain code, message, and details
- Status codes: 1 (success), 0 (failure), -1 (warning)
- Functions return `error` type (alias for `string`) for error messages
- Errors are logged and displayed to the user

## Contributing Guidelines

When contributing to this project:

1. **Follow the established code structure**
   - Maintain the modular architecture
   - Keep UI, business logic, and data management separated

2. **Use proper memory management**
   - Initialize all structures before use
   - Free all allocated memory
   - Check for NULL pointers

3. **Follow the naming conventions**
   - Functions: `camelCase`
   - Types: `PascalCase`
   - Memory allocation: `allocate*` functions
   - Structure initialization: `init*` functions
   - Object creation: `create*` functions
   - Memory deallocation: `Free*` functions

4. **Include comprehensive error handling**
   - Check return values from all functions
   - Provide detailed error messages
   - Clean up resources on error paths

5. **Test thoroughly**
   - Ensure all features work as expected
   - Verify there are no memory leaks
   - Test on different platforms if possible