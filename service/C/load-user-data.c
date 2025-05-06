#include "../load-user-data.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status LoadUserDataAPI(Auth* auth) {
    if (auth == NULL) {
        return SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer");
    }
    if (auth->userData != NULL) {
        FreeUserData(auth->userData);
        auth->userData = NULL;
    }
    auth->userData = (UserData*)malloc(sizeof(UserData));
    if (auth->userData == NULL) {
        return SetStatus(0, "Memory allocation failed", "Can't Allocate UserData");
    }
    auth->userData->notificationList = NULL;
    auth->userData->calendarList = NULL;
    auth->userData->leb2 = NULL;

    Status status = LoadLEB2Data(auth);
    if (status.code != 1) {
        Error(status.msg);
        FreeUserData(auth->userData);
        auth->userData = NULL;
        return status;
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
    
    auth->userData->leb2->semesterList = NULL;
    
    err = LoadSemesterData(auth, leb2DataPath);
    if (err != NULL) {
        if (auth->userData->leb2->semesterList == NULL) {
            return SetStatus(0, "Failed to load any semester data", err);
        } else {
            LogMsg("Warning: Some semester data could not be loaded");
            Error(err);
        }
    }
    
    if (auth->userData->leb2->semesterList == NULL) {
        return SetStatus(0, "No semesters found", "LEB2 directory contains no semesters");
    }
    
    return SetStatus(1, "LEB2 data loaded successfully", NULL);
}

error LoadSemesterData(Auth* auth, DataPath* leb2DataPath) {
    if (auth == NULL || leb2DataPath == NULL) {
        return "Invalid parameters in LoadSemesterData";
    }
    
    error err = NULL;
    code semestersLoaded = 0;
    
    for (uint16 i = 0; i < leb2DataPath->sizeDir; i++) {
        DataPath* semesterDataPath = leb2DataPath->Dir[i];
        if (semesterDataPath == NULL) continue;
        
        Semester semester;
        sscanf(semesterDataPath->filename.path, "%hhu-%hhu", &semester.year, &semester.term);
        
        semester.classList = NULL;
        
        error classErr = LoadClassData(auth, semesterDataPath, &semester);
        if (classErr != NULL) {
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
    
    for (uint16 j = 0; j < semesterDataPath->sizeDir; j++) {
        DataPath* classDataPath = semesterDataPath->Dir[j];
        if (classDataPath == NULL) continue;
        
        Class class;
        class.classId = NULL;
        err = allocateString(&class.classId, classDataPath->filename.path);
        if (err != NULL) {
            continue;
        }
        
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
        
        Status status = LoadAllClassData(auth, &class, classDataPath);
        if (status.code != 1) {
            continue;
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

Status LoadAllClassData(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    Status status;

    status = LoadAssignment(auth, class, classDataPath);
    if (status.code != 1) {
        return status;
    }
    
    return SetStatus(1, "All class data loaded successfully", NULL);
}

Status LoadSurvey(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->survey != NULL) {
        FreeSurvey(class->survey);
        class->survey = NULL;
    }

    DataPath* surveyDir = NULL;
    err = findDataPathByFilename(classDataPath, "Survey", &surveyDir);
    if (err != NULL || surveyDir == NULL) {
        return SetStatus(1, "No Survey directory found", NULL);
    }
    
    err = getDirDataPath(surveyDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read Survey directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(surveyDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No survey datalist file found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No survey data to load", NULL);
    }

    err = allocateSurvey(&class->survey);
    if (err != NULL) {
        FreeTextLine(lines);
        return SetStatus(0, "Failed to allocate Survey", err);
    }

    err = createSurvey(class->survey);
    if (err != NULL) {
        FreeSurvey(class->survey);
        class->survey = NULL;
        FreeTextLine(lines);
        return SetStatus(0, "Failed to create survey", err);
    }

    textline* currentLine = lines;
    while (currentLine != NULL) {
        // Process each line of the survey data
        // (Add specific survey data parsing here when needed)
        
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);

    return SetStatus(1, "Survey loaded successfully", NULL);
}

Status LoadMember(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->memberList != NULL) {
        FreeMemberList(class->memberList);
        class->memberList = NULL;
    }

    DataPath* memberDir = NULL;
    err = findDataPathByFilename(classDataPath, "Member", &memberDir);
    if (err != NULL || memberDir == NULL) {
        return SetStatus(1, "No Member directory found", NULL);
    }
    err = getDirDataPath(memberDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read Member directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(memberDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No members data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No members data to load", NULL);
    }
    
    code membersLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        Member member;
        member.studentId = NULL;
        member.name = NULL;
        
        char studentId[32] = {0};
        char name[256] = {0};
        
        sscanf(currentLine->line, "%[^,],%[^\n]", studentId, name);
        
        err = allocateString(&member.studentId, studentId);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = allocateString(&member.name, name);
        if (err != NULL) {
            FreeString(&member.studentId);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = addMemberToList(&class->memberList, member);
        if (err != NULL) {
            FreeMemberContents(&member);
            currentLine = currentLine->nextline;
            continue;
        }
        
        membersLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (membersLoaded == 0) {
        return SetStatus(1, "No members loaded", NULL);
    }
    
    return SetStatus(1, "Members loaded successfully", NULL);
}

Status LoadDashboard(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->dashboard != NULL) {
        FreeDashboard(class->dashboard);
        class->dashboard = NULL;
    }

    DataPath* dashboardDir = NULL;
    err = findDataPathByFilename(classDataPath, "Dashboard", &dashboardDir);
    if (err != NULL || dashboardDir == NULL) {
        err = allocateDashboard(&class->dashboard);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate dashboard", err);
        }
        
        err = createDashboard(class->dashboard, "No dashboard available");
        if (err != NULL) {
            FreeDashboard(class->dashboard);
            class->dashboard = NULL;
            return SetStatus(0, "Failed to create default dashboard", err);
        }
        
        return SetStatus(1, "Created default dashboard", NULL);
    }
    err = getDirDataPath(dashboardDir);
    if (err != NULL) {
        err = allocateDashboard(&class->dashboard);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate dashboard", err);
        }
        
        err = createDashboard(class->dashboard, "No dashboard available");
        if (err != NULL) {
            FreeDashboard(class->dashboard);
            class->dashboard = NULL;
            return SetStatus(0, "Failed to create default dashboard", err);
        }
        
        return SetStatus(1, "Failed to read Dashboard directory, using default", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(dashboardDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        err = allocateDashboard(&class->dashboard);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate dashboard", err);
        }
        
        err = createDashboard(class->dashboard, "No dashboard available");
        if (err != NULL) {
            FreeDashboard(class->dashboard);
            class->dashboard = NULL;
            return SetStatus(0, "Failed to create default dashboard", err);
        }
        
        return SetStatus(1, "No dashboard data found, using default", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        err = allocateDashboard(&class->dashboard);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate dashboard", err);
        }
        
        err = createDashboard(class->dashboard, "No dashboard available");
        if (err != NULL) {
            FreeDashboard(class->dashboard);
            class->dashboard = NULL;
            return SetStatus(0, "Failed to create default dashboard", err);
        }
        
        return SetStatus(1, "No dashboard data to load, using default", NULL);
    }
    
    err = allocateDashboard(&class->dashboard);
    if (err != NULL) {
        FreeTextLine(lines);
        return SetStatus(0, "Failed to allocate dashboard", err);
    }
    
    char description[1024] = {0};
    if (lines != NULL && lines->line != NULL) {
        sscanf(lines->line, "%[^\n]", description);
    } else {
        strcpy(description, "No dashboard available");
    }
    
    err = createDashboard(class->dashboard, description);
    if (err != NULL) {
        FreeDashboard(class->dashboard);
        class->dashboard = NULL;
        FreeTextLine(lines);
        return SetStatus(0, "Failed to create dashboard", err);
    }
    
    FreeTextLine(lines);
    
    return SetStatus(1, "Dashboard loaded successfully", NULL);
}

Status LoadSyllabus(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->syllabus != NULL) {
        FreeSyllabus(class->syllabus);
        class->syllabus = NULL;
    }

    DataPath* syllabusDir = NULL;
    err = findDataPathByFilename(classDataPath, "Syllabus", &syllabusDir);
    if (err != NULL || syllabusDir == NULL) {
        err = allocateSyllabus(&class->syllabus);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate syllabus", err);
        }
        
        err = createSyllabus(class->syllabus, "No syllabus available");
        if (err != NULL) {
            FreeSyllabus(class->syllabus);
            class->syllabus = NULL;
            return SetStatus(0, "Failed to create default syllabus", err);
        }
        
        return SetStatus(1, "Created default syllabus", NULL);
    }
    
    err = getDirDataPath(syllabusDir);
    if (err != NULL) {
        err = allocateSyllabus(&class->syllabus);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate syllabus", err);
        }
        
        err = createSyllabus(class->syllabus, "No syllabus available");
        if (err != NULL) {
            FreeSyllabus(class->syllabus);
            class->syllabus = NULL;
            return SetStatus(0, "Failed to create default syllabus", err);
        }
        
        return SetStatus(1, "Failed to read Syllabus directory, using default", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(syllabusDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        err = allocateSyllabus(&class->syllabus);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate syllabus", err);
        }
        
        err = createSyllabus(class->syllabus, "No syllabus available");
        if (err != NULL) {
            FreeSyllabus(class->syllabus);
            class->syllabus = NULL;
            return SetStatus(0, "Failed to create default syllabus", err);
        }
        
        return SetStatus(1, "No syllabus data found, using default", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        err = allocateSyllabus(&class->syllabus);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate syllabus", err);
        }
        
        err = createSyllabus(class->syllabus, "No syllabus available");
        if (err != NULL) {
            FreeSyllabus(class->syllabus);
            class->syllabus = NULL;
            return SetStatus(0, "Failed to create default syllabus", err);
        }
        
        return SetStatus(1, "No syllabus data to load, using default", NULL);
    }
    
    err = allocateSyllabus(&class->syllabus);
    if (err != NULL) {
        FreeTextLine(lines);
        return SetStatus(0, "Failed to allocate syllabus", err);
    }
    
    char description[1024] = {0};
    if (lines != NULL && lines->line != NULL) {
        sscanf(lines->line, "%[^\n]", description);
    } else {
        strcpy(description, "No syllabus available");
    }
    
    err = createSyllabus(class->syllabus, description);
    if (err != NULL) {
        FreeSyllabus(class->syllabus);
        class->syllabus = NULL;
        FreeTextLine(lines);
        return SetStatus(0, "Failed to create syllabus", err);
    }
    
    FreeTextLine(lines);
    
    return SetStatus(1, "Syllabus loaded successfully", NULL);
}

Status LoadAssignment(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    if (class->assignmentList != NULL) {
        FreeAssignmentList(class->assignmentList);
        class->assignmentList = NULL;
    }

    DataPath* assignmentDir = NULL;
    err = findDataPathByFilename(classDataPath, "AssignmentActivity", &assignmentDir);
    if (err != NULL || assignmentDir == NULL) {
        return SetStatus(1, "No AssignmentActivity directory found", NULL);
    }
    
    err = getDirDataPath(assignmentDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read AssignmentActivity directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(assignmentDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No assignments data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No assignment data to load", NULL);
    }
    
    code assignmentsLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        Assignment assignment;
        assignment.head = NULL;
        assignment.description = NULL;
        
        char head[256] = {0};
        char assignDateStr[64] = {0};
        char dueDateStr[64] = {0};
        char description[1024] = {0};
        
        sscanf(currentLine->line, "%[^,],%[^,],%[^,],%[^\n]", 
               head, assignDateStr, dueDateStr, description);
        
        err = allocateString(&assignment.head, head);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = stringToDateTime(&assignment.assignDate, assignDateStr);
        if (err != NULL) {
            FreeString(&assignment.head);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = stringToDateTime(&assignment.dueDate, dueDateStr);
        if (err != NULL) {
            FreeString(&assignment.head);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = allocateString(&assignment.description, description);
        if (err != NULL) {
            FreeString(&assignment.head);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = addAssignmentToList(&class->assignmentList, assignment);
        if (err != NULL) {
            FreeAssignmentContents(&assignment);
            currentLine = currentLine->nextline;
            continue;
        }
        
        assignmentsLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (assignmentsLoaded == 0) {
        return SetStatus(1, "No assignment data was loaded", NULL);
    }
    
    return SetStatus(1, "Assignment data loaded successfully", NULL);
}

Status LoadLearningActivities(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    if (class->learningActivityList != NULL) {
        FreeLearningActivityList(class->learningActivityList);
        class->learningActivityList = NULL;
    }

    DataPath* learningActDir = NULL;
    err = findDataPathByFilename(classDataPath, "LearningActivity", &learningActDir);
    if (err != NULL || learningActDir == NULL) {
        return SetStatus(1, "No LearningActivity directory found", NULL);
    }
    
    err = getDirDataPath(learningActDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read LearningActivity directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(learningActDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No learning activities data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No learning activity data to load", NULL);
    }
    
    code activitiesLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        LearningActivity activity;
        activity.head = NULL;
        activity.description = NULL;
        activity.url = NULL;
        activity.file = NULL;
        
        char head[256] = {0};
        char description[1024] = {0};
        char url[512] = {0};
        
        sscanf(currentLine->line, "%[^,],%[^,],%[^\n]", head, url, description);
        
        err = allocateString(&activity.head, head);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        if (strlen(url) > 0) {
            err = allocateString(&activity.url, url);
            if (err != NULL) {
                FreeString(&activity.head);
                currentLine = currentLine->nextline;
                continue;
            }
        }
        
        err = allocateString(&activity.description, description);
        if (err != NULL) {
            if (activity.url != NULL) FreeString(&activity.url);
            FreeString(&activity.head);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = addLearningActivityToList(&class->learningActivityList, activity);
        if (err != NULL) {
            FreeLearningActivityContents(&activity);
            currentLine = currentLine->nextline;
            continue;
        }
        
        activitiesLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (activitiesLoaded == 0) {
        return SetStatus(1, "No learning activity data was loaded", NULL);
    }
    
    return SetStatus(1, "Learning activities loaded successfully", NULL);
}

Status LoadAttendance(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    if (class->attendanceList != NULL) {
        FreeAttendanceList(class->attendanceList);
        class->attendanceList = NULL;
    }
    
    DataPath* attendanceDir = NULL;
    err = findDataPathByFilename(classDataPath, "Attendance", &attendanceDir);
    if (err != NULL || attendanceDir == NULL) {
        return SetStatus(1, "No Attendance directory found", NULL);
    }
    
    err = getDirDataPath(attendanceDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read Attendance directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(attendanceDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No attendance data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No attendance data to load", NULL);
    }
    
    code attendancesLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        Attendance attendance;
        
        char dateStr[64] = {0};
        int attendanceCode = 0;
        
        sscanf(currentLine->line, "%[^,],%d", dateStr, &attendanceCode);
        
        err = stringToDateTime(&attendance.date, dateStr);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        attendance.joined = attendanceCode;
        
        err = addAttendanceToList(&class->attendanceList, attendance);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        attendancesLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (attendancesLoaded == 0) {
        return SetStatus(1, "No attendance data was loaded", NULL);
    }
    
    return SetStatus(1, "Attendance records loaded successfully", NULL);
}

Status LoadScoreBook(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }
    
    error err = NULL;
    
    if (class->scoreBookList != NULL) {
        FreeScoreBookList(class->scoreBookList);
        class->scoreBookList = NULL;
    }
    
    DataPath* scoreBookDir = NULL;
    err = findDataPathByFilename(classDataPath, "ScoreBook", &scoreBookDir);
    if (err != NULL || scoreBookDir == NULL) {
        return SetStatus(1, "No ScoreBook directory found", NULL);
    }
    
    err = getDirDataPath(scoreBookDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read ScoreBook directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(scoreBookDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No scorebook data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No scorebook data to load", NULL);
    }
    
    code scoresLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        ScoreBook scoreBook;
        scoreBook.head = NULL;
        scoreBook.description = NULL;
        
        char head[256] = {0};
        char description[1024] = {0};
        float32 score = 0.0f;
        
        sscanf(currentLine->line, "%[^,],%f,%[^\n]", head, &score, description);
        
        err = allocateString(&scoreBook.head, head);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        scoreBook.score = score;
        
        err = allocateString(&scoreBook.description, description);
        if (err != NULL) {
            FreeString(&scoreBook.head);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = addScoreBookToList(&class->scoreBookList, scoreBook);
        if (err != NULL) {
            FreeScoreBookContents(&scoreBook);
            currentLine = currentLine->nextline;
            continue;
        }
        
        scoresLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (scoresLoaded == 0) {
        return SetStatus(1, "No scorebook data was loaded", NULL);
    }
    
    return SetStatus(1, "Scorebook data loaded successfully", NULL);
}

Status LoadLearnIt(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->learnItList != NULL) {
        FreeLearnItList(class->learnItList);
        class->learnItList = NULL;
    }
    
    DataPath* learnItDir = NULL;
    err = findDataPathByFilename(classDataPath, "LearnIt", &learnItDir);
    if (err != NULL || learnItDir == NULL) {
        return SetStatus(1, "No LearnIt directory found", NULL);
    }
    
    err = getDirDataPath(learnItDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read LearnIt directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(learnItDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No LearnIt data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No LearnIt data to load", NULL);
    }
    
    code learnItsLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        LearnIt learnIt;
        learnIt.name = NULL;
        learnIt.description = NULL;
        learnIt.url = NULL;
        
        char name[256] = {0};
        char description[1024] = {0};
        char url[512] = {0};
        
        sscanf(currentLine->line, "%[^,],%[^,],%[^\n]", name, description, url);
        
        err = allocateString(&learnIt.name, name);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = allocateString(&learnIt.description, description);
        if (err != NULL) {
            FreeString(&learnIt.name);
            currentLine = currentLine->nextline;
            continue;
        }
        
        if (strlen(url) > 0) {
            err = allocateString(&learnIt.url, url);
            if (err != NULL) {
                FreeString(&learnIt.description);
                FreeString(&learnIt.name);
                currentLine = currentLine->nextline;
                continue;
            }
        }
        
        err = addLearnItToList(&class->learnItList, learnIt);
        if (err != NULL) {
            FreeLearnItContents(&learnIt);
            currentLine = currentLine->nextline;
            continue;
        }
        
        learnItsLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (learnItsLoaded == 0) {
        return SetStatus(1, "No LearnIt items loaded", NULL);
    }
    
    return SetStatus(1, "LearnIt items loaded successfully", NULL);
}

Status LoadFileList(Auth* auth, Class* class, DataPath* classDataPath) {
    if (auth == NULL || class == NULL || classDataPath == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, Class, or DataPath is NULL");
    }

    error err = NULL;
    
    if (class->fileList != NULL) {
        FreeFileList(class->fileList);
        class->fileList = NULL;
    }
    
    DataPath* filesDir = NULL;
    err = findDataPathByFilename(classDataPath, "Files", &filesDir);
    if (err != NULL || filesDir == NULL) {
        return SetStatus(1, "No Files directory found", NULL);
    }
    
    err = getDirDataPath(filesDir);
    if (err != NULL) {
        return SetStatus(0, "Failed to read Files directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(filesDir, "datalist.csv", &datalistPath);
    if (err != NULL || datalistPath == NULL) {
        return SetStatus(1, "No files data found", NULL);
    }
    
    textline* lines = NULL;
    err = readFileToTextLinesPath(&lines, datalistPath->path);
    if (err != NULL || lines == NULL) {
        return SetStatus(1, "No files data to load", NULL);
    }
    
    code filesLoaded = 0;
    textline* currentLine = lines;
    
    while (currentLine != NULL) {
        File file;
        
        char filename[256] = {0};
        char dirPath[512] = {0};
        char fileType[64] = {0};
        
        sscanf(currentLine->line, "%[^,],%[^,],%[^\n]", filename, dirPath, fileType);
        
        initPath(&file.filename);
        initPath(&file.dirPath);
        initPath(&file.fileType);
        
        err = createPath(&file.filename, filename);
        if (err != NULL) {
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = createPath(&file.dirPath, dirPath);
        if (err != NULL) {
            FreePathContent(&file.filename);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = createPath(&file.fileType, fileType);
        if (err != NULL) {
            FreePathContent(&file.filename);
            FreePathContent(&file.dirPath);
            currentLine = currentLine->nextline;
            continue;
        }
        
        err = addFileToList(&class->fileList, file);
        if (err != NULL) {
            FreeFileContent(&file);
            currentLine = currentLine->nextline;
            continue;
        }
        
        filesLoaded = 1;
        currentLine = currentLine->nextline;
    }
    
    FreeTextLine(lines);
    
    if (filesLoaded == 0) {
        return SetStatus(1, "No files loaded", NULL);
    }
    
    return SetStatus(1, "Files loaded successfully", NULL);
}