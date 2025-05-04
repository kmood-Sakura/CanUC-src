#include "../load-user-data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status LoadUserDataAPI(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    if (auth->userData != NULL) {
        FreeUserData(auth->userData); // Free existing user data
        auth->userData = NULL; // Set userData pointer to NULL
    }
    auth->userData = (UserData*)malloc(sizeof(UserData)); // Allocate memory for user data
    if (auth->userData == NULL) {
        return SetStatus(0, "Memory allocation failed", "Can't Allocate UserData");
    }
    auth->userData->notificationList = NULL; // Initialize notification list
    auth->userData->calendarList = NULL; // Initialize calendar list
    auth->userData->leb2 = NULL; // Initialize LEB2 data

    // Load user data process
    Status status = LoadLEB2Data(auth); // Load LEB2 data
    if (status.code != 1) {
        Error(status.msg); // Log error message
        FreeUserData(auth->userData); // Free user data on failure
        auth->userData = NULL; // Set userData pointer to NULL
        return status; // Return failure status
    }

    return SetStatus(1, "User data loaded successfully", NULL);
}

Status LoadAllUserAppDataPathAPI(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    if (auth->dataPath == NULL) {
        return SetStatus(0, "Data path pointer is NULL", "Data path not loaded yet");
    }
    error err = NULL;
    for (uint16 i = 0; i < auth->dataPath->sizeDir; i++) {
        if (auth->dataPath->Dir[i]->isFolder == 1) {
            err = getDirDataPath(auth->dataPath->Dir[i]);
            if (err != NULL) {
                Error(err);
                err = NULL; // Reset error to NULL for next iteration
                continue;
            }
        }
    }

    return SetStatus(1, "User data path loaded successfully", NULL);
}

// Main function to load all LEB2 data
Status LoadLEB2Data(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    
    if (auth->dataPath == NULL) {
        return SetStatus(0, "DataPath is NULL", "Invalid data path");
    }
    
    error err = NULL;
    // Allocate UserData if not already allocated
    if (auth->userData == NULL) {
        auth->userData = (UserData*)malloc(sizeof(UserData));
        if (auth->userData == NULL) {
            return SetStatus(0, "Failed to allocate UserData", "Memory allocation failed");
        }
        auth->userData->notificationList = NULL;
        auth->userData->calendarList = NULL;
        auth->userData->leb2 = NULL;
    }
    if (auth->userData->leb2 != NULL) {
        FreeLEB2(auth->userData->leb2); // Free existing LEB2 data if it exists
        auth->userData->leb2 = NULL; // Set leb2 pointer to NULL
    }
    // LogMsg("Loading LEB2 data...");
    err = allocateLEB2(&auth->userData->leb2);
    if (err != NULL) {
        return SetStatus(0, "Failed to allocate LEB2", err);
    }
    
    DataPath* leb2DataPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "LEB2", &leb2DataPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to find LEB2 data path", err);
    }
    
    if (leb2DataPath == NULL) {
        return SetStatus(0, "LEB2 folder not found", "LEB2 folder not found in dataPath");
    }
    
    // Initialize SemesterList
    auth->userData->leb2->semesterList = NULL;
    
    // Load semester data
    err = LoadSemesterData(auth, leb2DataPath);
    if (err != NULL) {
        // This is a serious error only if no semesters were loaded at all
        if (auth->userData->leb2->semesterList == NULL) {
            return SetStatus(0, "Failed to load any semester data", err);
        } else {
            LogMsg("Warning: Some semester data could not be loaded");
            Error(err);
            // Continue anyway since we loaded some semesters
        }
    }
    
    if (auth->userData->leb2->semesterList == NULL) {
        return SetStatus(0, "No semesters found", "LEB2 directory contains no semesters");
    }
    
    return SetStatus(1, "LEB2 data loaded successfully", NULL);
}

// Function to load semester data
error LoadSemesterData(Auth* auth, DataPath* leb2DataPath) {
    if (auth == NULL || leb2DataPath == NULL) {
        return "Invalid parameters in LoadSemesterData";
    }
    
    error err = NULL;
    code semestersLoaded = 0;
    
    // Process each semester directory
    for (uint16 i = 0; i < leb2DataPath->sizeDir; i++) {
        DataPath* semesterDataPath = leb2DataPath->Dir[i];
        if (semesterDataPath == NULL) continue;
        // LogMsg("Loading Semester ");
        // printf("%s\n", semesterDataPath->filename.path);
        // Create a new Semester object
        Semester semester;
        sscanf(semesterDataPath->filename.path, "%hhu-%hhu", &semester.year, &semester.term);
        
        semester.classList = NULL;
        
        error classErr = LoadClassData(auth, semesterDataPath, &semester);
        if (classErr != NULL) {
            LogMsg("Warning: Some class data could not be loaded");
            Error(classErr);
            if (semester.classList == NULL) {
                continue;
            }
        }

        if (auth->userData->leb2->semesterList == NULL) {
            err = createSemesterListNode(&auth->userData->leb2->semesterList, semester);
            if (err != NULL) {
                // LogMsg("Failed to create SemesterList node");
                // Error(err);
                FreeSemesterContents(&semester);
                continue;
            }
        } else {
            err = addSemesterToList(&auth->userData->leb2->semesterList, semester);
            if (err != NULL) {
                // LogMsg("Failed to add semester to list");
                // Error(err);
                FreeSemesterContents(&semester);
                continue;
            }
        }
        
        semestersLoaded = 1;
    }
    
    if (semestersLoaded == 0) {
        return "No semester data was loaded";
    }
    
    return NULL;
}

error LoadClassData(Auth* auth, DataPath* semesterDataPath, Semester* semester) {
    if (auth == NULL || semesterDataPath == NULL || semester == NULL) {
        return "Invalid parameters in LoadClassData";
    }
    
    error err = NULL;
    code classesLoaded = 0;
    
    // Process each class directory in this semester
    for (uint16 j = 0; j < semesterDataPath->sizeDir; j++) {
        DataPath* classDataPath = semesterDataPath->Dir[j];
        if (classDataPath == NULL) continue;
        // LogMsg("Loading Class ");
        // printf("%s\n", classDataPath->filename.path);
        
        Class class;
        class.classId = NULL;
        err = allocateString(&class.classId, classDataPath->filename.path);
        if (err != NULL) {
            // LogMsg("Failed to allocate class ID string");
            // Error(err);
            continue; // Skip this class and try the next one
        }
        
        // Initialize all class components to NULL
        class.dashboard = NULL;
        class.syllabus = NULL;
        class.assignmentList = NULL;
        class.learningActivityList = NULL;
        class.attendanceList = NULL;
        class.scoreBookList = NULL;
        class.learnItList = NULL;
        class.fileList = NULL;
        class.survey = NULL;
        class.memberList = NULL;
        
        // Only allocate the AssignmentList component for now
        err = allocateAssignmentList(&class.assignmentList);
        if (err != NULL) {
            // LogMsg("Failed to allocate AssignmentList");
            // Error(err);
            FreeString(&class.classId);
            continue; // Skip this class and try the next one
        }
        
        // Load assignment data for this class
        error assignmentErr = LoadAssignment(classDataPath, &class);
        if (assignmentErr != NULL) {
            LogMsg("Warning: Assignment data could not be loaded");
            Error(assignmentErr);
            // We continue anyway, even with no assignments
        }
        
        // Add the class to the semester's classList
        if (semester->classList == NULL) {
            // Create the first node in the class list
            err = createClassListNode(&semester->classList, class);
            if (err != NULL) {
                // LogMsg("Failed to create ClassList node");
                // Error(err);
                if (class.assignmentList != NULL) FreeAssignmentList(class.assignmentList);
                FreeString(&class.classId);
                continue; // Skip this class and try the next one
            }
        } else {
            // Add to existing class list
            err = addClassToList(&semester->classList, class);
            if (err != NULL) {
                // LogMsg("Failed to add class to list");
                // Error(err);
                if (class.assignmentList != NULL) FreeAssignmentList(class.assignmentList);
                FreeString(&class.classId);
                continue; // Skip this class and try the next one
            }
        }
        
        classesLoaded = 1;
    }
    
    // Return an error if no classes were loaded
    if (classesLoaded == 0) {
        return "No class data was loaded";
    }
    
    return NULL; // Success, at least one class was loaded
}

// Function to load assignment data
error LoadAssignment(DataPath* classDataPath, Class* class) {
    if (classDataPath == NULL || class == NULL) {
        return "Invalid parameters in LoadAssignment";
    }
    
    error err = NULL;
    
    // Find AssignmentActivity directory
    DataPath* assignmentDirPath = NULL;
    err = findDataPathByFilename(classDataPath, "AssignmentActivity", &assignmentDirPath);
    if (err != NULL) {
        // LogMsg("Failed to find AssignmentActivity directory");
        // Error(err);
        return "AssignmentActivity directory not found"; // A real but non-critical error
    }
    
    if (assignmentDirPath == NULL) {
        return "AssignmentActivity directory not found";
    }
    
    // Get assignment directory contents
    err = getDirDataPath(assignmentDirPath);
    if (err != NULL) {
        // LogMsg("Failed to get AssignmentActivity directory contents");
        // Error(err);
        return "Failed to read assignment directory contents";
    }
    
    code assignmentsLoaded = 0;
    
    // Process each file in the AssignmentActivity directory
    for (uint16 k = 0; k < assignmentDirPath->sizeDir; k++) {
        DataPath* assignmentFilePath = assignmentDirPath->Dir[k];
        if (assignmentFilePath == NULL) continue;
        
        // LogMsg("Loading Assignment ");
        // printf("%s\n", assignmentFilePath->filename.path);

        // Skip folders, we only want files
        if (assignmentFilePath->isFolder == 1) continue;
        
        // Create a new Assignment object
        Assignment assignment;
        assignment.head = NULL;
        assignment.description = NULL;
        
        // Use filename as assignment title
        err = allocateString(&assignment.head, assignmentFilePath->filename.path);
        if (err != NULL) {
            // LogMsg("Failed to allocate assignment head");
            // Error(err);
            continue; // Continue to next assignment if this one fails
        }
        
        // Create a simple description using the filename
        err = allocateString(&assignment.description, "Assignment from file");
        if (err != NULL) {
            // LogMsg("Failed to allocate assignment description");
            // Error(err);
            FreeString(&assignment.head);
            continue; // Continue to next assignment if this one fails
        }
        
        // Set assignment dates
        DateTime currentTime;
        err = getCurrentDateTime(&currentTime);
        if (err != NULL) {
            // LogMsg("Failed to get current datetime");
            // Error(err);
            FreeString(&assignment.description);
            FreeString(&assignment.head);
            continue; // Continue to next assignment if this one fails
        }
        
        // Set dates (assign date as current time, due date as a week later)
        assignment.assignDate = currentTime;
        assignment.dueDate = currentTime + 604800; // Due in 1 week
        
        // Add the assignment to the class's assignment list
        err = addAssignmentToList(&class->assignmentList, assignment);
        if (err != NULL) {
            // LogMsg("Failed to add assignment to list");
            // Error(err);
            FreeString(&assignment.description);
            FreeString(&assignment.head);
            continue; // Continue to next assignment if this one fails
        }
        
        // Free temporary strings (they've been copied in addAssignmentToList)
        FreeString(&assignment.head);
        FreeString(&assignment.description);
        
        assignmentsLoaded = 1;
    }
    
    // Return a message if no assignments were loaded, but this isn't a critical error
    if (assignmentsLoaded == 0) {
        return "No assignment data was loaded";
    }
    
    return NULL; // Success, at least one assignment was loaded
}