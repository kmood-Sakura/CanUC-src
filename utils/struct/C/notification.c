#include "../notification.h"

#include <stdio.h>
#include <stdlib.h>

Notification* createNotification(const string header, const string title, DateTime dueDate) {
    Notification* newNotification = (Notification*)malloc(sizeof(Notification));
    if (newNotification == NULL) {
        fprintf(stderr, "Memory allocation failed for Notification\n");
        return NULL;
    }

    newNotification->header = createString(header);
    newNotification->title = createString(title);
    newNotification->dueDate = dueDate;

    if (newNotification->header == NULL || newNotification->title == NULL) {
        Error("Memory allocation failed for Notification fields");
        free(newNotification->header);
        free(newNotification->title);
        free(newNotification);
        return NULL;
    }

    return newNotification;
}

void FreeNotification(Notification* notification) {
    if (notification != NULL) {
        free(notification);
    }
}