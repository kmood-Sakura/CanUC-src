#ifndef CALENDAR_PAGE_H
#define CALENDAR_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"
#include "../utils/common/request.h"

#include "../utils/struct/calendar.h"
#include "../utils/struct/auth.h"
#include "../utils/struct/path.h"
#include "../utils/struct/file.h"

#include "../utils/datatype/date-type.h"
#include "../utils/datatype/string-type.h"
#include "../utils/datatype/int-type.h"

#include "../service/calendar-service.h"

void CalendarPage(Auth* auth);

Status addTask(Auth* auth);
Status showTodayCalendar(Auth* auth);
Status promptAndRemoveTask(Auth* auth);
Status navigateToDate(Auth* auth);

error showCalendarForDate(Auth* auth, Date date);
Status createTaskEntry(Task** task, string title, string location, Date date, int startHour, int startMinute, int endHour, int endMinute);
Date getTodayDate(void);
void printCalendarTasks(Calendar* calendar);


#endif 