#include "../calendar.h"

#include <stdio.h>
#include <stdlib.h>

Calendar* createCalendar(Date date, Task* task) {
    Calendar* calendar = (Calendar*)malloc(sizeof(Calendar));
    if (calendar == NULL) {
        fprintf(stderr, "Memory allocation failed for Calendar\n");
        exit(EXIT_FAILURE);
    }
    calendar->date = date;
    calendar->tasks = task;
    return calendar;
}

Task* createTask(string title, string location, DateTime setBegin, DateTime setEnd, DateTime dueDate) {
    Task* task = (Task*)malloc(sizeof(Task));
    if (task == NULL) {
        fprintf(stderr, "Memory allocation failed for Task\n");
        exit(EXIT_FAILURE);
    }
    task->title = title;
    task->location = location;
    task->setBegin = setBegin;
    task->setEnd = setEnd;
    task->dueDate = dueDate;
    return task;
}

void FreeCalendar(Calendar* calendar) {
    if (calendar == NULL) return;
    FreeTask(calendar->tasks);
    free(calendar);
}

void FreeTask(Task* task) {
    if (task == NULL) return;
    free(task);
}