#include "../leb2-service.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status AddAssignmentService(Auth* auth, Class* classnode, Assignment* assignment) {
    if (auth == NULL || classnode == NULL || assignment == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, class, or assignment is NULL");
    }
    
    AssignmentList* current = classnode->assignmentList;
    while (current != NULL) {
        if (strcmp(current->assignment.head, assignment->head) == 0) {
            return SetStatus(0, "Assignment already exists", "An assignment with this title already exists");
        }
        current = current->next;
    }
    
    error err = NULL;
    if (classnode->assignmentList == NULL) {
        err = allocateAssignmentList(&classnode->assignmentList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate assignment list", err);
        }
        
        err = createAssignmentListNode(&classnode->assignmentList, *assignment);
        if (err != NULL) {
            return SetStatus(0, "Failed to create assignment node", err);
        }
    } else {
        err = addAssignmentToList(&classnode->assignmentList, *assignment);
        if (err != NULL) {
            return SetStatus(0, "Failed to add assignment to list", err);
        }
    }
    
    Status status = UpdateAssignmentData(auth, classnode);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update assignment file");
        Error(status.msg);
        return status;
    }
    
    return SetStatus(1, "Assignment added successfully", NULL);
}

Status RemoveAssignmentService(Auth* auth, Class* classnode, const string title) {
    if (auth == NULL || classnode == NULL || title == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, class, or title is NULL");
    }
    
    if (classnode->assignmentList == NULL) {
        return SetStatus(0, "No assignments available", NULL);
    }
    
    AssignmentList* current = classnode->assignmentList;
    AssignmentList* prev = NULL;
    code found = 0;
    
    while (current != NULL) {
        if (strcmp(current->assignment.head, title) == 0) {
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    if (!found) {
        return SetStatus(0, "Assignment not found", "No assignment with the given title exists");
    }
    
    if (prev == NULL) {
        classnode->assignmentList = current->next;
    } else {
        prev->next = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = prev;
    }
    
    FreeAssignmentContents(&current->assignment);
    free(current);
    
    Status status = UpdateAssignmentData(auth, classnode);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update assignment file");
        Error(status.msg);
        return status;
    }
    
    return SetStatus(1, "Assignment removed successfully", NULL);
}

Status UpdateAssignmentData(Auth* auth, Class* classnode) {
    if (auth == NULL || auth->dataPath == NULL || classnode == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, dataPath or class is NULL");
    }
    DataPath* classPath = NULL;
    error err = findDataPathByFilename(auth->dataPath, classnode->classId, &classPath);
    if (err != NULL || classPath == NULL) {
        return SetStatus(0, "Failed to find class directory", err ? err : "Class directory not found");
    }
    DataPath* assignmentDir = NULL;
    err = findDataPathByFilename(classPath, "AssignmentActivity", &assignmentDir);
    if (err != NULL || assignmentDir == NULL) {
        err = getDirDataPath(classPath);
        if (err != NULL) {
            return SetStatus(0, "Failed to read class directory", err);
        }
        Folder folder;
        err = createFolder(&folder, "AssignmentActivity", classPath->path.path);
        if (err != NULL) {
            return SetStatus(0, "Failed to create AssignmentActivity folder", err);
        }
        
        err = MakeFolder(&folder);
        if (err != NULL) {
            FreeFolderContent(&folder);
            return SetStatus(0, "Failed to create AssignmentActivity directory", err);
        }
        
        err = getDirDataPath(classPath);
        if (err != NULL) {
            return SetStatus(0, "Failed to refresh class directory", err);
        }
        
        err = findDataPathByFilename(classPath, "AssignmentActivity", &assignmentDir);
        if (err != NULL || assignmentDir == NULL) {
            return SetStatus(0, "Failed to find newly created AssignmentActivity directory", err);
        }
        
        FreeFolderContent(&folder);
    }
    File file;
    initPath(&file.filename);
    initPath(&file.dirPath);
    initPath(&file.fileType);
    
    err = createPath(&file.filename, "datalist");
    if (err != NULL) {
        return SetStatus(0, "Failed to create filename path", err);
    }
    
    err = createPath(&file.dirPath, assignmentDir->path.path);
    if (err != NULL) {
        FreePathContent(&file.filename);
        return SetStatus(0, "Failed to create dirPath path", err);
    }
    
    err = createPath(&file.fileType, ".csv");
    if (err != NULL) {
        FreePathContent(&file.filename);
        FreePathContent(&file.dirPath);
        return SetStatus(0, "Failed to create fileType path", err);
    }
    
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, assignmentDir->path.path, "/datalist.csv");
    if (err != NULL) {
        FreeFileContent(&file);
        return SetStatus(0, "Failed to create full path for datalist", err);
    }
    if (FileExist(fullPath)) {
        remove(fullPath);
    }
    
    // Create the file
    err = MakeFile(&file);
    if (err != NULL) {
        FreeString(&fullPath);
        FreeFileContent(&file);
        return SetStatus(0, "Failed to create datalist file", err);
    }
    FILE* fileHandle = fopen(fullPath, "w");
    if (!fileHandle) {
        FreeString(&fullPath);
        FreeFileContent(&file);
        return SetStatus(0, "Failed to open datalist file for writing", "File open error");
    }
    
    AssignmentList* current = classnode->assignmentList;
    while (current != NULL) {
        string assignDateStr = NULL, dueDateStr = NULL;
        err = dateTimeToString(&assignDateStr, current->assignment.assignDate);
        if (err != NULL) {
            Error(err);
            FreeString(&fullPath);
            FreeFileContent(&file);
            fclose(fileHandle);
            return SetStatus(0, "Failed to convert assign date to string", err);
        }
        
        err = dateTimeToString(&dueDateStr, current->assignment.dueDate);
        if (err != NULL) {
            Error(err);
            FreeString(&assignDateStr);
            FreeString(&fullPath);
            FreeFileContent(&file);
            fclose(fileHandle);
            return SetStatus(0, "Failed to convert due date to string", err);
        }
        
        fprintf(fileHandle, "%s,%s,%s,%s\n", 
                current->assignment.head, 
                assignDateStr, 
                dueDateStr, 
                current->assignment.description);
        
        FreeString(&assignDateStr);
        FreeString(&dueDateStr);
        current = current->next;
    }
    
    fclose(fileHandle);
    FreeString(&fullPath);
    FreeFileContent(&file);
    
    return SetStatus(1, "Assignment data updated successfully", NULL);
}

Status AddLearningActivityService(Auth* auth, Class* classnode, LearningActivity* activity) {
    if (auth == NULL || classnode == NULL || activity == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, class, or activity is NULL");
    }
    LearningActivityList* current = classnode->learningActivityList;
    while (current != NULL) {
        if (strcmp(current->learningActivity.head, activity->head) == 0) {
            return SetStatus(0, "Learning activity already exists", "A learning activity with this title already exists");
        }
        current = current->next;
    }
    error err = NULL;
    if (classnode->learningActivityList == NULL) {
        err = allocateLearningActivityList(&classnode->learningActivityList);
        if (err != NULL) {
            return SetStatus(0, "Failed to allocate learning activity list", err);
        }
        
        err = createLearningActivityListNode(&classnode->learningActivityList, *activity);
        if (err != NULL) {
            return SetStatus(0, "Failed to create learning activity node", err);
        }
    } else {
        err = addLearningActivityToList(&classnode->learningActivityList, *activity);
        if (err != NULL) {
            return SetStatus(0, "Failed to add learning activity to list", err);
        }
    }
    
    Status status = UpdateLearningActivityData(auth, classnode);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update learning activity file");
        Error(status.msg);
        return status;
    }
    
    return SetStatus(1, "Learning activity added successfully", NULL);
}

Status RemoveLearningActivityService(Auth* auth, Class* classnode, const string title) {
    if (auth == NULL || classnode == NULL || title == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, class, or title is NULL");
    }
    
    if (classnode->learningActivityList == NULL) {
        return SetStatus(0, "No learning activities available", NULL);
    }
    
    LearningActivityList* current = classnode->learningActivityList;
    LearningActivityList* prev = NULL;
    code found = 0;
    
    while (current != NULL) {
        if (strcmp(current->learningActivity.head, title) == 0) {
            found = 1;
            break;
        }
        prev = current;
        current = current->next;
    }
    
    if (!found) {
        return SetStatus(0, "Learning activity not found", "No learning activity with the given title exists");
    }
    
    if (prev == NULL) {
        classnode->learningActivityList = current->next;
    } else {
        prev->next = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = prev;
    }
    FreeLearningActivityContents(&current->learningActivity);
    free(current);
    Status status = UpdateLearningActivityData(auth, classnode);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update learning activity file");
        Error(status.msg);
        return status;
    }
    
    return SetStatus(1, "Learning activity removed successfully", NULL);
}

Status UpdateLearningActivityData(Auth* auth, Class* classnode) {
    if (auth == NULL || auth->dataPath == NULL || classnode == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, dataPath or class is NULL");
    }
    
    DataPath* classPath = NULL;
    error err = findDataPathByFilename(auth->dataPath, classnode->classId, &classPath);
    if (err != NULL || classPath == NULL) {
        return SetStatus(0, "Failed to find class directory", err ? err : "Class directory not found");
    }
    
    DataPath* learningActivityDir = NULL;
    err = findDataPathByFilename(classPath, "LearningActivity", &learningActivityDir);
    if (err != NULL || learningActivityDir == NULL) {
        err = getDirDataPath(classPath);
        if (err != NULL) {
            return SetStatus(0, "Failed to read class directory", err);
        }
        Folder folder;
        err = createFolder(&folder, "LearningActivity", classPath->path.path);
        if (err != NULL) {
            return SetStatus(0, "Failed to create LearningActivity folder", err);
        }
        
        err = MakeFolder(&folder);
        if (err != NULL) {
            FreeFolderContent(&folder);
            return SetStatus(0, "Failed to create LearningActivity directory", err);
        }
        
        err = getDirDataPath(classPath);
        if (err != NULL) {
            return SetStatus(0, "Failed to refresh class directory", err);
        }
        
        err = findDataPathByFilename(classPath, "LearningActivity", &learningActivityDir);
        if (err != NULL || learningActivityDir == NULL) {
            return SetStatus(0, "Failed to find newly created LearningActivity directory", err);
        }
        
        FreeFolderContent(&folder);
    }
    
    File file;
    initPath(&file.filename);
    initPath(&file.dirPath);
    initPath(&file.fileType);
    
    err = createPath(&file.filename, "datalist");
    if (err != NULL) {
        return SetStatus(0, "Failed to create filename path", err);
    }
    
    err = createPath(&file.dirPath, learningActivityDir->path.path);
    if (err != NULL) {
        FreePathContent(&file.filename);
        return SetStatus(0, "Failed to create dirPath path", err);
    }
    
    err = createPath(&file.fileType, ".csv");
    if (err != NULL) {
        FreePathContent(&file.filename);
        FreePathContent(&file.dirPath);
        return SetStatus(0, "Failed to create fileType path", err);
    }
    
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, learningActivityDir->path.path, "/datalist.csv");
    if (err != NULL) {
        FreeFileContent(&file);
        return SetStatus(0, "Failed to create full path for datalist", err);
    }
    if (FileExist(fullPath)) {
        remove(fullPath);
    }
    err = MakeFile(&file);
    if (err != NULL) {
        FreeString(&fullPath);
        FreeFileContent(&file);
        return SetStatus(0, "Failed to create datalist file", err);
    }
    FILE* fileHandle = fopen(fullPath, "w");
    if (!fileHandle) {
        FreeString(&fullPath);
        FreeFileContent(&file);
        return SetStatus(0, "Failed to open datalist file for writing", "File open error");
    }
    LearningActivityList* current = classnode->learningActivityList;
    while (current != NULL) {
        fprintf(fileHandle, "%s,%s,%s\n", 
                current->learningActivity.head, 
                current->learningActivity.url ? current->learningActivity.url : "", 
                current->learningActivity.description);
        
        current = current->next;
    }
    
    fclose(fileHandle);
    FreeString(&fullPath);
    FreeFileContent(&file);
    
    return SetStatus(1, "Learning activity data updated successfully", NULL);
}