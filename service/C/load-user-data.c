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
                err = NULL;
                continue;
            }
        }
    }

    return SetStatus(1, "User data path loaded successfully", NULL);
}

Status LoadLEB2Data(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    
    if (auth->dataPath == NULL) {
        return SetStatus(0, "DataPath is NULL", "Invalid data path");
    }
    
    error err = NULL;
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
        FreeLEB2(auth->userData->leb2);
        auth->userData->leb2 = NULL;
    }
    
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
        
        Semester semester;
        sscanf(semesterDataPath->filename.path, "%hhu-%hhu", &semester.year, &semester.term);
        
        semester.classList = NULL;
        
        error classErr = LoadClassData(auth, semesterDataPath, &semester);
        if (classErr != NULL) {
            // LogMsg("Warning: Some class data could not be loaded");
            // Error(classErr);
            if (semester.classList == NULL) {
                continue;
            }
        }

        if (auth->userData->leb2->semesterList == NULL) {
            err = createSemesterListNode(&auth->userData->leb2->semesterList, semester);
            if (err != NULL) {
                FreeSemesterContents(&semester);
                continue;
            }
        } else {
            err = addSemesterToList(&auth->userData->leb2->semesterList, semester);
            if (err != NULL) {
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
        
        Class class;
        class.classId = NULL;
        err = allocateString(&class.classId, classDataPath->filename.path);
        if (err != NULL) {
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
        
        // Load all class components
        Status status = LoadAllClassData(auth, &class, classDataPath);
        if (status.code != 1) {
            // LogMsg("Warning: Not all class data could be loaded");
            // Error(status.msg);
        }
        
        if (semester->classList == NULL) {
            err = createClassListNode(&semester->classList, class);
            if (err != NULL) {
                FreeClassContents(&class);
                continue;
            }
        } else {
            err = addClassToList(&semester->classList, class);
            if (err != NULL) {
                FreeClassContents(&class);
                continue;
            }
        }
        
        classesLoaded = 1;
    }
    
    if (classesLoaded == 0) {
        return "No class data was loaded";
    }
    
    return NULL;
}

Status LoadLearningActivities(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;

    DataPath* learningActDirPath = NULL;
    err = findDataPathByFilename(classDataPath, "LearningActivity", &learningActDirPath);
    if (err != NULL) {
        return SetStatus(0, "LearningActivity directory not found", err);
    }
    
    if (learningActDirPath == NULL) {
        return SetStatus(0, "LearningActivity directory not found", "Directory pointer is NULL");
    }

    err = getDirDataPath(learningActDirPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to read learning activity directory contents", err);
    }
    
    // Initialize learning activity list
    if (class->learningActivityList == NULL) {
        err = allocateLearningActivityList(&class->learningActivityList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate learning activity list", err);
        }
    }
    
    code activitiesLoaded = 0;
    
    for (uint16 i = 0; i < learningActDirPath->sizeDir; i++) {
        DataPath* activityFilePath = learningActDirPath->Dir[i];
        if (activityFilePath == NULL) continue;
    
        if (activityFilePath->isFolder == 1) continue;
        
        if (!stringEndsWith(activityFilePath->filename.path, ".csv")) continue;
        
        LearningActivity activity;
        activity.head = NULL;
        activity.description = NULL;
        activity.url = NULL;
        activity.file = NULL;
        
        uint32 filenameLen = stringLen(activityFilePath->filename.path);
        uint32 titleLen = filenameLen - 4;
        
        string titleBuffer = (string)malloc(titleLen + 1);
        if (titleBuffer == NULL) {
            continue;
        }
        
        stringCopyN(titleBuffer, activityFilePath->filename.path, titleLen);
        titleBuffer[titleLen] = '\0';
        
        err = allocateString(&activity.head, titleBuffer);
        free(titleBuffer);
        
        if (err != NULL) {
            continue;
        }
        
        textline* lines = NULL;
        err = readFileToTextLinesPath(&lines, activityFilePath->path);
        if (err != NULL || lines == NULL) {
            FreeString(&activity.head);
            continue;
        }
        
        textline* currentLine = lines;
        uint8 lineCount = 0;
        string description = NULL;
        
        while (currentLine != NULL) {
            lineCount++;
            
            if (lineCount == 1) {
                if (stringLen(currentLine->line) > 0) {
                    err = allocateString(&activity.url, currentLine->line);
                    if (err != NULL) {
                        break;
                    }
                }
            } 
            else {
                if (description == NULL) {
                    err = allocateString(&description, currentLine->line);
                    if (err != NULL) {
                        break;
                    }
                } 
                else {
                    string tempDesc = NULL;
                    err = mergeTwoStrings(&tempDesc, description, "\n");
                    if (err != NULL) {
                        break;
                    }
                    
                    string updatedDesc = NULL;
                    err = mergeTwoStrings(&updatedDesc, tempDesc, currentLine->line);
                    if (err != NULL) {
                        FreeString(&tempDesc);
                        break;
                    }
                    
                    FreeString(&description);
                    FreeString(&tempDesc);
                    description = updatedDesc;
                }
            }
            
            currentLine = currentLine->nextline;
        }
        
        // Clean up the lines
        FreeTextLine(lines);
        
        // Skip this activity if we had errors processing the file
        if (err != NULL) {
            if (description != NULL) {
                FreeString(&description);
            }
            if (activity.url != NULL) {
                FreeString(&activity.url);
            }
            FreeString(&activity.head);
            continue;
        }
        
        // Set the description (or use a default if none was found)
        if (description != NULL) {
            activity.description = description;
        } else {
            err = allocateString(&activity.description, "No description available");
            if (err != NULL) {
                if (activity.url != NULL) {
                    FreeString(&activity.url);
                }
                FreeString(&activity.head);
                continue;
            }
        }
        
        // Add the learning activity to the class's list
        err = addLearningActivityToList(&class->learningActivityList, activity);
        if (err != NULL) {
            if (activity.description != NULL) {
                FreeString(&activity.description);
            }
            if (activity.url != NULL) {
                FreeString(&activity.url);
            }
            FreeString(&activity.head);
            continue;
        }
        
        activitiesLoaded = 1;
    }
    
    if (activitiesLoaded == 0) {
        return SetStatus(0, "No learning activity data was loaded", NULL);
    }
    
    return SetStatus(1, "Learning activities loaded successfully", NULL);
}

Status LoadAttendance(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    // Find Attendance directory
    DataPath* attendanceDirPath = NULL;
    err = findDataPathByFilename(classDataPath, "Attendance", &attendanceDirPath);
    if (err != NULL) {
        return SetStatus(0, "Attendance directory not found", err);
    }
    
    if (attendanceDirPath == NULL) {
        return SetStatus(0, "Attendance directory not found", "Directory pointer is NULL");
    }
    
    // Get attendance directory contents
    err = getDirDataPath(attendanceDirPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to read attendance directory contents", err);
    }
    
    // Initialize attendance list
    if (class->attendanceList == NULL) {
        err = allocateAttendanceList(&class->attendanceList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate attendance list", err);
        }
    }
    
    code attendancesLoaded = 0;
    
    // Process each file in the Attendance directory
    for (uint16 i = 0; i < attendanceDirPath->sizeDir; i++) {
        DataPath* attendanceFilePath = attendanceDirPath->Dir[i];
        if (attendanceFilePath == NULL) continue;
        
        // Skip folders, we only want files
        if (attendanceFilePath->isFolder == 1) continue;
        
        // Only process CSV files
        if (!stringEndsWith(attendanceFilePath->filename.path, ".csv")) continue;
        
        // Read the CSV file content
        textline* lines = NULL;
        err = readFileToTextLinesPath(&lines, attendanceFilePath->path);
        if (err != NULL || lines == NULL) {
            continue; // Continue to next attendance if this one fails
        }
        
        // Process the file - each line is a separate attendance record
        textline* currentLine = lines;
        
        while (currentLine != NULL) {
            // Create a new Attendance object
            Attendance attendance;
            
            // Parse the line format: YYYY-MM-DD HH:MM:SS,code
            string comma = strchr(currentLine->line, ',');
            if (comma == NULL) {
                currentLine = currentLine->nextline;
                continue; // Invalid format, skip this line
            }
            
            // Split the line into date and code
            *comma = '\0'; // Temporarily split the string at the comma
            string dateStr = currentLine->line;
            string codeStr = comma + 1;
            
            // Parse the date
            DateTime date;
            err = stringToDateTime(&date, dateStr);
            if (err != NULL) {
                *comma = ','; // Restore the comma
                currentLine = currentLine->nextline;
                continue; // Invalid date format, skip this line
            }
            
            // Parse the code (1 for present, 0 for absent)
            code attendanceCode = atoi(codeStr);
            
            // Restore the comma
            *comma = ',';
            
            // Set attendance data
            attendance.date = date;
            attendance.joined = attendanceCode;
            
            // Add the attendance to the class's list
            err = addAttendanceToList(&class->attendanceList, attendance);
            if (err != NULL) {
                currentLine = currentLine->nextline;
                continue; // Failed to add, skip this line
            }
            
            attendancesLoaded = 1;
            currentLine = currentLine->nextline;
        }
        
        // Clean up the lines
        FreeTextLine(lines);
    }
    
    if (attendancesLoaded == 0) {
        return SetStatus(0, "No attendance data was loaded", NULL);
    }
    
    return SetStatus(1, "Attendance records loaded successfully", NULL);
}

Status LoadScoreBook(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    // Find ScoreBook directory
    DataPath* scoreBookDirPath = NULL;
    err = findDataPathByFilename(classDataPath, "ScoreBook", &scoreBookDirPath);
    if (err != NULL) {
        return SetStatus(0, "ScoreBook directory not found", err);
    }
    
    if (scoreBookDirPath == NULL) {
        return SetStatus(0, "ScoreBook directory not found", "Directory pointer is NULL");
    }
    
    // Get scorebook directory contents
    err = getDirDataPath(scoreBookDirPath);
    if (err != NULL) {
        return SetStatus(0, "Failed to read scorebook directory contents", err);
    }
    
    // Initialize scorebook list
    if (class->scoreBookList == NULL) {
        err = allocateScoreBookList(&class->scoreBookList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate scorebook list", err);
        }
    }
    
    code scoresLoaded = 0;
    
    // Process each file in the ScoreBook directory
    for (uint16 i = 0; i < scoreBookDirPath->sizeDir; i++) {
        DataPath* scoreFilePath = scoreBookDirPath->Dir[i];
        if (scoreFilePath == NULL) continue;
        
        // Skip folders, we only want files
        if (scoreFilePath->isFolder == 1) continue;
        
        // Only process CSV files
        if (!stringEndsWith(scoreFilePath->filename.path, ".csv")) continue;
        
        // Create a new ScoreBook object
        ScoreBook scoreBook;
        scoreBook.head = NULL;
        scoreBook.description = NULL;
        
        // Use filename without .csv extension as title
        uint32 filenameLen = stringLen(scoreFilePath->filename.path);
        uint32 titleLen = filenameLen - 4; // Remove .csv
        
        string titleBuffer = (string)malloc(titleLen + 1);
        if (titleBuffer == NULL) {
            continue; // Memory allocation failed
        }
        
        stringCopyN(titleBuffer, scoreFilePath->filename.path, titleLen);
        titleBuffer[titleLen] = '\0';
        
        err = allocateString(&scoreBook.head, titleBuffer);
        free(titleBuffer);
        
        if (err != NULL) {
            continue; // Continue to next score if this one fails
        }
        
        // Read the CSV file content
        textline* lines = NULL;
        err = readFileToTextLinesPath(&lines, scoreFilePath->path);
        if (err != NULL || lines == NULL) {
            FreeString(&scoreBook.head);
            continue; // Continue to next score if this one fails
        }
        
        // Process the CSV content
        textline* currentLine = lines;
        uint8 lineCount = 0;
        string description = NULL;
        
        while (currentLine != NULL) {
            lineCount++;
            
            if (lineCount == 1) {
                // First line is the score value
                scoreBook.score = (float32)atof(currentLine->line);
            } 
            else {
                // Second line and beyond form the description
                if (description == NULL) {
                    // First description line
                    err = allocateString(&description, currentLine->line);
                    if (err != NULL) {
                        break; // Memory allocation failed
                    }
                } 
                else {
                    // Append subsequent lines with a newline
                    string tempDesc = NULL;
                    err = mergeTwoStrings(&tempDesc, description, "\n");
                    if (err != NULL) {
                        break;
                    }
                    
                    string updatedDesc = NULL;
                    err = mergeTwoStrings(&updatedDesc, tempDesc, currentLine->line);
                    if (err != NULL) {
                        FreeString(&tempDesc);
                        break;
                    }
                    
                    FreeString(&description);
                    FreeString(&tempDesc);
                    description = updatedDesc;
                }
            }
            
            currentLine = currentLine->nextline;
        }
        
        // Clean up the lines
        FreeTextLine(lines);
        
        // Skip this score if we had errors processing the file
        if (err != NULL) {
            if (description != NULL) {
                FreeString(&description);
            }
            FreeString(&scoreBook.head);
            continue;
        }
        
        // Set the description (or use a default if none was found)
        if (description != NULL) {
            scoreBook.description = description;
        } else {
            err = allocateString(&scoreBook.description, "No description available");
            if (err != NULL) {
                FreeString(&scoreBook.head);
                continue;
            }
        }
        
        // Add the score to the class's list
        err = addScoreBookToList(&class->scoreBookList, scoreBook);
        if (err != NULL) {
            FreeString(&scoreBook.description);
            FreeString(&scoreBook.head);
            continue;
        }
        
        scoresLoaded = 1;
    }
    
    if (scoresLoaded == 0) {
        return SetStatus(0, "No scorebook data was loaded", NULL);
    }
    
    return SetStatus(1, "Scorebook data loaded successfully", NULL);
}

Status LoadAllClassData(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    Status status;
    error err = NULL;
    
    // Initialize all components that aren't already initialized
    if (class->dashboard == NULL) {
        err = allocateDashboard(&class->dashboard);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate dashboard", err);
        }
        
        // Create a default dashboard description
        err = createDashboard(class->dashboard, "No dashboard available");
        if (err != NULL) {
            return SetStatus(0, "Failed to create dashboard", err);
        }
    }
    
    if (class->syllabus == NULL) {
        err = allocateSyllabus(&class->syllabus);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate syllabus", err);
        }
        
        // Create a default syllabus description
        err = createSyllabus(class->syllabus, "No syllabus available");
        if (err != NULL) {
            return SetStatus(0, "Failed to create syllabus", err);
        }
    }
    
    // Load assignments
    if (class->assignmentList == NULL) {
        err = allocateAssignmentList(&class->assignmentList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate assignment list", err);
        }
    }
    
    err = LoadAssignment(classDataPath, class);
    if (err != NULL) {
        // LogMsg("Warning: Assignment loading had issues");
        // Error(err);
    }
    
    status = LoadLearningActivities(auth, class, classDataPath);
    if (status.code != 1) {
        // LogMsg("Warning: Learning activities loading had issues");
        // Error(status.msg);
    }
    
    // Load attendance
    status = LoadAttendance(auth, class, classDataPath);
    if (status.code != 1) {
        // LogMsg("Warning: Attendance loading had issues");
        // Error(status.msg);
    }
    
    status = LoadScoreBook(auth, class, classDataPath);
    if (status.code != 1) {
        // LogMsg("Warning: Scorebook loading had issues");
        // Error(status.msg);
    }
    
    if (class->learnItList == NULL) {
        err = allocateLearnItList(&class->learnItList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate LearnIt list", err);
        }
    }
    
    if (class->fileList == NULL) {
        err = allocateFileList(&class->fileList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate file list", err);
        }
    }
    
    if (class->survey == NULL) {
        err = allocateSurvey(&class->survey);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate survey", err);
        }
    }
    
    if (class->memberList == NULL) {
        err = allocateMemberList(&class->memberList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate member list", err);
        }
    }
    
    return SetStatus(1, "All class data loaded successfully", NULL);
}

error LoadAssignment(DataPath* classDataPath, Class* class) {
    if (classDataPath == NULL || class == NULL) {
        return "Invalid parameters in LoadAssignment";
    }
    
    error err = NULL;
    
    // Find AssignmentActivity directory
    DataPath* assignmentDirPath = NULL;
    err = findDataPathByFilename(classDataPath, "AssignmentActivity", &assignmentDirPath);
    if (err != NULL) {
        return "AssignmentActivity directory not found";
    }
    
    if (assignmentDirPath == NULL) {
        return "AssignmentActivity directory not found";
    }
    
    // Get assignment directory contents
    err = getDirDataPath(assignmentDirPath);
    if (err != NULL) {
        return "Failed to read assignment directory contents";
    }
    
    code assignmentsLoaded = 0;
    
    // Process each file in the AssignmentActivity directory
    for (uint16 k = 0; k < assignmentDirPath->sizeDir; k++) {
        DataPath* assignmentFilePath = assignmentDirPath->Dir[k];
        if (assignmentFilePath == NULL) continue;
        
        if (assignmentFilePath->isFolder == 1) continue;
        
        if (!stringEndsWith(assignmentFilePath->filename.path, ".csv")) continue;
        // printf("Loading Assignment %s\n", assignmentFilePath->filename.path);
        Assignment assignment;
        assignment.head = NULL;
        assignment.description = NULL;
        
        // Use filename without .csv extension as assignment title
        uint32 filenameLen = stringLen(assignmentFilePath->filename.path);
        uint32 titleLen = filenameLen - 4; // Remove .csv
        
        string titleBuffer = (string)malloc(titleLen + 1);
        if (titleBuffer == NULL) {
            continue; // Memory allocation failed
        }
        stringCopyN(titleBuffer, assignmentFilePath->filename.path, titleLen);
        titleBuffer[titleLen] = '\0';
        
        err = allocateString(&assignment.head, titleBuffer);
        free(titleBuffer);
        if (err != NULL) {
            continue; // Continue to next assignment if this one fails
        }
        
        // Read the CSV file content
        textline* lines = NULL;
        err = readFileToTextLinesPath(&lines, assignmentFilePath->path);
        if (err != NULL || lines == NULL) {
            FreeString(&assignment.head);
            continue; // Continue to next assignment if this one fails
        }
        
        // Process the CSV content
        textline* currentLine = lines;
        uint8 lineCount = 0;
        string description = NULL;
        
        while (currentLine != NULL) {
            lineCount++;
            
            if (lineCount == 1) {
                // First line is the assign date
                err = stringToDateTime(&assignment.assignDate, currentLine->line);
                if (err != NULL) {
                    LogMsg("Failed to parse assignment date");
                    Error(err);
                    break; // Invalid date format, skip this file
                }
            } 
            else if (lineCount == 2) {
                // Second line is the due date
                err = stringToDateTime(&assignment.dueDate, currentLine->line);
                if (err != NULL) {
                    LogMsg("Failed to parse due date");
                    Error(err);
                    break; // Invalid date format, skip this file
                }
            } 
            else {
                if (description == NULL) {
                    err = allocateString(&description, currentLine->line);
                    if (err != NULL) {
                        LogMsg("Failed to allocate description");
                        Error(err);
                        break;
                    }
                } 
                else {
                    string tempDesc = NULL;
                    err = mergeTwoStrings(&tempDesc, description, "\n");
                    if (err != NULL) {
                        LogMsg("Failed to append newline");
                        Error(err);
                        break;
                    }
                    
                    string updatedDesc = NULL;
                    err = mergeTwoStrings(&updatedDesc, tempDesc, currentLine->line);
                    if (err != NULL) {
                        LogMsg("Failed to append line to description");
                        Error(err);
                        FreeString(&tempDesc);
                        break;
                    }
                    
                    FreeString(&description);
                    FreeString(&tempDesc);
                    description = updatedDesc;
                }
            }
            
            currentLine = currentLine->nextline;
        }
        
        FreeTextLine(lines);
        
        if (err != NULL) {
            if (description != NULL) {
                FreeString(&description);
            }
            FreeString(&assignment.head);
            continue;
        }
        
        // Set the description (or use a default if none was found)
        if (description != NULL) {
            assignment.description = description;
        } else {
            err = allocateString(&assignment.description, "No description available");
            if (err != NULL) {
                LogMsg("Failed to allocate default description");
                Error(err);
                FreeString(&assignment.head);
                continue;
            }
        }
        
        // Add the assignment to the class's assignment list
        err = addAssignmentToList(&class->assignmentList, assignment);
        if (err != NULL) {
            LogMsg("Failed to add assignment to list");
            Error(err);
            FreeString(&assignment.description);
            FreeString(&assignment.head);
            continue;
        }
        
        // Don't free the strings after adding to the list - they're now owned by the list
        
        assignmentsLoaded = 1;
    }
    
    // Return a message if no assignments were loaded, but this isn't a critical error
    if (assignmentsLoaded == 0) {
        return "No assignment data was loaded";
    }
    
    return NULL; // Success, at least one assignment was loaded
}