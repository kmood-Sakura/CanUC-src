#ifndef CALENDAR_SERVICE_H
#define CALENDAR_SERVICE_H

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

Status addTaskToMemoryCalendar(Auth* auth, Task* task, Date date);
Status removeTaskFromMemoryCalendar(Auth* auth, const string title, Date date);
Status updateCalendarFile(Auth* auth, Date date);
Status LoadCalendarFromFile(Auth* auth, Date date);
error findCalendarByDate(CalendarList** calendarNode, Auth* auth, Date date);

#endif // CALENDAR_SERVICE_H