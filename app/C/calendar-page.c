#include "../calendar-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CalendarPage(Auth* auth) {
    if (auth == NULL) return;
    if (auth->userData == NULL) return;

    char cmd;
    
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mCalendar\033[0m\n\n");
        
        printf("  [1] Today's Task\n");
        printf("  [2] Add Task\n");
        printf("  [3] Remove Task\n");
        printf("  [4] Search by Date\n\n");
        printf("  [b] Back\n\n");
        
        if(!requestCommand(&cmd)) {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        Status status;
        switch (cmd) {
            case '1':
                showTodayCalendar(auth);
                if (status.code != 1) {
                    printf("\n\033[0;31mNo tasks found for this date.\033[0m\n");
                }
                break;
            case '2':
                status = addTask(auth);
                if (status.code != 1) {
                    printf("\n\033[0;31mFailed to add task: %s\033[0m\n", status.details);
                }
                break;
            case '3':
                status = promptAndRemoveTask(auth);
                if (status.code != 1) {
                    printf("\n\033[0;31mFailed to remove task: %s\033[0m\n", status.details);
                }
                break;
            case '4':
                status = navigateToDate(auth);
                if (status.code != 1) {
                    printf("\n\033[0;31mFailed to navigate to date: %s\033[0m\n", status.details);
                }
                break;
            case 'b':
                return;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }
    
    return;
}

Status showTodayCalendar(Auth* auth) {
    Date today = getTodayDate();
    error err = showCalendarForDate(auth, today);
    if (err != NULL) {
        return SetStatus(0, "Failed to show today's calendar", err);
    }
    return SetStatus(1, "Today's calendar displayed successfully", NULL);
}

error showCalendarForDate(Auth* auth, Date date) {
    if (!auth || !auth->userData) {
        return "Invalid user data";
    }

    string dateStr = NULL;
    error err = NULL;
    err = dateToString(&dateStr, &date);
    if (err != NULL) {
        return err;
    }

    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mCalendar for %s\033[0m\n\n", dateStr);

    Status status = LoadCalendarFromFile(auth, date);
    if (status.code != 1) {
        return status.msg;
    }

    CalendarList* calendarNode = NULL;
    err = findCalendarByDate(&calendarNode, auth, date);
    
    if (err == NULL && calendarNode && calendarNode->calendar.taskList) {
        printCalendarTasks(&calendarNode->calendar);
    } else {
        printf("\n\033[0;31mNo tasks found for this date.\033[0m\n");
    }
    
    printf("\nPress Enter to continue...");
    getchar();
    
    return NULL;
}

Status addTask(Auth* auth) {
    if (!auth || !auth->userData) {
        return SetStatus(0, "Invalid user data", "Invalid user data");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mAdd New Task\033[0m\n\n");
    
    Task* newTask = NULL;
    error err = allocateTask(&newTask);
    if (err != NULL) {
        return SetStatus(0, "Failed to allocate task", err);
    }
    
    string taskTitle = NULL;
    code result = requestString(&taskTitle, 50, "Enter Task Title");
    if (result != 1 || taskTitle == NULL) {
        free(newTask);
        return SetStatus(0, "Invalid task title", "Failed to get task title");
    }
    string taskLocation = NULL;
    result = requestString(&taskLocation, 50, "Enter Location");
    if (result != 1 || taskLocation == NULL) {
        FreeString(&taskTitle);
        free(newTask);
        return SetStatus(0, "Invalid location", "Failed to get task location");
    }
    
    string dateInput = NULL;
    result = requestString(&dateInput, 20, "Enter Date (YYYY-MM-DD)");
    if (result != 1 || dateInput == NULL) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        free(newTask);
        return SetStatus(0, "Invalid date input", "Failed to get date");
    }
    
    Date taskDate;
    if (sscanf(dateInput, "%hu-%hhu-%hhu", &taskDate.year, &taskDate.month, &taskDate.day) != 3) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        FreeString(&dateInput);
        free(newTask);
        return SetStatus(0, "Invalid date format", "Date format should be YYYY-MM-DD");
    }
    FreeString(&dateInput);
    
    string timeInput = NULL;
    result = requestString(&timeInput, 10, "Enter Start Time (HH MM)");
    if (result != 1 || timeInput == NULL) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        free(newTask);
        return SetStatus(0, "Invalid start time", "Failed to get start time");
    }
    
    int startH, startM;
    if (sscanf(timeInput, "%d %d", &startH, &startM) != 2) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        FreeString(&timeInput);
        free(newTask);
        return SetStatus(0, "Invalid time format", "Time format should be HH MM");
    }
    FreeString(&timeInput);
    
    timeInput = NULL;
    result = requestString(&timeInput, 10, "Enter End Time (HH MM)");
    if (result != 1 || timeInput == NULL) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        free(newTask);
        return SetStatus(0, "Invalid end time", "Failed to get end time");
    }
    
    int endH, endM;
    if (sscanf(timeInput, "%d %d", &endH, &endM) != 2) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        FreeString(&timeInput);
        free(newTask);
        return SetStatus(0, "Invalid time format", "Time format should be HH MM");
    }
    FreeString(&timeInput);
    
    Status status = createTaskEntry(&newTask, taskTitle, taskLocation, taskDate, startH, startM, endH, endM);
    if (!status.code) {
        FreeString(&taskTitle);
        FreeString(&taskLocation);
        free(newTask);
        return status;
    }
    
    status = addTaskToMemoryCalendar(auth, newTask, taskDate);
    if (!status.code) {
        FreeTaskContent(newTask);
        free(newTask);
        return status;
    }
    status = updateCalendarFile(auth, taskDate);
    if (!LogFatal(&status)) {
        LogMsg("Failed to update calendar file after adding task");
    }
    printf("\n\033[0;32mTask '%s' added successfully.\033[0m\n", newTask->title);
    
    return SetStatus(1, "Task added successfully", NULL);
}

Status createTaskEntry(Task** task, string title, string location, Date date, int startHour, int startMinute, int endHour, int endMinute) {
    if (!task || !(*task) || !title || !location) {
        return SetStatus(0, "Invalid task parameters", "Task, title, or location is NULL");
    }
    
    error err = createTask(*task, title, location, makeDateTime(date, startHour, startMinute), makeDateTime(date, endHour, endMinute), makeDateTime(date, endHour, endMinute));
    if (err != NULL) {
        return SetStatus(0, "Failed to create task", err);
    }
    
    return SetStatus(1, "Task created successfully", NULL);
}

Status promptAndRemoveTask(Auth* auth) {
    if (!auth || !auth->userData) {
        return SetStatus(0, "Invalid user data", "Auth or userData is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mRemove Task\033[0m\n\n");
    
    // Ask for date first
    string dateInput = NULL;
    code result = requestString(&dateInput, 20, "Enter Date (YYYY-MM-DD)");
    if (result != 1 || dateInput == NULL) {
        return SetStatus(0, "Invalid date input", "Failed to get date");
    }
    
    Date taskDate;
    if (sscanf(dateInput, "%hu-%hhu-%hhu", &taskDate.year, &taskDate.month, &taskDate.day) != 3) {
        FreeString(&dateInput);
        return SetStatus(0, "Invalid date format", "Date format should be YYYY-MM-DD");
    }
    FreeString(&dateInput);
    
    // Load calendar for the specified date
    Status status = LoadCalendarFromFile(auth, taskDate);
    if (!status.code) {
        LogMsg(status.msg);
        return SetStatus(0, "Failed to load calendar for this date", "No tasks found for this date");
    }
    
    // Show calendar to help user choose which task to remove
    error err = showCalendarForDate(auth, taskDate);
    if (err != NULL) {
        return SetStatus(0, "Failed to show calendar", err);
    }

    string titleToRemove = NULL;
    result = requestString(&titleToRemove, 50, "\nEnter task title to remove "); // Don't prompt again
    if (result != 1 || titleToRemove == NULL) {
        return SetStatus(0, "Invalid task title", "Failed to get task title");
    }
    
    // Try to remove the task
    status = removeTaskFromMemoryCalendar(auth, titleToRemove, taskDate);
    if (!status.code) {
        printf("\n\033[0;31mFailed to remove task: %s\033[0m\n", status.msg);
        FreeString(&titleToRemove);
        return status;
    }
    
    // Update the file with the current tasks
    status = updateCalendarFile(auth, taskDate);
    if (!status.code) {
        LogMsg(status.msg);
        printf("\n\033[0;33mTask removed from memory but failed to update file: %s\033[0m\n", 
               status.details);
    } else {
        printf("\n\033[0;32mTask '%s' removed successfully.\033[0m\n", titleToRemove);
    }
    
    FreeString(&titleToRemove);
    
    return SetStatus(1, "Task removed successfully", NULL);
}

void printCalendarTasks(Calendar* calendar) {
    if (!calendar || !calendar->taskList) {
        printf("\n\033[0;31mNo tasks for this date.\033[0m\n");
        return;
    }

    printf("\n+----------------+------------+--------------+-------------+--------------------------------+\n");
    printf("| Task Title     | Date       | Start Time   | End Time    | Location                       |\n");
    printf("+----------------+------------+--------------+-------------+--------------------------------+\n");
    
    TaskList* current = calendar->taskList;
    while (current) {
        string beginStr = NULL, endStr = NULL;
        error err = dateTimeToString(&beginStr, current->task.setBegin);
        if (err == NULL) {
            err = dateTimeToString(&endStr, current->task.setEnd);
            if (err == NULL) {
                printf("| %-14s | %-10.10s | %-12.5s | %-11.5s | %-30s |\n",
                    current->task.title, 
                    beginStr, 
                    beginStr + 11,
                    endStr + 11,
                    current->task.location);
            }
            FreeString(&beginStr);
        }
        if (endStr != NULL) {
            FreeString(&endStr);
        }
        
        current = current->next;
    }
    
    printf("+----------------+------------+--------------+-------------+--------------------------------+\n");
}

Status navigateToDate(Auth* auth) {
    if (!auth || !auth->userData) {
        return SetStatus(0, "Invalid user data", "Auth or userData is NULL");
    }
    
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mSearch by Date\033[0m\n\n");
    
    string dateInput = NULL;
    code result = requestString(&dateInput, 20, "Enter Date (YYYY-MM-DD)");
    if (result != 1 || dateInput == NULL) {
        return SetStatus(0, "Invalid date input", "Failed to get date");
    }
    
    Date targetDate;
    if (sscanf(dateInput, "%hu-%hhu-%hhu", &targetDate.year, &targetDate.month, &targetDate.day) != 3) {
        FreeString(&dateInput);
        return SetStatus(0, "Invalid date format", "Date format should be YYYY-MM-DD");
    }
    FreeString(&dateInput);
    error err = showCalendarForDate(auth, targetDate);
    if (err != NULL) {
        return SetStatus(0, "Failed to show calendar for the specified date", err);
    }
    return SetStatus(1, "Navigated to date successfully", NULL);
}

Date getTodayDate(void) {
    DateTime currentTime;
    Date today = {0};
    
    error err = getCurrentDateTime(&currentTime);
    if (err != NULL) {
        Error(err);
        return today;
    }
    
    struct tm* tm_info = localtime(&currentTime);
    
    today.year = tm_info->tm_year + 1900;
    today.month = tm_info->tm_mon + 1;
    today.day = tm_info->tm_mday;
    
    return today;
}