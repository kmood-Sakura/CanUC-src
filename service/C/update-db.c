#include "../update-db.h"

#include <stdio.h>
#include <stdlib.h>

Status SetUpDataBase(Auth* auth) {
    LogMsg("SetUp DB API called");
    
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");;
    }
    if (auth->studentId == NULL) {
        return SetStatus(0, "Student ID is NULL", "Invalid student ID");;
    }

    Path currentPath;
    error err = NULL;
    err = getCurrentPath(&currentPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to get current path", "Failed to get current path");
    }
    
    // printf("Current Path: ");
    // LogMsg(currentPath.path);
    
    // 1. Create DB folder in current path
    Path DBPath;
    initPath(&DBPath);
    err = createFolderPathLen(&DBPath, "DB", currentPath, 2); // Create DB folder path
    if (err != NULL) {
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create DBPath", "Failed to create DBPath");
    }
    
    // printf("DB Path: ");
    // LogMsg(DBPath.path);
    
    err = MakeFolderByPath(DBPath.path); // Create DB folder
    if (err != NULL) {
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create DB folder", err);
    }
    
    // 2. Create student ID folder in DB folder
    Path studentId;
    initPath(&studentId);
    err = createPath(&studentId, auth->studentId);
    if (err != NULL) {
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create studentId path", err);
    }

    Path studentIdPath;
    initPath(&studentIdPath);
    err = createDirPath(&studentIdPath, studentId, DBPath);
    if (err != NULL) {
        FreePathContent(&studentId);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create studentId path", err);
    }

    // printf("Student ID Path: ");
    // LogMsg(studentIdPath.path);
    
    err = MakeFolderByPath(studentIdPath.path);
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create student ID folder", err);
    }
    
    // 3. Create Notification folder in student ID folder
    Path notification;
    initPath(&notification); // Initialize Notification path
    err = createPathLen(&notification, "Notification", 12); // Create Notification path
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Notification path", err);
    }

    Path notificationPath;
    initPath(&notificationPath); // Initialize Notification path
    err = createDirPath(&notificationPath, notification, studentIdPath); // Create Notification folder path
    if (err != NULL) {
        FreePathContent(&notification);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create NotificationPath path", err);
    }
    
    // printf("Notification Path: ");
    // LogMsg(notificationPath.path);
    
    err = MakeFolderByPath(notificationPath.path); // Create Notification folder
    if (err != NULL) {
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Notification folder", err);
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
        return SetStatus(0, "Failed to create Calendar path", err);
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
        return SetStatus(0, "Failed to create CalendarPath path", err);
    }
    
    // printf("Calendar Path: ");
    // LogMsg(calendarPath.path); // Log Calendar path
    
    err = MakeFolderByPath(calendarPath.path); // Create Calendar folder
    if (err != NULL) {
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Calendar folder", "Failed to create Calendar folder");
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
        return SetStatus(0, "Failed to create LEB2 path", "Failed to create LEB2 path");
    }

    Path leb2Path;
    initPath(&leb2Path);
    err = createDirPath(&leb2Path, leb2, studentIdPath);
    if (err != NULL) {
        FreePathContent(&leb2);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create LEB2Path path", err);
    }
    
    // printf("LEB2 Path: ");
    // LogMsg(leb2Path.path); // Log LEB2 path
    
    err = MakeFolderByPath(leb2Path.path); // Create LEB2 folder
    if (err != NULL) {
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create LEB2 folder", "Failed to create LEB2 folder");
    }
    
    // Now create the DataPath tree structure
    
    DataPath* studentIdDataPath = NULL;
    studentIdDataPath = (DataPath*)malloc(sizeof(DataPath));
    if (studentIdDataPath == NULL) {
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Memory allocation for DB dataPath failed", "Can't ALlocate studentId dataPath");
    }
    
    initDataPath(studentIdDataPath);
    
    err = createDataPath(studentIdDataPath, studentIdPath, studentId, 1);
    if (err != NULL) {
        FreeDataPath(studentIdDataPath);
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create DB dataPath", err);
    }
    
    if (auth->dataPath != NULL) {
        FreeDataPath(auth->dataPath);
        auth->dataPath = NULL;
    }
    
    auth->dataPath = studentIdDataPath;
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
        return SetStatus(0, "Memory allocation for notification dataPath failed", "Can't Allocate notification dataPath");
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
        return SetStatus(0, "Failed to create notification dataPath", err);
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
        return SetStatus(0, "Failed to add notification dataPath as child", err);
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
        return SetStatus(0, "Memory allocation for calendar dataPath failed", "Can't Allocate calendar dataPath");
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
        return SetStatus(0, "Failed to create calendar dataPath", err);
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
        return SetStatus(0, "Failed to add calendar dataPath as child", err);
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
        return SetStatus(0, "Memory allocation for LEB2 dataPath failed", "Can't Allocate LEB2 dataPath");
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
        return SetStatus(0, "Failed to create LEB2 dataPath", err);
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
        return SetStatus(0, "Failed to add LEB2 dataPath as child", err);
    }
    
    // Free studentIdDataPathorary Path objects
    FreePathContent(&leb2);
    FreePathContent(&calendar);
    FreePathContent(&notification);
    FreePathContent(&studentId);
    
    FreePathContent(&leb2Path);
    FreePathContent(&calendarPath);
    FreePathContent(&notificationPath);
    FreePathContent(&studentIdPath);
    FreePathContent(&DBPath);
    FreePathContent(&currentPath);
    
    return SetStatus(1, "Database setup successfully", NULL);
}
//update