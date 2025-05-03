/*#include "../calendar-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CalendarPage(Auth* auth) {
    Calendar myCalendar;
    myCalendar.taskList = NULL; // Initialize the task list to NULL
    myCalendar.date.year = 2025; // Set to a specific date for demonstration
    myCalendar.date.month = 5;
    myCalendar.date.day = 2;

    int choice;
    char title[256];
    Task task;

    do {
        printMenu();
        printf("Calendar Path : ");
        ShowCalendarPath(auth);
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clean the newline character from the input buffer

        switch (choice) {
            case 1: // Add Task
                printf("Enter task title: ");
                fgets(task.title, sizeof(task.title), stdin);
                task.title[strcspn(task.title, "\n")] = 0; // Remove the newline character

                printf("Enter task location: ");
                fgets(task.location, sizeof(task.location), stdin);
                task.location[strcspn(task.location, "\n")] = 0; // Remove the newline character

                printf("Enter task set Begin date and time (timestamp): ");
                scanf("%lld", &task.setBegin); // Reading timestamp as long long
                printf("Enter task set End date and time (timestamp): ");
                scanf("%lld", &task.setEnd); // Reading timestamp as long long
                printf("Enter task due date (timestamp): ");
                scanf("%lld", &task.dueDate); // Reading timestamp as long long

                addTask(&myCalendar, task);
                break;

            case 2: // Remove Task
                printf("Enter task title to remove: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0; // Remove the newline character
                removeTask(&myCalendar, title);
                break;

            case 3: // Print Calendar
                printCalendar(&myCalendar);
                break;

            case 4: // Exit
                printf("Exiting the program.\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    } while (choice != 4);
}

void ShowCalendarPath(Auth* auth) {
    DataPath* parentsOfCalendar = auth->dataPath;
    DataPath* CalendarDataPath;
    Path calendar;
    error err = NULL;
    initPath(&calendar);
    err = createPathLen(&calendar, "Calendar", 8);
    if (err != NULL) {
        Error(err);
        return;
    }
    err = findDataPathByFilename(parentsOfCalendar,calendar, &CalendarDataPath);
    if (err != NULL){
        Error(err);
        return;
    }
    printf("%s\n", CalendarDataPath->path.path);
}

void printMenu() {
    printf("\n--- Calendar Menu ---\n");
    printf("1. Add Task\n");
    printf("2. Remove Task\n");
    printf("3. Print Calendar\n");
    printf("4. Exit\n");
}

// Function to add a task to a calendar
void addTask(Calendar* calendar, Task task) {
    TaskList* newTask = (TaskList*)malloc(sizeof(TaskList));
    if (!newTask) {
        printf("Error: Could not allocate memory for new task.\n");
        return;
    }
    newTask->task = task;
    newTask->next = NULL;
    newTask->prev = NULL;

    if (!calendar->taskList) {
        calendar->taskList = newTask;
    } else {
        TaskList* temp = calendar->taskList;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newTask;
        newTask->prev = temp;
    }
}

// Function to remove a task from a calendar based on its title
void removeTask(Calendar* calendar, const char* title) {
    TaskList* current = calendar->taskList;
    while (current != NULL) {
        if (strcmp(current->task.title, title) == 0) {
            if (current->prev != NULL) {
                current->prev->next = current->next;
            }
            if (current->next != NULL) {
                current->next->prev = current->prev;
            }
            if (current == calendar->taskList) {
                calendar->taskList = current->next;
            }
            free(current);
            printf("Task \"%s\" removed successfully.\n", title);
            return;
        }
        current = current->next;
    }
    printf("Task \"%s\" not found.\n", title);
}

// Function to find a task by its title
Task* findTaskByTitle(const Calendar* calendar, const char* title) {
    TaskList* current = calendar->taskList;
    while (current != NULL) {
        if (strcmp(current->task.title, title) == 0) {
            return &current->task;
        }
        current = current->next;
    }
    return NULL;  // Task not found
}

// Function to print all tasks in a calendar
void printCalendar(const Calendar* calendar) {
    printf("Tasks for date: " DATE_FORMAT "\n", calendar->date.year, calendar->date.month, calendar->date.day);
    TaskList* current = calendar->taskList;
    while (current != NULL) {
        printf("Title: %s, Location: %s\n", current->task.title, current->task.location);
        // Additional task details can be printed here
        current = current->next;
    }
} */


// calendar.c

/*#include "../calendar-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Get today's Date structure
Date getTodayDate() {
    Date date;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    date.year = tm_info->tm_year + 1900;
    date.month = tm_info->tm_mon + 1;
    date.day = tm_info->tm_mday;

    return date;
}

// Get filename from Date (e.g., 2025-05-03.csv)
void getFilenameFromDate(Date date, char *filename) {
    sprintf(filename, DATE_FORMAT ".csv", date.year, date.month, date.day);
}

// Convert DateTime to readable string format (HH:MM:SS)
void formatTime(DateTime dt, char *buffer) {
    struct tm *tm_info = localtime(&dt);
    sprintf(buffer, CLOCK_FORMAT, tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

// Convert DateTime to Date string (YYYY-MM-DD)
void formatDate(DateTime dt, char *buffer) {
    struct tm *tm_info = localtime(&dt);
    sprintf(buffer, DATE_FORMAT, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
}

// Menu loop
void showMenu(Auth* auth) {
    ShowCalendarPath(auth);
    printf("\n");
    int choice;

    while (1) {
        printf("\n==== Calendar Menu ====\n");
        printf("1. Show Calendar for Today\n");
        printf("2. Add Task\n");
        printf("3. Exit\n");
        printf("Choose an option: ");
        scanf("%d", &choice);
        getchar(); // Consume newline

        if (choice == 1) {
            showCalendarForToday();
        } else if (choice == 2) {
            addTask();
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid option. Try again.\n");
        }
    }
}

// Show tasks from today's CSV file
void showCalendarForToday() {
    char filename[30];
    Date today = getTodayDate();
    getFilenameFromDate(today, filename);

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("No tasks found for today.\n");
        return;
    }

    printf("\n--- Tasks for Today (%s) ---\n", filename);
    char line[300];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

// Add task and save to today's CSV
void addTask() {
    Task task;
    char buffer[100];
    char filename[30];

    printf("Enter Task Title: ");
    fgets(buffer, sizeof(buffer), stdin);
    task.title = strdup(strtok(buffer, "\n"));  // remove newline

    printf("Enter Location: ");
    fgets(buffer, sizeof(buffer), stdin);
    task.location = strdup(strtok(buffer, "\n"));

    time_t now = time(NULL);
    task.setBegin = now;

    printf("Enter End Time (HH MM SS): ");
    struct tm end_tm = *localtime(&now);
    scanf("%d %d %d", &end_tm.tm_hour, &end_tm.tm_min, &end_tm.tm_sec);
    getchar(); // consume newline
    task.setEnd = mktime(&end_tm);

    task.dueDate = task.setEnd; // for now, make dueDate same as end

    // Save to file
    Date today = getTodayDate();
    getFilenameFromDate(today, filename);

    FILE *file = fopen(filename, "a");
    if (!file) {
        printf("Failed to open file.\n");
        return;
    }

    char dateStr[20], beginStr[20], endStr[20];
    formatDate(task.setBegin, dateStr);
    formatTime(task.setBegin, beginStr);
    formatTime(task.setEnd, endStr);

    fprintf(file, "%s,%s,%s,%s,%s,%s\n",
            "SUBJECT", task.title, dateStr, beginStr, endStr, task.location);

    fclose(file);
    printf("Task added successfully.\n");
}

void ShowCalendarPath(Auth* auth) {
    DataPath* parentsOfCalendar = auth->dataPath;
    DataPath* CalendarDataPath;
    Path calendar;
    error err = NULL;
    initPath(&calendar);
    err = createPathLen(&calendar, "Calendar", 8);
    if (err != NULL) {
        Error(err);
        return;
    }
    err = findDataPathByFilename(parentsOfCalendar,calendar, &CalendarDataPath);
    if (err != NULL){
        Error(err);
        return;
    }
    printf("%s\n", CalendarDataPath->path.path);
} */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
// for Auth and ShowCalendarPath()
#include "../calendar-page.h"

void getTodayDateString(char* buffer, size_t size) {
    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

void addTask(Auth* auth) {
    char subjectCode[50];
    char title[100];
    char taskDate[20];
    char startTime[10];
    char endTime[10];
    char location[100];

    printf("Enter Subject Code: ");
    scanf(" %[^\n]", subjectCode);

    printf("Enter Task Title: ");
    scanf(" %[^\n]", title);

    printf("Enter Task Date (YYYY-MM-DD): ");
    scanf(" %[^\n]", taskDate);

    printf("Enter Start Time (HH:MM): ");
    scanf(" %[^\n]", startTime);

    printf("Enter End Time (HH:MM): ");
    scanf(" %[^\n]", endTime);

    printf("Enter Location: ");
    scanf(" %[^\n]", location);

    // Get calendar directory path
    DataPath* parentsOfCalendar = auth->dataPath;
    DataPath* CalendarDataPath;
    Path calendar;
    error err = NULL;

    initPath(&calendar);
    err = createPathLen(&calendar, "Calendar", 8);
    if (err != NULL) {
        Error(err);
        return;
    }

    err = findDataPathByFilename(parentsOfCalendar, calendar, &CalendarDataPath);
    if (err != NULL) {
        Error(err);
        return;
    }

    // Build full file path using your system
    Path filename, fullPath;
    err = createPathLen(&filename, strcat(strdup(taskDate), ".csv"), strlen(taskDate) + 4);
    if (err != NULL) {
        Error(err);
        return;
    }

    err = createPathFileorFolder(&fullPath, filename, CalendarDataPath->path);
    if (err != NULL) {
        Error(err);
        return;
    }

    FILE* file = fopen(fullPath.path, "a");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    if (fileSize == 0) {
        fprintf(file, "Subject Code,Task Title,Date,Start Time,End Time,Location\n");
    }

    fprintf(file, "%s,%s,%s,%s,%s,%s\n", subjectCode, title, taskDate, startTime, endTime, location);
    fclose(file);

    printf("Task added successfully to %s\n", fullPath.path);
}

void showTodayCalendar(Auth* auth) {
    char today[20];
    getTodayDateString(today, sizeof(today));

    DataPath* parentsOfCalendar = auth->dataPath;
    DataPath* CalendarDataPath;
    Path calendar;
    error err = NULL;

    initPath(&calendar);
    err = createPathLen(&calendar, "Calendar", 8);
    if (err != NULL) {
        Error(err);
        return;
    }

    err = findDataPathByFilename(parentsOfCalendar, calendar, &CalendarDataPath);
    if (err != NULL) {
        Error(err);
        return;
    }

    Path filename, fullPath;
    err = createPathLen(&filename, strcat(strdup(today), ".csv"), strlen(today) + 4);
    if (err != NULL) {
        Error(err);
        return;
    }

    err = createPathFileorFolder(&fullPath, filename, CalendarDataPath->path);
    if (err != NULL) {
        Error(err);
        return;
    }

    FILE* file = fopen(fullPath.path, "r");
    if (file == NULL) {
        printf("No calendar file found for today (%s).\n", today);
        return;
    }
    
    char line[256];
    /*printf("\n+----------------+----------------------------+--------------+-------------+-------------+----------------------+\n");
    printf("|  Subject Code  |        Task Title         |     Date     |  Start Time |   End Time  |       Location       |\n");*/
    printf("+----------------+----------------------------+--------------+-------------+-------------+----------------------+\n");
    
    while (fgets(line, sizeof(line), file)) {
        string subjectCode = strtok(line, ",");
        string title = strtok(NULL, ",");
        string date = strtok(NULL, ",");
        string startTime = strtok(NULL, ",");
        string endTime = strtok(NULL, ",");
        string location = strtok(NULL, "\n");
    
        if (subjectCode && title && date && startTime && endTime && location) {
            printf("| %-14s | %-26s | %-12s | %-11s | %-11s | %-20s |\n",
                subjectCode, title, date, startTime, endTime, location);
        }
    }
    
    printf("+----------------+----------------------------+--------------+-------------+-------------+----------------------+\n");
    fclose(file);
    
} 


void showMenu(Auth* auth) {
    int choice;

    while (1) {
        printf("\n==== Calendar Menu ====\n");
        printf("1. Show Calendar for Today\n");
        printf("2. Add Task\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                showTodayCalendar(auth);
                break;
            case 2:
                addTask(auth);
                break;
            case 3:
                printf("Exiting...\n");
                return;
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}


