#include "../update-db.h"

#include <stdio.h>
#include <stdlib.h>

Status SetUpDataBase(Auth* auth) {
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

    Path DBPath;
    initPath(&DBPath);
    err = createFolderPathLen(&DBPath, "DB", currentPath, 2);
    if (err != NULL) {
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create DBPath", "Failed to create DBPath");
    }
    
    err = MakeFolderByPath(DBPath.path);
    if (err != NULL) {
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create DB folder", err);
    }
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
    err = MakeFolderByPath(studentIdPath.path);
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create student ID folder", err);
    }
    
    Path notification;
    initPath(&notification);
    err = createPathLen(&notification, "Notification", 12);
    if (err != NULL) {
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Notification path", err);
    }

    Path notificationPath;
    initPath(&notificationPath);
    err = createDirPath(&notificationPath, notification, studentIdPath);
    if (err != NULL) {
        FreePathContent(&notification);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create NotificationPath path", err);
    }
    err = MakeFolderByPath(notificationPath.path);
    if (err != NULL) {
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Notification folder", err);
    }
    
    Path calendar;
    initPath(&calendar);
    err = createPathLen(&calendar, "Calendar", 8);
    if (err != NULL) {
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Calendar path", err);
    }

    Path calendarPath;
    initPath(&calendarPath);
    err = createDirPath(&calendarPath, calendar, studentIdPath); 
    if (err != NULL) {
        FreePathContent(&calendar);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create CalendarPath path", err);
    }
    
    err = MakeFolderByPath(calendarPath.path);
    if (err != NULL) {
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create Calendar folder", "Failed to create Calendar folder");
    }
    Path leb2;
    initPath(&leb2);
    err = createPathLen(&leb2, "LEB2", 4);
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
    err = MakeFolderByPath(leb2Path.path);
    if (err != NULL) {
        FreePathContent(&leb2Path);
        FreePathContent(&calendarPath);
        FreePathContent(&notificationPath);
        FreePathContent(&studentIdPath);
        FreePathContent(&DBPath);
        FreePathContent(&currentPath);
        return SetStatus(0, "Failed to create LEB2 folder", "Failed to create LEB2 folder");
    }
    
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

Status MakeTempData(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    
    if (auth->dataPath == NULL) {
        return SetStatus(0, "DataPath is NULL", "Invalid data path");
    }
    
    DataPath* leb2DataPath = NULL;
    for (uint16 i = 0; i < auth->dataPath->sizeDir; i++) {
        if (auth->dataPath->Dir[i] != NULL && 
            stringCmp(auth->dataPath->Dir[i]->filename.path, "LEB2")) {
            leb2DataPath = auth->dataPath->Dir[i];
            break;
        }
    }
    
    if (leb2DataPath == NULL) {
        return SetStatus(0, "LEB2 folder not found", "LEB2 folder not found in dataPath");
    }
    
    error err = NULL;
    
    const string semesters[] = {"1-1", "1-2"};
    const uint16 numSemesters = 2;
    
    const string semester1Classes[] = {"MTH101", "CPE101"};
    const string semester2Classes[] = {"MTH102", "CPE112"};
    const uint16 numClasses = 2;
    
    const string ClassComponents[] = {
        "Dashboard", "Syllabus", "AssignmentActivity", "LearningActivity",
        "Attendance", "ScoreBook", "LearnIt", "File",
        "Survey", "Member"
    };
    const uint16 numClassComponents = 10;
    
    for (int s = 0; s < numSemesters; s++) {
        Path semPath, semName;
        initPath(&semPath);
        initPath(&semName);
        
        err = createPath(&semName, semesters[s]);
        if (err != NULL) {
            return SetStatus(0, "Failed to create semester path", err);
        }
        
        err = createDirPath(&semPath, semName, leb2DataPath->path);
        if (err != NULL) {
            FreePathContent(&semName);
            return SetStatus(0, "Failed to create semester directory path", err);
        }
        
        err = MakeFolderByPath(semPath.path);
        if (err != NULL) {
            FreePathContent(&semPath);
            FreePathContent(&semName);
            return SetStatus(0, "Failed to create semester folder", err);
        }
        
        DataPath* semDataPath = NULL;
        err = allocateDataPath(&semDataPath);
        if (err != NULL) {
            FreePathContent(&semPath);
            FreePathContent(&semName);
            return SetStatus(0, "Failed to allocate semester DataPath", err);
        }
        
        initDataPath(semDataPath);
        
        err = createDataPath(semDataPath, semPath, semName, 1);
        if (err != NULL) {
            FreeDataPath(semDataPath);
            FreePathContent(&semPath);
            FreePathContent(&semName);
            return SetStatus(0, "Failed to create semester DataPath", err);
        }
        
        err = addChildDataPath(leb2DataPath, semDataPath);
        if (err != NULL) {
            FreeDataPath(semDataPath);
            FreePathContent(&semPath);
            FreePathContent(&semName);
            return SetStatus(0, "Failed to add semester DataPath as child", err);
        }
        
        const string* classes = (s == 0) ? semester1Classes : semester2Classes;
        
        for (uint16 c = 0; c < numClasses; c++) {
            Path classPath, className;
            initPath(&classPath);
            initPath(&className);
            
            err = createPath(&className, classes[c]);
            if (err != NULL) {
                LogMsg("Failed to create class path");
                continue;
            }
            
            err = createDirPath(&classPath, className, semPath);
            if (err != NULL) {
                FreePathContent(&className);
                LogMsg("Failed to create class directory path");
                continue;
            }
            
            err = MakeFolderByPath(classPath.path);
            if (err != NULL) {
                FreePathContent(&classPath);
                FreePathContent(&className);
                LogMsg("Failed to create class folder");
                continue;
            }
            
            DataPath* classDataPath = NULL;
            err = allocateDataPath(&classDataPath);
            if (err != NULL) {
                FreePathContent(&classPath);
                FreePathContent(&className);
                LogMsg("Failed to allocate class DataPath");
                continue;
            }
            
            initDataPath(classDataPath);
            
            err = createDataPath(classDataPath, classPath, className, 1);
            if (err != NULL) {
                FreeDataPath(classDataPath);
                FreePathContent(&classPath);
                FreePathContent(&className);
                LogMsg("Failed to create class DataPath");
                continue;
            }
            
            err = addChildDataPath(semDataPath, classDataPath);
            if (err != NULL) {
                FreeDataPath(classDataPath);
                FreePathContent(&classPath);
                FreePathContent(&className);
                LogMsg("Failed to add class DataPath as child");
                continue;
            }
            
            for (uint16 cmp = 0; cmp < numClassComponents; cmp++) {
                Path compPath, compName;
                initPath(&compPath);
                initPath(&compName);
                
                err = createPath(&compName, ClassComponents[cmp]);
                if (err != NULL) {
                    LogMsg("Failed to create component path");
                    continue;
                }
                
                err = createDirPath(&compPath, compName, classPath);
                if (err != NULL) {
                    FreePathContent(&compName);
                    LogMsg("Failed to create component directory path");
                    continue;
                }
                
                err = MakeFolderByPath(compPath.path);
                if (err != NULL) {
                    FreePathContent(&compPath);
                    FreePathContent(&compName);
                    LogMsg("Failed to create component folder");
                    continue;
                }
                
                DataPath* compDataPath = NULL;
                err = allocateDataPath(&compDataPath);
                if (err != NULL) {
                    FreePathContent(&compPath);
                    FreePathContent(&compName);
                    LogMsg("Failed to allocate component DataPath");
                    continue;
                }
                
                initDataPath(compDataPath);
                
                err = createDataPath(compDataPath, compPath, compName, 1);
                if (err != NULL) {
                    FreeDataPath(compDataPath);
                    FreePathContent(&compPath);
                    FreePathContent(&compName);
                    LogMsg("Failed to create component DataPath");
                    continue;
                }
                
                err = addChildDataPath(classDataPath, compDataPath);
                if (err != NULL) {
                    FreeDataPath(compDataPath);
                    FreePathContent(&compPath);
                    FreePathContent(&compName);
                    LogMsg("Failed to add component DataPath as child");
                    continue;
                }
                FreePathContent(&compPath);
                FreePathContent(&compName);
            }
            
            FreePathContent(&classPath);
            FreePathContent(&className);
        }
        FreePathContent(&semPath);
        FreePathContent(&semName);
    }
    
    return SetStatus(1, "LEB2 temporary data created successfully", NULL);
}