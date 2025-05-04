#include "../calendar-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
// for Auth and ShowCalendarPath()


void getTodayDateString(char* buffer, size_t size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

void addTask(Auth* auth) {
    if (!auth || !auth->userData) {
        printf("Invalid user data.\n");
        return;
    }
    error err = NULL;
    Task* newTask = malloc(sizeof(Task));
    if (!newTask) {
        perror("Memory allocation failed");
        return;
    }
    newTask->title = NULL;
    newTask->location = NULL;

    char tempStr[50];

    // Input Task Title
    printf("Enter Task Title: ");
    if (fgets(tempStr, 50, stdin) == NULL) {
        printf("Failed to read Task Title.\n");
        free(newTask);
        return;
    }
    uint32 tempStrLen = stringLen(tempStr);
    if (tempStr[tempStrLen-1] == '\n') {
        tempStr[tempStrLen-1] = '\0';
    }
    err = allocateStringLen(&newTask->title, tempStr, tempStrLen);
    if (err != NULL) {
        Error(err);
        free(newTask);
        return;
    }
    // newTask->title = tempStr;

    char locStr[50];
    // Input Location
    printf("Enter Location: ");
    if (fgets(locStr, 50, stdin) == NULL) {
        printf("Failed to read Location.\n");
        free(newTask->title);
        free(newTask);
        return;
    }
    uint32 locStrLen = stringLen(locStr);
    if (locStr[locStrLen-1] == '\n') {
        locStr[locStrLen-1] = '\0';
    }
    err = allocateStringLen(&newTask->location, locStr, locStrLen);
    if (err != NULL) {
        Error(err);
        free(newTask);
        return;
    }
    // Input Date
    char dateStr[11];
    printf("Enter Task Date (YYYY-MM-DD): ");
    if (fgets(dateStr, 11, stdin) == NULL) {
        printf("Failed to read date.\n");
        free(newTask->title);
        free(newTask);
        return;
    }

    Date taskDate;
    if (sscanf(dateStr, "%hu-%hhu-%hhu", &taskDate.year, &taskDate.month, &taskDate.day) != 3) {
        printf("Invalid date format.\n");
        free(newTask->title);
        free(newTask);
        return;
    }

    // Input Start Time
    int startH, startM;
    printf("Enter Start Time (HH MM): ");
    if (scanf("%d %d", &startH, &startM) != 2) {
        printf("Invalid start time input.\n");
        free(newTask->title);
        free(newTask);
        return;
    }

    // Input End Time
    int endH, endM;
    printf("Enter End Time (HH MM): ");
    if (scanf("%d %d", &endH, &endM) != 2) {
        printf("Invalid end time input.\n");
        free(newTask->title);
        free(newTask);
        return;
    }

    newTask->setBegin = makeDateTime(taskDate, startH, startM);
    newTask->setEnd = makeDateTime(taskDate, endH, endM);
    newTask->dueDate = newTask->setEnd;

    // Find the calendar for the date
    CalendarList* calList = auth->userData->calendarList;
    Calendar* targetCal = NULL;

    while (calList) {
        if (isSameDate(&calList->calendar.date, &taskDate)) {
            targetCal = &calList->calendar;
            break;
        }
        calList = calList->next;
    }

    // Insert task into calendar's task list (only if calendar exists)
    if (targetCal) {
        TaskList* newNode = malloc(sizeof(TaskList));
        if (!newNode) {
            perror("Memory allocation failed");
            free(newTask->title);
            free(newTask->location);
            free(newTask);
            return;
        }
        newNode->task = *newTask;
        newNode->next = targetCal->taskList;
        newNode->prev = NULL;
        if (targetCal->taskList != NULL)
            targetCal->taskList->prev = newNode;
        targetCal->taskList = newNode;
    }

    // Write task to CSV file
    Path calendarDir;
    initPath(&calendarDir);
    err = NULL;
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        Error(err);
    } else {
        char filename[32];
        snprintf(filename, sizeof(filename), "%04d-%02d-%02d.csv",
                taskDate.year, taskDate.month, taskDate.day);

        Path filenamePath, fullPath;
        createPathLen(&filenamePath, filename, strlen(filename));
        createPathFileorFolder(&fullPath, filenamePath, calPath->path);

        // Check if file exists
        bool fileExists = false;
        FILE* testFile = fopen(fullPath.path, "r");
        if (testFile != NULL) {
            fileExists = true;
            fclose(testFile);
        }

        FILE* f = fopen(fullPath.path, "a");
        if (f) {
            // Write header if file is new
            if (!fileExists) {
                fprintf(f, "Title,Date,Begin,End,Location\n");
            }

            string beginStr, endStr;
            dateTimeToString(&beginStr, newTask->setBegin);
            dateTimeToString(&endStr, newTask->setEnd);

            fprintf(f, "%s,%s,%s,%s\n",
                    newTask->title,
                    beginStr,
                    endStr,
                    newTask->location);

            fclose(f);
            free(beginStr);
            free(endStr);

            printf("Task '%s' added successfully.\n", newTask->title);
        } else {
            perror("Failed to open calendar file for writing");
        }
    }


    // Clean up
    if (newTask->title != NULL) {
        free(newTask->title);
    }
    if (newTask->location != NULL) {
        free(newTask->location);
    }
    if (newTask != NULL) {
        free(newTask);
    }
}

void showTodayCalendar(Auth* auth) {
    if (!auth || !auth->userData) {
        printf("Invalid user data.\n");
        return;
    }

    char today[20];
    getTodayDateString(today, sizeof(today));

    Path calendarDir;
    initPath(&calendarDir);
    error err = NULL;
    DataPath* calPath = NULL;
    err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        Error(err);
        return;
    }

    char filename[32];
    snprintf(filename, sizeof(filename), "%s.csv", today);

    Path filenamePath, fullPath;
    createPathLen(&filenamePath, filename, strlen(filename));
    createPathFileorFolder(&fullPath, filenamePath, calPath->path);

    FILE* file = fopen(fullPath.path, "r");
    if (file == NULL) {
        printf("No tasks for today (%s).\n", today);
        return;
    }

    printf("\n+----------------+------------+--------------+-------------+----------------------+\n");
    printf("|  Task Title    |   Date     | Start Time   | End Time    | Location             |\n");
    printf("+----------------+------------+--------------+-------------+----------------------+\n");

    char line[256];
    Task task;

    // Skip header
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        char tempTitle[50], tempLocation[50], tempBegin[20], tempEnd[20];
        if (sscanf(line, "%49[^,],%19[^,],%19[^,],%49[^\n]",
                   tempTitle, tempBegin, tempEnd, tempLocation) == 4) {

            task.title = strdup(tempTitle);
            task.location = strdup(tempLocation);

            Date taskDate;
            int startH, startM, startS, endH, endM, endS;

            // Parse full datetime with seconds for start time
            if (sscanf(tempBegin, "%hu-%hhu-%hhu %d:%d:%d", &taskDate.year, &taskDate.month, &taskDate.day,
                       &startH, &startM, &startS) != 6) {
                printf("Invalid start time format: %s\n", tempBegin);
                continue;
            }

            // Parse full datetime with seconds for end time
            if (sscanf(tempEnd, "%hu-%hhu-%hhu %d:%d:%d", &taskDate.year, &taskDate.month, &taskDate.day,
                       &endH, &endM, &endS) != 6) {
                printf("Invalid end time format: %s\n", tempEnd);
                continue;
            }

            task.setBegin = makeDateTime(taskDate, startH, startM);
            task.setEnd = makeDateTime(taskDate, endH, endM);

            printf("| %-14s | %-10.10s | %-12.5s | %-11.5s | %-20s |\n",
                task.title, tempBegin, tempBegin + 11, tempEnd + 11, task.location);
        }
    }

    printf("+----------------+------------+--------------+-------------+----------------------+\n");

    fclose(file);

    if (task.title != NULL) free(task.title);
    if (task.location != NULL) free(task.location);
}


void promptAndRemoveTask(Auth* auth) {
    if (!auth || !auth->userData) {
        printf("Invalid user data.\n");
        return;
    }

    char today[20];
    getTodayDateString(today, sizeof(today));

    // Locate the Calendar path
    DataPath* calPath = NULL;
    error err = findDataPathByFilename(auth->dataPath, "Calendar", &calPath);
    if (err != NULL) {
        Error(err);
        return;
    }

    // Construct full file path
    char filename[32];
    snprintf(filename, sizeof(filename), "%s.csv", today);

    Path filenamePath, fullPath;
    createPathLen(&filenamePath, filename, strlen(filename));
    createPathFileorFolder(&fullPath, filenamePath, calPath->path);

    FILE* file = fopen(fullPath.path, "r");
    if (!file) {
        printf("No task file found for today (%s).\n", today);
        return;
    }

    // Skip header
    char line[256];
    fgets(line, sizeof(line), file);

    TaskList* head = NULL;
    TaskList* tail = NULL;

    // Read tasks into linked list
    while (fgets(line, sizeof(line), file)) {
        char tempTitle[50], tempBegin[25], tempEnd[25], tempLocation[50];
        if (sscanf(line, "%49[^,],%24[^,],%24[^,],%49[^\n]",
                   tempTitle, tempBegin, tempEnd, tempLocation) == 4) {
            TaskList* node = malloc(sizeof(TaskList));
            if (!node) continue;

            node->task.title = strdup(tempTitle);
            node->task.location = strdup(tempLocation);

            // Use %lld for DateTime (time_t or long long int)
            sscanf(tempBegin, "%lld", &node->task.setBegin);  
            sscanf(tempEnd, "%lld", &node->task.setEnd);      

            node->next = NULL;
            node->prev = tail;

            if (tail)
                tail->next = node;
            else
                head = node;
            tail = node;
        }
    }
    fclose(file);

    char titleToRemove[50];
    printf("Enter the task title to remove: ");
    scanf(" %49[^\n]", titleToRemove);

    TaskList* current = head;
    int found = 0;
    while (current) {
        if (strcmp(current->task.title, titleToRemove) == 0) {
            found = 1;

            if (current->prev)
                current->prev->next = current->next;
            else
                head = current->next;

            if (current->next)
                current->next->prev = current->prev;
            else
                tail = current->prev;

            free(current->task.title);
            free(current->task.location);
            TaskList* temp = current;
            current = current->next;
            free(temp);
            break;
        } else {
            current = current->next;
        }
    }

    if (!found) {
        printf("Task with title \"%s\" not found.\n", titleToRemove);
        return;
    }

    // Rewrite file with remaining tasks
    file = fopen(fullPath.path, "w");
    if (!file) {
        printf("Unable to rewrite file.\n");
        return;
    }

    fprintf(file, "Title,Date,Begin,End,Location\n");
    current = head;
    while (current) {
        char beginStr[64], endStr[64];
        
        // Convert DateTime (time_t) to human-readable format
        struct tm* beginTime = localtime(&current->task.setBegin);
        strftime(beginStr, sizeof(beginStr), "%Y-%m-%d %H:%M", beginTime);

        struct tm* endTime = localtime(&current->task.setEnd);
        strftime(endStr, sizeof(endStr), "%Y-%m-%d %H:%M", endTime);

        // Write task data to file
        fprintf(file, "%s,%s,%s,%s\n",
                current->task.title,
                //today,  // You may want to modify this for date formatting.
                beginStr,
                endStr,
                current->task.location);

        TaskList* temp = current;
        current = current->next;
        free(temp->task.title);
        free(temp->task.location);
        free(temp);
    }

    fclose(file);
    printf("Task \"%s\" removed successfully.\n", titleToRemove);
}



void showMenu(Auth* auth) {

    while (1) {
        printf("\033[1mCalendar page\033[0m\n\n");
        printf("  [1] Show Today's Calendar\n  [2] Add Task\n  [3] Remove Task\n\n  [e] Exit\n");
        char cmd;
        printf("command : ");
        scanf(" %c", &cmd);
        int32 next = getchar();
        if (next != '\n' && next != EOF) {
            ungetc(next, stdin);
        }
        if (cmd == '1') {
            showTodayCalendar(auth);
            continue;
        }
        else if (cmd == '2') {
            addTask(auth);
            continue;
        }
        else if (cmd == '3') {
            promptAndRemoveTask(auth);
            continue;
        }
        else if (cmd == 'e') {
            printf("Exiting Calendar Page...\n");
            return;
        }
        else {
            printf("Invalid Command. Try Again!!!");
        }
    }
}


