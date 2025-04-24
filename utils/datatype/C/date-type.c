#include "../date-type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Utility functions */
static const uint8 daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static uint8 isLeapYear(uint16 year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static uint8 getDaysInMonth(uint8 month, uint16 year) {
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return daysInMonth[month];
}

/* Time-tm conversion functions - Removed unused static functions */

/*
 * Log Functions
 */

void logDate(const Date* date) {
    if (date == NULL) {
        printf("Date: NULL\n");
        return;
    }
    printf("Date: ");
    printf(DATE_LOG_FORMAT, date->year, date->month, date->day);
}

void logClock(const Clock* clock) {
    if (clock == NULL) {
        printf("Clock: NULL\n");
        return;
    }
    printf("Clock: ");
    printf(CLOCK_LOG_FORMAT, clock->hour, clock->minute, clock->second);
}

void logTime(const Time* time) {
    if (time == NULL) {
        printf("Time: NULL\n");
        return;
    }
    printf("Time: ");
    printf(TIME_LOG_FORMAT, time->year, time->month, time->day,
           time->hour, time->minute, time->second, time->millisecond);
}

void logDateTimeInfo(const DateTimeInfo* info) {
    if (info == NULL) {
        printf("DateTimeInfo: NULL\n");
        return;
    }
    
    printf("DateTimeInfo: ");
    printf(DATETIME_LOG_FORMAT, info->year, info->month, info->day,
           info->hour, info->minute, info->second, info->utc);
}

void logTimeValue(TimeValue value) {
    uint16 ms = value % 1000;
    value /= 1000;
    uint8 sec = value % 100;
    value /= 100;
    uint8 min = value % 100;
    value /= 100;
    uint8 hour = value % 100;
    value /= 100;
    uint8 day = value % 100;
    value /= 100;
    uint8 month = value % 100;
    value /= 100;
    uint16 year = value;
    
    printf("TimeValue: ");
    printf(TIMEVALUE_LOG_FORMAT, year, month, day, hour, min, sec, ms);
}

void logDateTime(DateTime dateTime) {
    char buffer[64];
    struct tm* timeInfo = localtime(&dateTime);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
    printf("DateTime: %s (%lld)\n", buffer, (long long)dateTime);
}

/*
 * DateTime Retrieval Functions
 */

DateTime getCurrentDateTime() {
    return time(NULL);
}

DateTime getDateTimeFromComponents(uint16 year, uint8 month, uint8 day, 
                                  uint8 hour, uint8 minute, uint8 second) {
    struct tm timeComponents = {0};
    timeComponents.tm_year = year - 1900;  // years since 1900
    timeComponents.tm_mon = month - 1;     // months since January (0-11)
    timeComponents.tm_mday = day;
    timeComponents.tm_hour = hour;
    timeComponents.tm_min = minute;
    timeComponents.tm_sec = second;
    timeComponents.tm_isdst = -1;          // let system determine DST
    
    return mktime(&timeComponents);
}

/*
 * Data Type Conversion Functions
 */

Date* dateTimeToDate(DateTime dt) {
    struct tm* timeInfo = localtime(&dt);
    
    Date* date = (Date*)malloc(sizeof(Date));
    if (date == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    date->year = timeInfo->tm_year + 1900;
    date->month = timeInfo->tm_mon + 1;
    date->day = timeInfo->tm_mday;
    
    return date;
}

Clock* dateTimeToClock(DateTime dt) {
    struct tm* timeInfo = localtime(&dt);
    
    Clock* clock = (Clock*)malloc(sizeof(Clock));
    if (clock == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    clock->hour = timeInfo->tm_hour;
    clock->minute = timeInfo->tm_min;
    clock->second = timeInfo->tm_sec;
    
    return clock;
}

Time* dateTimeToTime(DateTime dt) {
    struct tm* timeInfo = localtime(&dt);
    
    Time* time = (Time*)malloc(sizeof(Time));
    if (time == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    time->year = timeInfo->tm_year + 1900;  // Convert from years since 1900 to full year
    time->month = timeInfo->tm_mon + 1;
    time->day = timeInfo->tm_mday;
    time->hour = timeInfo->tm_hour;
    time->minute = timeInfo->tm_min;
    time->second = timeInfo->tm_sec;
    time->millisecond = 0;                 // DateTime doesn't have milliseconds
    
    return time;
}

DateTimeInfo* dateTimeToDateTimeInfo(DateTime dt) {
    struct tm* timeInfo = localtime(&dt);
    
    // Calculate UTC offset
    time_t now = time(NULL);
    struct tm* gmt = gmtime(&now);
    struct tm* local = localtime(&now);
    
    // Create temporary tm structures for comparison
    struct tm local_tm = *local;
    struct tm gmt_tm = *gmt;
    
    // Reset DST flags to avoid inconsistencies
    local_tm.tm_isdst = 0;
    gmt_tm.tm_isdst = 0;
    
    // Use noon time to avoid DST issues
    local_tm.tm_hour = 12;
    local_tm.tm_min = 0;
    local_tm.tm_sec = 0;
    
    gmt_tm.tm_hour = 12;
    gmt_tm.tm_min = 0;
    gmt_tm.tm_sec = 0;
    
    time_t local_time = mktime(&local_tm);
    time_t gmt_time = mktime(&gmt_tm);
    
    // Calculate difference in hours
    int8 hour_diff = (int8)((local_time - gmt_time) / 3600);
    uint8 utc_offset = (hour_diff < 0) ? (uint8)(-hour_diff) : (uint8)hour_diff;
    
    DateTimeInfo* info = (DateTimeInfo*)malloc(sizeof(DateTimeInfo));
    if (info == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    info->year = timeInfo->tm_year + 1900;
    info->month = timeInfo->tm_mon + 1;
    info->day = timeInfo->tm_mday;
    info->hour = timeInfo->tm_hour;
    info->minute = timeInfo->tm_min;
    info->second = timeInfo->tm_sec;
    info->utc = utc_offset;
    
    return info;
}

/*
 * Memory Management Functions
 */

void freeDate(Date* date) {
    if (date != NULL) {
        free(date);
    }
}

void freeClock(Clock* clock) {
    if (clock != NULL) {
        free(clock);
    }
}

void freeTime(Time* time) {
    if (time != NULL) {
        free(time);
    }
}

void freeDateTimeInfo(DateTimeInfo* info) {
    if (info != NULL) {
        free(info);
    }
}

/*
 * Additional Functions for Time
 */

Time* createTime(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second, uint16 millisecond) {
    if (month < 1 || month > 12 || day < 1 || day > getDaysInMonth(month, year) ||
        hour > 23 || minute > 59 || second > 59 || millisecond > 999) {
        return NULL;
    }
    
    Time* newTime = (Time*)malloc(sizeof(Time));
    if (newTime == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    newTime->year = year;
    newTime->month = month;
    newTime->day = day;
    newTime->hour = hour;
    newTime->minute = minute;
    newTime->second = second;
    newTime->millisecond = millisecond;
    
    return newTime;
}

Time* getCurrentTime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    DateTime t = ts.tv_sec;
    struct tm* timeInfo = localtime(&t);
    
    uint16 millisecond = ts.tv_nsec / 1000000;  // Convert nanoseconds to milliseconds
    
    return createTime((uint16)(timeInfo->tm_year + 1900),  // Full year
                     (uint8)(timeInfo->tm_mon + 1),
                     (uint8)timeInfo->tm_mday, 
                     (uint8)timeInfo->tm_hour,
                     (uint8)timeInfo->tm_min, 
                     (uint8)timeInfo->tm_sec, 
                     millisecond);
}

Time* copyTime(const Time* time) {
    if (time == NULL) {
        return NULL;
    }
    
    return createTime(time->year, time->month, time->day, 
                     time->hour, time->minute, time->second, 
                     time->millisecond);
}

/*
 * Additional Functions for Date
 */

Date* createDate(uint8 day, uint8 month, uint16 year) {
    if (month < 1 || month > 12) {
        return NULL;
    }
    
    uint8 maxDays = getDaysInMonth(month, year);
    if (day < 1 || day > maxDays) {
        return NULL;
    }
    
    Date* newDate = (Date*)malloc(sizeof(Date));
    if (newDate == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    newDate->day = day;
    newDate->month = month;
    newDate->year = year;
    
    return newDate;
}

Date* getCurrentDate() {
    DateTime t = time(NULL);
    struct tm* timeInfo = localtime(&t);
    
    return createDate((uint8)timeInfo->tm_mday, 
                      (uint8)(timeInfo->tm_mon + 1), 
                      (uint16)(timeInfo->tm_year + 1900));
}

Date* copyDate(const Date* date) {
    if (date == NULL) {
        return NULL;
    }
    
    return createDate(date->day, date->month, date->year);
}

/*
 * Additional Functions for Clock
 */

Clock* createClock(uint8 hour, uint8 minute, uint8 second) {
    if (hour > 23 || minute > 59 || second > 59) {
        return NULL;
    }
    
    Clock* newClock = (Clock*)malloc(sizeof(Clock));
    if (newClock == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    newClock->hour = hour;
    newClock->minute = minute;
    newClock->second = second;
    
    return newClock;
}

Clock* getCurrentClock() {
    DateTime t = time(NULL);
    struct tm* timeInfo = localtime(&t);
    
    return createClock((uint8)timeInfo->tm_hour, 
                       (uint8)timeInfo->tm_min, 
                       (uint8)timeInfo->tm_sec);
}

Clock* copyClock(const Clock* clock) {
    if (clock == NULL) {
        return NULL;
    }
    
    return createClock(clock->hour, clock->minute, clock->second);
}

/*
 * Additional Functions for DateTimeInfo
 */

DateTimeInfo* createDateTimeInfo(uint16 year, uint8 month, uint8 day, uint8 hour, uint8 minute, uint8 second, uint8 utc) {
    if (month < 1 || month > 12 || day < 1 || day > getDaysInMonth(month, year) ||
        hour > 23 || minute > 59 || second > 59 || utc > 12) {
        return NULL;
    }
    
    DateTimeInfo* newInfo = (DateTimeInfo*)malloc(sizeof(DateTimeInfo));
    if (newInfo == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    newInfo->year = year;
    newInfo->month = month;
    newInfo->day = day;
    newInfo->hour = hour;
    newInfo->minute = minute;
    newInfo->second = second;
    newInfo->utc = utc;
    
    return newInfo;
}

DateTimeInfo* getCurrentDateTimeInfo() {
    DateTime now = time(NULL);
    return dateTimeToDateTimeInfo(now);
}

DateTimeInfo* copyDateTimeInfo(const DateTimeInfo* info) {
    if (info == NULL) {
        return NULL;
    }
    
    return createDateTimeInfo(info->year, info->month, info->day, 
                             info->hour, info->minute, info->second, 
                             info->utc);
}

/*
 * TimeValue Functions
 */

TimeValue createTimeValue(uint16 year, uint8 month, uint8 day, 
                        uint8 hour, uint8 minute, uint8 second, uint16 millisecond) {
    return  (TimeValue)year * 10000000000ULL +
            (TimeValue)month * 100000000ULL +
            (TimeValue)day * 1000000ULL +
            (TimeValue)hour * 10000ULL +
            (TimeValue)minute * 100ULL +
            (TimeValue)second * 1ULL +
            (TimeValue)millisecond;         // Include milliseconds directly
}

TimeValue getCurrentTimeValue() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    
    DateTime t = ts.tv_sec;
    struct tm* timeInfo = localtime(&t);
    
    uint16 millisecond = ts.tv_nsec / 1000000;  // Convert nanoseconds to milliseconds
    
    return createTimeValue(timeInfo->tm_year + 1900, 
                          timeInfo->tm_mon + 1, 
                          timeInfo->tm_mday,
                          timeInfo->tm_hour, 
                          timeInfo->tm_min, 
                          timeInfo->tm_sec, 
                          millisecond);
}

TimeValue timeToTimeValue(const Time* time) {
    if (time == NULL) {
        return 0;
    }
    
    return createTimeValue(time->year, 
                          time->month, 
                          time->day,
                          time->hour, 
                          time->minute, 
                          time->second, 
                          time->millisecond);
}

Time* timeValueToTime(TimeValue value) {
    uint16 ms = value % 1000;
    value /= 1000;
    uint8 sec = value % 100;
    value /= 100;
    uint8 min = value % 100;
    value /= 100;
    uint8 hour = value % 100;
    value /= 100;
    uint8 day = value % 100;
    value /= 100;
    uint8 month = value % 100;
    value /= 100;
    uint16 year = value;
    
    return createTime(year, month, day, hour, min, sec, ms);
}

/*
 * String Conversion Functions
 */

char* dateToString(const Date* date) {
    if (date == NULL) {
        return NULL;
    }
    
    char* str = (char*)malloc(12 * sizeof(char));  // YYYY-MM-DD\0 (increased from 11 to 12)
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    sprintf(str, DATE_FORMAT, date->year, date->month, date->day);
    return str;
}

char* clockToString(const Clock* clock) {
    if (clock == NULL) {
        return NULL;
    }
    
    char* str = (char*)malloc(10 * sizeof(char));  // HH:MM:SS\0 (increased from 9 to 10)
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    sprintf(str, CLOCK_FORMAT, clock->hour, clock->minute, clock->second);
    return str;
}

char* timeToString(const Time* time) {
    if (time == NULL) {
        return NULL;
    }
    
    char* str = (char*)malloc(30 * sizeof(char));  // YYYY-MM-DD HH:MM:SS.mmm\0 (increased from 24 to 30)
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    sprintf(str, TIME_FORMAT, 
            time->year, time->month, time->day,
            time->hour, time->minute, time->second,
            time->millisecond);
    
    return str;
}

char* dateTimeInfoToString(const DateTimeInfo* info) {
    if (info == NULL) {
        return NULL;
    }
    
    char* str = (char*)malloc(35 * sizeof(char));  // YYYY-MM-DD HH:MM:SS UTC+X\0 (increased from 30 to 35)
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    sprintf(str, DATETIME_FORMAT, 
            info->year, info->month, info->day,
            info->hour, info->minute, info->second,
            info->utc);
    
    return str;
}

char* timeValueToString(TimeValue value) {
    char* str = (char*)malloc(20 * sizeof(char));  // YYYYMMDDHHMMSSmmm\0 (increased from 18 to 20)
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    uint16 ms = value % 1000;
    value /= 1000;
    uint8 sec = value % 100;
    value /= 100;
    uint8 min = value % 100;
    value /= 100;
    uint8 hour = value % 100;
    value /= 100;
    uint8 day = value % 100;
    value /= 100;
    uint8 month = value % 100;
    value /= 100;
    uint16 year = value;
    
    sprintf(str, TIMEVALUE_FORMAT, 
            year, month, day, hour, min, sec, ms);
    
    return str;
}

char* dateTimeToString(DateTime dt) {
    char* str = (char*)malloc(20 * sizeof(char));  // YYYY-MM-DD HH:MM:SS\0
    if (str == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    struct tm* timeInfo = localtime(&dt);
    strftime(str, 20, "%Y-%m-%d %H:%M:%S", timeInfo);
    
    return str;
}