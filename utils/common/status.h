#ifndef STATUS_H
#define STATUS_H

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"

// typedef int8 code; // 0-failed, 1-success, -1-warning

typedef string error;

typedef struct Status {
    code code; // 0-failed, 1-success, -1-warning
    string msg; // message
    string details; // detail message
} Status;

void FreeStatus(Status* status); // free status

Status* createStatus(const uint8 code, const string msg, const string details); // create status
Status* initStatus(); // initialize status

#endif // STATUS_H