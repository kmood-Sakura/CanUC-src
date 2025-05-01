#include "../update-db.h"

#include <stdio.h>
#include <stdlib.h>

Status SetUpDataBase(Auth* auth) {
    Status status;
    initStatus(&status); // Initialize status

    LogMsg("SetUp DB API called"); // Log message
    
    // Verify parameter
    if (auth == NULL) {
        status = SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
        return status;
    }
    
    // Get current path
    Path currentPath;
    error err = NULL;
    err = getCurrentPath(&currentPath);
    if (err != NULL) {
        status = SetStatus(0, "Failed to get current path", "Failed to get current path");
        return status;
    }
    
    printf("Current Path: ");
    LogMsg(currentPath.path); // Log current path
    
    // 1. Create DB folder in current path
    Path DBPath;
    initPath(&DBPath);
    err = createFolderPathLen(&DBPath, "DB", currentPath, 2); // Create DB folder path
    if (err != NULL) {
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create DBPath", "Failed to create DBPath");
        return status;
    }
    
    printf("DB Path: ");
    LogMsg(DBPath.path); // Log DB path
    
    err = MakeFolderByPath(DBPath.path); // Create DB folder
    if (err != NULL) {
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create DB folder", err);
        return status;
    }
    
    // 2. Create student ID folder in DB folder
    Path studentId;
    initPath(&studentId); // Initialize student ID path
    err = createPath(&studentId, auth->studentId); // Create student ID path
    if (err != NULL) {
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create studentId path", err);
        return status;
    }

    Path studentIdPath;
    initPath(&studentIdPath); // Initialize student ID path
    err = createDirPath(&studentIdPath, studentId, DBPath); // Create student ID folder path
    if (err != NULL) {
        FreePathContent(&studentId);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create studentIdPath path", err);
        return status;
    }

    printf("Student ID Path: ");
    LogMsg(studentIdPath.path); // Log student ID path
    
    err = MakeFolderByPath(studentIdPath.path); // Create student ID folder
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create student ID folder", err);
        return status;
    }
    
    // 3. Create Notification folder in student ID folder
    Path notification;
    initPath(&notification); // Initialize Notification path
    err = createPathLen(&notification, "Notification", 12); // Create Notification path
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create Notification path", err);
        return status;
    }

    Path notificationPath;
    initPath(&notificationPath); // Initialize Notification path
    err = createDirPath(&notificationPath, notification, studentIdPath); // Create Notification folder path
    if (err != NULL) {
        FreePathContent(&notification);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create NotificationPath path", err);
        return status;
    }
    
    printf("Notification Path: ");
    LogMsg(notificationPath.path); // Log Notification path
    
    err = MakeFolderByPath(notificationPath.path); // Create Notification folder
    if (err != NULL) {
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create Notification folder", err);
        return status;
    }
    
    // 4. Create Calendar folder in student ID folder
    Path calendar;
    initPath(&calendar); // Initialize Calendar path
    err = createPathLen(&calendar, "Calendar", 8); // Create Calendar path
    if (err != NULL) {
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create Calendar path", err);
        return status;
    }

    Path calendarPath;
    initPath(&calendarPath); // Initialize Calendar path
    err = createDirPath(&calendarPath, calendar, studentIdPath); // Create Calendar folder path
    if (err != NULL) {
        FreePathContent(&calendar);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create CalendarPath path", err);
        return status;
    }
    
    printf("Calendar Path: ");
    LogMsg(calendarPath.path); // Log Calendar path
    
    err = MakeFolderByPath(calendarPath.path); // Create Calendar folder
    if (err != NULL) {
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create Calendar folder", "Failed to create Calendar folder");
        return status;
    }
    
    // 5. Create LEB2 folder in student ID folder
    Path leb2;
    initPath(&leb2); // Initialize LEB2 path
    err = createPathLen(&leb2, "LEB2", 4); // Create LEB2 path
    if (err != NULL) {
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create LEB2 path", "Failed to create LEB2 path");
        return status;
    }

    Path leb2Path;
    initPath(&leb2Path); // Initialize LEB2 path
    err = createDirPath(&leb2Path, leb2, studentIdPath); // Create LEB2 folder path
    if (err != NULL) {
        FreePathContent(&leb2);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create LEB2Path path", err);
        return status;
    }
    
    printf("LEB2 Path: ");
    LogMsg(leb2Path.path); // Log LEB2 path
    
    err = MakeFolderByPath(leb2Path.path); // Create LEB2 folder
    if (err != NULL) {
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create LEB2 folder", "Failed to create LEB2 folder");
        return status;
    }
    
    // Now create the DataPath tree structure
    
    // Prepare DataPath objects for each folder
    DataPath* Temp = NULL;
    
    // Create DB DataPath
    Temp = (DataPath*)malloc(sizeof(DataPath));
    if (Temp == NULL) {
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Memory allocation for DB dataPath failed", "Memory allocation failed");
        return status;
    }
    
    initDataPath(Temp);
    
    err = createDataPath(Temp, studentIdPath, studentId, 1);
    if (err != NULL) {
        FreeDataPath(Temp);
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create DB dataPath", err);
        return status;
    }
    
    // Check if auth->dataPath already exists and free it
    if (auth->dataPath != NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
    }
    
    // Set the main dataPath in auth to point to the student ID path
    auth->dataPath = Temp;
    
    // Create Notification DataPath as child of student ID
    DataPath* notificationDataPath = (DataPath*)malloc(sizeof(DataPath));
    if (notificationDataPath == NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Memory allocation for notification dataPath failed", "Memory allocation failed");
        return status;
    }
    
    initDataPath(notificationDataPath);
    
    err = createDataPath(notificationDataPath, notificationPath, notification, 1);
    if (err != NULL) {
        FreeDataPath(notificationDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create notification dataPath", err);
        return status;
    }
    
    // Add Notification DataPath as child of student ID
    err = addChildDataPath(auth->dataPath, notificationDataPath);
    if (err != NULL) {
        FreeDataPath(notificationDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to add notification dataPath as child", err);
        return status;
    }
    
    // Create Calendar DataPath as child of student ID
    DataPath* calendarDataPath = (DataPath*)malloc(sizeof(DataPath));
    if (calendarDataPath == NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Memory allocation for calendar dataPath failed", "Memory allocation failed");
        return status;
    }
    
    initDataPath(calendarDataPath);
    
    err = createDataPath(calendarDataPath, calendarPath, calendar,1);
    if (err != NULL) {
        FreeDataPath(calendarDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create calendar dataPath", err);
        return status;
    }
    
    // Add Calendar DataPath as child of student ID
    err = addChildDataPath(auth->dataPath, calendarDataPath);
    if (err != NULL) {
        FreeDataPath(calendarDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to add calendar dataPath as child", err);
        return status;
    }
    
    // Create LEB2 DataPath as child of student ID
    DataPath* leb2DataPath = (DataPath*)malloc(sizeof(DataPath));
    if (leb2DataPath == NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Memory allocation for LEB2 dataPath failed", "Memory allocation failed");
        return status;
    }
    
    initDataPath(leb2DataPath);
    
    err = createDataPath(leb2DataPath, leb2Path, leb2,1);
    if (err != NULL) {
        FreeDataPath(leb2DataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create LEB2 dataPath", err);
        return status;
    }
    
    // Add LEB2 DataPath as child of student ID
    err = addChildDataPath(auth->dataPath, leb2DataPath);
    if (err != NULL) {
        FreeDataPath(leb2DataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to add LEB2 dataPath as child", err);
        return status;
    }
    
    // Create DB DataPath as parent of student ID
    DataPath* dbDataPath = (DataPath*)malloc(sizeof(DataPath));
    if (dbDataPath == NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Memory allocation for DB parent dataPath failed", "Memory allocation failed");
        return status;
    }
    
    initDataPath(dbDataPath);
    
    Path dbName;
    initPath(&dbName);
    err = createPathLen(&dbName, "DB", 2);
    if (err != NULL) {
        FreeDataPath(dbDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create DB name path", err);
        return status;
    }
    
    err = createDataPath(dbDataPath, DBPath, dbName,1);
    if (err != NULL) {
        FreePathContent(&dbName);
        FreeDataPath(dbDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to create DB parent dataPath", err);
        return status;
    }
    
    // Set DB as parent of student ID and add student ID as child of DB
    auth->dataPath->parent = dbDataPath;
    err = addChildDataPath(dbDataPath, auth->dataPath);
    if (err != NULL) {
        FreePathContent(&dbName);
        FreeDataPath(dbDataPath);
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        status = SetStatus(0, "Failed to link DB and student ID dataPath", err);
        return status;
    }
    
    // Free temporary Path objects
    FreePathContent(&leb2);
    FreePathContent(&calendar);
    FreePathContent(&notification);
    FreePathContent(&studentId);
    FreePathContent(&dbName);
    
    FreePathContent(&leb2Path);
    FreePathContent(&calendarPath);
    FreePathContent(&notificationPath);
    FreePathContent(&studentIdPath);
    FreePathContent(&DBPath);
    FreePathContent(&currentPath);
    
    // Set success status
    status = SetStatus(1, "Database setup successfully", "Database setup successfully");
    return status;
}
//update