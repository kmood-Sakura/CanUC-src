#include "../calendar-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void CalendarPage() {
    Calendar myCalendar;
    myCalendar.taskList = NULL; // Initialize the task list to NULL
    myCalendar.date.year = 2025; // Set to a specific date for demonstration
    myCalendar.date.month = 5;
    myCalendar.date.day = 2;

    int choice;
    char title[256];
    char location[256];
    Task task;

    do {
        printMenu();
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
}