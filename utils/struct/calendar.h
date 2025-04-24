#ifndef CALENDAR_H 
#define CALENDAR_H

#include "../datatype/date-type.h"
#include "../datatype/int-type.h"   
#include "../datatype/string-type.h"

#include "../common/status.h"
#include "../common/log.h"

typedef struct Task {
    string title; // task title
    string location; // task location
    DateTime setBegin;
    DateTime setEnd; // task set date and time
    DateTime dueDate; // task due date (optional to extend setEnd)
} Task;

typedef struct Calendar {
    Date date; // date
    Task* tasks; // list of tasks
} Calendar;

Calendar* createCalendar(Date date, Task* task);
Task* createTask(string title, string location, DateTime setBegin, DateTime setEnd, DateTime dueDate);

void FreeCalendar(Calendar* calendar);
void FreeTask(Task* task);

#endif // CALENDAR_H