#ifndef CALENDAR_PAGE_H
#define CALENDAR_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"
#include "../utils/common/request.h"

#include "../utils/struct/calendar.h"
#include "../utils/struct/auth.h"

/*void addTask(Calendar* calendar, Task task);
void removeTask(Calendar* calendar, const char* title);
Task* findTaskByTitle(const Calendar* calendar, const char* title);
void printCalendar(const Calendar* calendar);
void CalendarPage(Auth* auth);
void ShowCalendarPath(Auth* auth);
void printMenu(); */

void showMenu(Auth* auth);
void showTodayCalendar(Auth* auth);
void addTask(Auth* auth);


#endif 