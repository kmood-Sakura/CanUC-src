#include "../class-operation.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status ShowAssignmentDetails(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mAssignment Details\033[0m\n\n");
    
    if (class->assignmentList == NULL) {
        printf("  No assignments available\n");
        return SetStatus(0, "No assignments available", NULL);
    }
    
    AssignmentList* current = class->assignmentList;
    int index = 1;
    
    printf("+---+----------------+------------------+----------------+--------------------------------+\n");
    printf("| # | Title          | Assign Date      | Due Date       | Description                    |\n");
    printf("+---+----------------+------------------+----------------+--------------------------------+\n");
    
    while (current != NULL) {
        string assignDate = NULL, dueDate = NULL;
        error err = dateTimeToString(&assignDate, current->assignment.assignDate);
        if (err != NULL) {
            Error(err);
            return SetStatus(0, "Failed to convert assign date to string", err);
        }
        
        err = dateTimeToString(&dueDate, current->assignment.dueDate);
        if (err != NULL) {
            Error(err);
            FreeString(&assignDate);
            return SetStatus(0, "Failed to convert due date to string", err);
        }
        
        printf("| %-1d | %-14s | %-16s | %-14s | %-30s |\n", 
               index++, 
               current->assignment.head, 
               assignDate, 
               dueDate, 
               current->assignment.description);
        
        FreeString(&assignDate);
        FreeString(&dueDate);
        current = current->next;
    }
    
    printf("+---+----------------+------------------+----------------+--------------------------------+\n");
    
    return SetStatus(1, "Assignment details displayed", NULL);
}

Status AddAssignment(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mAdd Assignment\033[0m\n\n");
    
    Assignment assignment;
    assignment.head = NULL;
    assignment.description = NULL;
    
    string title = NULL;
    code result = requestString(&title, 50, "Enter Assignment Title");
    if (result != 1 || title == NULL) {
        return SetStatus(0, "Invalid title", "Failed to get assignment title");
    }
    
    string description = NULL;
    result = requestString(&description, 200, "Enter Description");
    if (result != 1 || description == NULL) {
        FreeString(&title);
        return SetStatus(0, "Invalid description", "Failed to get assignment description");
    }
    
    string assignDateStr = NULL;
    result = requestString(&assignDateStr, 20, "Enter Assign Date (YYYY-MM-DD HH:MM:SS)");
    if (result != 1 || assignDateStr == NULL) {
        FreeString(&title);
        FreeString(&description);
        return SetStatus(0, "Invalid assign date", "Failed to get assign date");
    }
    
    string dueDateStr = NULL;
    result = requestString(&dueDateStr, 20, "Enter Due Date (YYYY-MM-DD HH:MM:SS)");
    if (result != 1 || dueDateStr == NULL) {
        FreeString(&title);
        FreeString(&description);
        FreeString(&assignDateStr);
        return SetStatus(0, "Invalid due date", "Failed to get due date");
    }
    
    DateTime assignDate, dueDate;
    error err = stringToDateTime(&assignDate, assignDateStr);
    if (err != NULL) {
        FreeString(&title);
        FreeString(&description);
        FreeString(&assignDateStr);
        FreeString(&dueDateStr);
        return SetStatus(0, "Invalid assign date format", err);
    }
    
    err = stringToDateTime(&dueDate, dueDateStr);
    if (err != NULL) {
        FreeString(&title);
        FreeString(&description);
        FreeString(&assignDateStr);
        FreeString(&dueDateStr);
        return SetStatus(0, "Invalid due date format", err);
    }
    
    FreeString(&assignDateStr);
    FreeString(&dueDateStr);
    
    err = allocateString(&assignment.head, title);
    if (err != NULL) {
        FreeString(&title);
        FreeString(&description);
        return SetStatus(0, "Failed to allocate title", err);
    }
    
    err = allocateString(&assignment.description, description);
    if (err != NULL) {
        FreeString(&title);
        FreeString(&description);
        FreeString(&assignment.head);
        return SetStatus(0, "Failed to allocate description", err);
    }
    
    assignment.assignDate = assignDate;
    assignment.dueDate = dueDate;
    
    FreeString(&title);
    FreeString(&description);
    
    if (class->assignmentList == NULL) {
        err = allocateAssignmentList(&class->assignmentList);
        if (err != NULL) {
            FreeString(&assignment.head);
            FreeString(&assignment.description);
            return SetStatus(0, "Failed to allocate assignment list", err);
        }
        
        err = createAssignmentListNode(&class->assignmentList, assignment);
        if (err != NULL) {
            FreeString(&assignment.head);
            FreeString(&assignment.description);
            return SetStatus(0, "Failed to create assignment node", err);
        }
    } else {
        err = addAssignmentToList(&class->assignmentList, assignment);
        if (err != NULL) {
            FreeString(&assignment.head);
            FreeString(&assignment.description);
            return SetStatus(0, "Failed to add assignment to list", err);
        }
    }
    
    Status status = UpdateAssignmentFile(auth, class);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update assignment file");
        Error(status.msg);
    }
    
    printf("\n\033[0;32mAssignment '%s' added successfully.\033[0m\n", assignment.head);
    return SetStatus(1, "Assignment added successfully", NULL);
}

Status RemoveAssignment(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mRemove Assignment\033[0m\n\n");
    
    Status status = ShowAssignmentDetails(auth, class);
    if (status.code != 1) {
        return status;
    }
    
    string input = NULL;
    printf("\nEnter the number of the assignment to remove ('C' or 'c' to cancel): ");
    code result = requestString(&input, 10, "");
    if (result != 1 || input == NULL) {
        return SetStatus(0, "Invalid input", "Failed to get input");
    }
    
    if (strcmp(input, "C") == 0 || strcmp(input, "c") == 0) {
        FreeString(&input);
        return SetStatus(1, "Operation cancelled", NULL);
    }
    
    int index = atoi(input);
    FreeString(&input);
    
    if (index < 1) {
        return SetStatus(0, "Invalid index", "Index must be greater than 0");
    }
    
    AssignmentList* current = class->assignmentList;
    AssignmentList* prev = NULL;
    int currentIndex = 1;
    
    while (current != NULL && currentIndex < index) {
        prev = current;
        current = current->next;
        currentIndex++;
    }
    
    if (current == NULL) {
        return SetStatus(0, "Invalid index", "Assignment not found");
    }
    
    if (prev == NULL) {
        class->assignmentList = current->next;
    } else {
        prev->next = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = prev;
    }
    
    string title = NULL;
    error err = allocateString(&title, current->assignment.head);
    if (err != NULL) {
        FreeAssignmentContents(&current->assignment);
        free(current);
        return SetStatus(0, "Failed to allocate title", err);
    }

    FreeAssignmentContents(&current->assignment);
    free(current);
    
    status = UpdateAssignmentFile(auth, class);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update assignment file");
        Error(status.msg);
    }
    
    printf("\n\033[0;32mAssignment '%s' removed successfully.\033[0m\n", title);
    FreeString(&title);
    return SetStatus(1, "Assignment removed successfully", NULL);
}

Status UpdateAssignmentFile(Auth* auth, Class* class) {
    if (auth == NULL || auth->dataPath == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, dataPath or class is NULL");
    }
    
    DataPath* classPath = NULL;
    error err = findDataPathByFilename(auth->dataPath, class->classId, &classPath);
    if (err != NULL || classPath == NULL) {
        return SetStatus(0, "Failed to find class directory", err);
    }
    
    DataPath* assignmentDir = NULL;
    err = findDataPathByFilename(classPath, "AssignmentActivity", &assignmentDir);
    if (err != NULL || assignmentDir == NULL) {
        return SetStatus(0, "Failed to find AssignmentActivity directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(assignmentDir, "datalist.csv", &datalistPath);
    if (err != NULL) {
        File file;
        err = createFileWithPath(&file, datalistPath->filename, assignmentDir->path, datalistPath->filename);
        if (err != NULL) {
            return SetStatus(0, "Failed to create datalist file path", err);
        }
        
        err = MakeFile(&file);
        if (err != NULL) {
            FreeFileContent(&file);
            return SetStatus(0, "Failed to create datalist file", err);
        }
        
        FreeFileContent(&file);
    }
    
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, assignmentDir->path.path, "/datalist.csv");
    if (err != NULL) {
        return SetStatus(0, "Failed to create full path for datalist", err);
    }
    
    FILE* file = fopen(fullPath, "w");
    if (!file) {
        FreeString(&fullPath);
        return SetStatus(0, "Failed to open datalist file for writing", "File open error");
    }
    
    AssignmentList* current = class->assignmentList;
    while (current != NULL) {
        string assignDateStr = NULL, dueDateStr = NULL;
        err = dateTimeToString(&assignDateStr, current->assignment.assignDate);
        if (err != NULL) {
            Error(err);
            FreeString(&fullPath);
            fclose(file);
            return SetStatus(0, "Failed to convert assign date to string", err);
        }
        
        err = dateTimeToString(&dueDateStr, current->assignment.dueDate);
        if (err != NULL) {
            Error(err);
            FreeString(&assignDateStr);
            FreeString(&fullPath);
            fclose(file);
            return SetStatus(0, "Failed to convert due date to string", err);
        }
        
        fprintf(file, "%s,%s,%s,%s\n", 
                current->assignment.head, 
                assignDateStr, 
                dueDateStr, 
                current->assignment.description);
        
        FreeString(&assignDateStr);
        FreeString(&dueDateStr);
        current = current->next;
    }
    
    fclose(file);
    FreeString(&fullPath);
    return SetStatus(1, "Assignment file updated successfully", NULL);
}

Status ShowLearningActivityDetails(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mLearning Activity Details\033[0m\n\n");
    
    if (class->learningActivityList == NULL) {
        printf("  No learning activities available\n");
        return SetStatus(0, "No learning activities available", NULL);
    }
    
    LearningActivityList* current = class->learningActivityList;
    int index = 1;
    
    printf("+---+----------------+----------------+--------------------------------+\n");
    printf("| # | Title          | URL            | Description                    |\n");
    printf("+---+----------------+----------------+--------------------------------+\n");
    
    while (current != NULL) {
        printf("| %-1d | %-14s | %-14s | %-30s |\n", 
               index++, 
               current->learningActivity.head, 
               current->learningActivity.url ? current->learningActivity.url : "N/A", 
               current->learningActivity.description);
        
        current = current->next;
    }
    
    printf("+---+----------------+----------------+--------------------------------+\n");
    
    return SetStatus(1, "Learning activity details displayed", NULL);
}

Status AddLearningActivity(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mAdd Learning Activity\033[0m\n\n");
    
    LearningActivity activity;
    activity.head = NULL;
    activity.description = NULL;
    activity.url = NULL;
    activity.file = NULL;
    
    string title = NULL;
    code result = requestString(&title, 50, "Enter Activity Title");
    if (result != 1 || title == NULL) {
        return SetStatus(0, "Invalid title", "Failed to get activity title");
    }
    
    string url = NULL;
    result = requestString(&url, 100, "Enter URL (or leave empty)");
    
    string description = NULL;
    result = requestString(&description, 200, "Enter Description");
    if (result != 1 || description == NULL) {
        FreeString(&title);
        if (url != NULL) FreeString(&url);
        return SetStatus(0, "Invalid description", "Failed to get activity description");
    }
    
    error err = allocateString(&activity.head, title);
    if (err != NULL) {
        FreeString(&title);
        if (url != NULL) FreeString(&url);
        FreeString(&description);
        return SetStatus(0, "Failed to allocate title", err);
    }
    
    if (url != NULL && strlen(url) > 0) {
        err = allocateString(&activity.url, url);
        if (err != NULL) {
            FreeString(&title);
            FreeString(&url);
            FreeString(&description);
            FreeString(&activity.head);
            return SetStatus(0, "Failed to allocate URL", err);
        }
    }
    
    err = allocateString(&activity.description, description);
    if (err != NULL) {
        FreeString(&title);
        if (url != NULL) FreeString(&url);
        FreeString(&description);
        FreeString(&activity.head);
        if (activity.url != NULL) FreeString(&activity.url);
        return SetStatus(0, "Failed to allocate description", err);
    }
    
    FreeString(&title);
    if (url != NULL) FreeString(&url);
    FreeString(&description);
    
    if (class->learningActivityList == NULL) {
        err = allocateLearningActivityList(&class->learningActivityList);
        if (err != NULL) {
            FreeString(&activity.head);
            if (activity.url != NULL) FreeString(&activity.url);
            FreeString(&activity.description);
            return SetStatus(0, "Failed to allocate learning activity list", err);
        }
        
        err = createLearningActivityListNode(&class->learningActivityList, activity);
        if (err != NULL) {
            FreeString(&activity.head);
            if (activity.url != NULL) FreeString(&activity.url);
            FreeString(&activity.description);
            return SetStatus(0, "Failed to create learning activity node", err);
        }
    } else {
        err = addLearningActivityToList(&class->learningActivityList, activity);
        if (err != NULL) {
            FreeString(&activity.head);
            if (activity.url != NULL) FreeString(&activity.url);
            FreeString(&activity.description);
            return SetStatus(0, "Failed to add learning activity to list", err);
        }
    }
    
    Status status = UpdateLearningActivityFile(auth, class);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update learning activity file");
        Error(status.msg);
    }
    
    printf("\n\033[0;32mLearning Activity '%s' added successfully.\033[0m\n", activity.head);
    return SetStatus(1, "Learning Activity added successfully", NULL);
}

Status RemoveLearningActivity(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth or class is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mRemove Learning Activity\033[0m\n\n");
    
    Status status = ShowLearningActivityDetails(auth, class);
    if (status.code != 1) {
        return status;
    }
    
    string input = NULL;
    printf("\nEnter the number of the learning activity to remove (or 'c' to cancel): ");
    code result = requestString(&input, 10, "");
    if (result != 1 || input == NULL) {
        return SetStatus(0, "Invalid input", "Failed to get input");
    }
    
    if (input[0] == 'c' || input[0] == 'C') {
        FreeString(&input);
        return SetStatus(0, "Operation cancelled", "User cancelled operation");
    }
    
    int index = atoi(input);
    FreeString(&input);
    
    if (index < 1) {
        return SetStatus(0, "Invalid index", "Index must be greater than 0");
    }
    
    LearningActivityList* current = class->learningActivityList;
    LearningActivityList* prev = NULL;
    int currentIndex = 1;
    
    while (current != NULL && currentIndex < index) {
        prev = current;
        current = current->next;
        currentIndex++;
    }
    
    if (current == NULL) {
        return SetStatus(0, "Invalid index", "Learning Activity not found");
    }
    
    if (prev == NULL) {
        class->learningActivityList = current->next;
    } else {
        prev->next = current->next;
    }
    
    if (current->next != NULL) {
        current->next->prev = prev;
    }
    
    string title = NULL;
    error err = allocateString(&title, current->learningActivity.head);
    if (err != NULL) {
        FreeLearningActivityContents(&current->learningActivity);
        free(current);
        return SetStatus(0, "Failed to allocate title", err);
    }
    FreeLearningActivityContents(&current->learningActivity);
    free(current);
    
    status = UpdateLearningActivityFile(auth, class);
    if (status.code != 1) {
        LogMsg("Warning: Failed to update learning activity file");
        Error(status.msg);
    }
    
    printf("\n\033[0;32mLearning Activity '%s' removed successfully.\033[0m\n", title);
    FreeString(&title);
    return SetStatus(1, "Learning Activity removed successfully", NULL);
}

Status UpdateLearningActivityFile(Auth* auth, Class* class) {
    if (auth == NULL || auth->dataPath == NULL || class == NULL) {
        return SetStatus(0, "Invalid parameters", "Auth, dataPath or class is NULL");
    }
    
    DataPath* classPath = NULL;
    error err = findDataPathByFilename(auth->dataPath, class->classId, &classPath);
    if (err != NULL || classPath == NULL) {
        return SetStatus(0, "Failed to find class directory", err);
    }
    
    DataPath* learningActivityDir = NULL;
    err = findDataPathByFilename(classPath, "LearningActivity", &learningActivityDir);
    if (err != NULL || learningActivityDir == NULL) {
        return SetStatus(0, "Failed to find LearningActivity directory", err);
    }
    
    DataPath* datalistPath = NULL;
    err = findDataPathByFilename(learningActivityDir, "datalist.csv", &datalistPath);
    if (err != NULL) {
        File file;
        err = createFileWithPath(&file, datalistPath->filename, learningActivityDir->path, datalistPath->filename);
        if (err != NULL) {
            return SetStatus(0, "Failed to create datalist file path", err);
        }
        
        err = MakeFile(&file);
        if (err != NULL) {
            FreeFileContent(&file);
            return SetStatus(0, "Failed to create datalist file", err);
        }
        
        FreeFileContent(&file);
    }
    
    string fullPath = NULL;
    err = mergeTwoStrings(&fullPath, learningActivityDir->path.path, "/datalist.csv");
    if (err != NULL) {
        return SetStatus(0, "Failed to create full path for datalist", err);
    }
    
    FILE* file = fopen(fullPath, "w");
    if (!file) {
        FreeString(&fullPath);
        return SetStatus(0, "Failed to open datalist file for writing", "File open error");
    }
    
    LearningActivityList* current = class->learningActivityList;
    while (current != NULL) {
        fprintf(file, "%s,%s,%s\n", 
                current->learningActivity.head, 
                current->learningActivity.url ? current->learningActivity.url : "", 
                current->learningActivity.description);
        
        current = current->next;
    }
    
    fclose(file);
    FreeString(&fullPath);
    return SetStatus(1, "Learning Activity file updated successfully", NULL);
}

void ClassAssignmentPage(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return;
    }
    
    char cmd;
    
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mClass Assignments - %s\033[0m\n\n", class->classId);
        
        printf("  [1] View Assignment List\n");
        printf("  [2] Add Assignment\n");
        printf("  [3] Remove Assignment\n\n");
        printf("  [b] Back\n\n");
        
        if(!requestCommand(&cmd)) {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        Status status;
        switch (cmd) {
            case '1':
                status = ShowAssignmentDetails(auth, class);
                if (status.code != 1) {
                    printf("\n\033[0;31mNo assignments found.\033[0m\n");
                }
                printf("\nPress Enter to continue...");
                while (getchar() != '\n');
                break;
            case '2':
                status = AddAssignment(auth, class);
                if (status.code != 1) {
                    printf("\n\033[0;31mFailed to add assignment: %s\033[0m\n", status.details);
                }
                break;
            case '3':
                status = RemoveAssignment(auth, class);
                if (status.code != 1 && status.msg != NULL) {
                    printf("\n\033[0;31m%s\033[0m\n", status.msg);
                }
                break;
            case 'b':
                return;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }
}

void ClassLearningActivityPage(Auth* auth, Class* class) {
    if (auth == NULL || class == NULL) {
        return;
    }
    
    char cmd;
    
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mClass Learning Activities - %s\033[0m\n\n", class->classId);
        
        printf("  [1] View Learning Activity List\n");
        printf("  [2] Add Learning Activity\n");
        printf("  [3] Remove Learning Activity\n\n");
        printf("  [b] Back\n\n");
        
        if(!requestCommand(&cmd)) {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        Status status;
        switch (cmd) {
            case '1':
                status = ShowLearningActivityDetails(auth, class);
                if (status.code != 1) {
                    printf("\n\033[0;31mNo learning activities found.\033[0m\n");
                }
                printf("\nPress Enter to continue...");
                while (getchar() != '\n');
                break;
            case '2':
                status = AddLearningActivity(auth, class);
                if (status.code != 1) {
                    printf("\n\033[0;31mFailed to add learning activity: %s\033[0m\n", status.details);
                }
                break;
            case '3':
                status = RemoveLearningActivity(auth, class);
                if (status.code != 1 && status.msg != NULL) {
                    printf("\n\033[0;31m%s\033[0m\n", status.msg);
                }
                break;
            case 'b':
                return;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }
}