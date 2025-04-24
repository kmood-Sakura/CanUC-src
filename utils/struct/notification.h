#ifndef NOTIFICATION_H 
#define NOTIFICATION_H

#include "../common/status.h"
#include "../common/log.h"

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"
#include "../datatype/date-type.h"

typedef struct Notification {
    string header; // notification title
    string title; // notification message
    DateTime dueDate; // notification date and time
} Notification;

Notification* createNotification(const string header, const string title, DateTime dueDate);
void FreeNotification(Notification* notification);

#endif // NOTIFICATION_H