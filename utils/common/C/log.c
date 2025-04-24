#include "../log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Log(const Status* status) {
    if (status == NULL) {
        fprintf(stderr, "Status is NULL\n");
        return;
    }
    switch (status->code) {
        case 0:
            Error(status->msg);
            break;
        case 1:
            Success(status->msg);
            break;
        case -1:
            Warning(status->msg);
            break;
        default:
            Error("Unknown status code");
            break;
    }
}

void Error(const string msg) {
    if (msg == NULL) {
        fprintf(stderr, "Error: Message is NULL\n");
        return;
    }
    fprintf(stderr, "Error: %s\n", msg);
}

void Warning(const string msg) {
    if (msg == NULL) {
        fprintf(stderr, "Warning: Message is NULL\n");
        return;
    }
    fprintf(stderr, "Warning: %s\n", msg);
}

void Success(const string msg) {
    if (msg == NULL) {
        fprintf(stderr, "Success: Message is NULL\n");
        return;
    }
    printf("Success: %s\n", msg);
}

void LogMsg(const string msg) {
    if (msg == NULL) {
        fprintf(stderr, "Message is NULL\n");
        return;
    }
    printf("%s\n", msg);
}

void Details(const string detail) {
    if (detail == NULL) {
        // fprintf(stderr, "Detail is NULL\n");
        return;
    }
    printf("Details: %s\n", detail);
}

void LogStatus(const Status* status) {
    if (status == NULL) {
        fprintf(stderr, "Status is NULL\n");
        return;
    }
    switch (status->code) {
        case 0:
            LogMsg("Status Failed");
            Error(status->msg);
            Details(status->details);
            break;
        case 1:
            LogMsg("Status Success");
            Success(status->msg);
            Details(status->details);
            break;
        case -1:
            LogMsg("Status Warning");
            Warning(status->msg);
            Details(status->details);
            break;
        default:
            Error("Unknown status code");
            break;
    }
}

void LogStatusMsg(const Status* status) {
    switch(status->code) {
        case 0:
            return Error(status->msg);
        case 1:
            return Success(status->msg);
        case -1:
            return Warning(status->msg);
        default:
            return;
    }
}

code LogFetal(const Status* status) {
    if (status == NULL) {
        Error("Status is NULL");
        return 0; // Invalid status
    }
    switch(status->code) {
        case 0:
            LogMsg("Status Failed");
            Error(status->msg);
            return 0;
        case 1:
            // return Success(status->msg);
            return 1;
        case -1:
            LogMsg("Status Warning");
            Warning(status->msg);
            return -1;
        default:
            Error("Unknown status code");
            return 0;
    }
    return 0;
}