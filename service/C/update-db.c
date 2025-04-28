#include "../update-db.h"

#include <stdio.h>

error UpdateDB(Auth* auth, Status* status, Path canUCdir) {
    LogMsg("Update DB API called"); // Log message
    if (auth == NULL) {
        return "Auth pointer is NULL"; // Invalid auth pointer
    }
    FreeStatusContent(status); // Free existing status
    
    error err = NULL;
    err = createStatus(status, 0, "Failed to update database", "Failed to update database"); // Create status
    if (err != NULL) {
        return err; // Memory allocation failed
    }


    // 1. mkdir DB folder in CanUC dir
    Path DBPath;
    initPath(&DBPath); // Initialize DB path
    err = createFolderPathLen(&DBPath, "DB", canUCdir, 2); // Create DB folder path
    if (err != NULL) return err; // Memory allocation failed
    printf("DB Path: ");
    LogMsg(DBPath.path); // Log DB path
    err = MakeFolderByPath(DBPath.path); // Create DB folder
    if (err != NULL) return err; // Folder creation failed


    // 2. mkdir `{studentId}` folder in DB folder
    Path studentIdPath;
    initPath(&studentIdPath); // Initialize student ID path length
    err = createFolderPath(&studentIdPath, auth->studentId, DBPath); // Create student ID folder path
    if (err != NULL)return err;
    printf("Student ID Path: ");
    LogMsg(studentIdPath.path); // Log student ID path
    err = MakeFolderByPath(studentIdPath.path); // Create student ID folder
    if (err != NULL)return err; // Folder creation failed


    // 3. mkdir Notification folder in `{studentId}` folder
    Path notificationPath;
    initPath(&notificationPath); // Initialize Notification path length
    err = createFolderPathLen(&notificationPath, "Notification", studentIdPath, 12); // Create Notification folder path
    if (err != NULL) {
        return err; // Memory allocation failed
    }
    printf("Notification Path: ");
    LogMsg(notificationPath.path); // Log Notification path
    err = MakeFolderByPath(notificationPath.path); // Create Notification folder
    if (err != NULL) {
        return err; // Folder creation failed
    }
    

    // 4. mkdir Calendar folder in `{studentId}` folder
    Path calendarPath;
    initPath(&calendarPath); // Initialize Calendar path length
    err = createFolderPathLen(&calendarPath, "Calendar", studentIdPath, 8); // Create Calendar folder path
    if (err != NULL) {
        return err; // Memory allocation failed
    }
    printf("Calendar Path: ");
    LogMsg(calendarPath.path); // Log Calendar path
    err = MakeFolderByPath(calendarPath.path); // Create Calendar folder
    if (err != NULL) {
        return err; // Folder creation failed
    }


    // 5. mkdir LEB2 folder in `{studentId}` folder
    Path leb2Path;
    initPath(&leb2Path); // Initialize LEB2 path length
    err = createFolderPathLen(&leb2Path, "LEB2", studentIdPath, 4); // Create LEB2 folder path
    if (err != NULL) {
        return err; // Memory allocation failed
    }
    printf("LEB2 Path: ");
    LogMsg(leb2Path.path); // Log LEB2 path
    err = MakeFolderByPath(leb2Path.path); // Create LEB2 folder
    if (err != NULL) {
        return err; // Folder creation failed
    }
    

    // 6. mkdir `{semesterId}` folder in LEB2 folder (loop for each semesterId)
    // uint8 semesterCount = 0;
    
    // 7. mkdir `{classId}` folder in `{semesterId}` folder (loop for each classId)

    // 8. mkdir Dashboard folder in `{classId}` folder
    // 9. mkdir Syllabus folder in `{classId}` folder
    // 10. mkdir AssignmentActivity folder in `{classId}` folder
    // 11. mkdir LearningActivity folder in `{classId}` folder
    // 12. mkdir Attendance folder in `{classId}` folder
    // 13. mkdir ScoreBook folder in `{classId}` folder
    // 14. mkdir LearnIt folder in `{classId}` folder
    // 15. mkdir Survey folder in `{classId}` folder
    // 16. mkdir Member folder in `{classId}` folder
    // 17. mkdir File folder in `{classId}` folder

    FreePathContent(&DBPath); // Free DB path content
    FreePathContent(&studentIdPath); // Free student ID path content
    FreePathContent(&notificationPath); // Free Notification path content
    FreePathContent(&calendarPath); // Free Calendar path content
    FreePathContent(&leb2Path); // Free LEB2 path content
    
    err = createStatus(status, 1, "Database updated successfully", "Database updated successfully"); // Create success status
    if (err != NULL) {
        return err; // Memory allocation failed
    }
    
    return NULL; // Success
}