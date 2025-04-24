#include "../status.h"
// te
#include <stdio.h>
#include <stdlib.h>

// Base function
Status* createStatus(const uint8 code, const string msg, const string details) {
    Status* status = (Status*)malloc(sizeof(Status));
    if (status == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL; // Failed
    }
    status->code = code;
    status->msg = createString(msg);
    status->details = createString(details);

    return status; // Success
}

Status* initStatus() {
    Status* status = (Status*)malloc(sizeof(Status));
    if (status == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL; // Failed
    }
    status->code = 0; // Default to failed
    status->msg = NULL;
    status->details = NULL;

    return status; // Success
}

void FreeStatus(Status* status) {
    if (status != NULL) {
        if (status->msg != NULL) {
            free(status->msg);
        }
        if (status->details != NULL) {
            free(status->details);
        }
        free(status);
    }
}