#include "../auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initAuth(Auth* auth) {
    auth->studentId = NULL; // Initialize student ID
    auth->password = NULL; // Initialize password
    auth->userData = NULL; // Initialize user data
}

code getAuth(Auth* auth) {
    if (verifyAuth(auth)) {
        return -1;
    }
    auth->studentId = requestString(MAX_STUDENT_ID, "studentId");
    if(auth->studentId == NULL) {
        return 0;
    }
    auth->password = requestString(MAX_PASSWORD_LEN, "password");
    if(auth->password == NULL) {
        auth->studentId = NULL;
        return 0;
    }
    return 1; // Success
}

code verifyAuth(const Auth* auth) {
    if (auth == NULL) {
        return 0; // Invalid auth
    }
    if (auth->studentId == NULL || auth->password == NULL) {
        return 0; // Invalid student ID or password
    }
    return 1; // Success
}

void FreeAuth(Auth* auth) {
    if (auth == NULL) {
        return; // Invalid auth
    }
    if (auth->studentId != NULL) {
        freeString(auth->studentId); // Free student ID
    }
    if (auth->password != NULL) {
        freeString(auth->password); // Free password
    }
    if (auth->userData != NULL) {
        FreeUserData(auth->userData); // Free user data
    }
    free(auth); // Free auth structure
}

void ResetAuth(Auth* auth) {
    if (auth == NULL) {
        return; // Invalid auth
    }
    if (auth->studentId != NULL) {
        freeString(auth->studentId); // Free student ID
        auth->studentId = NULL; // Reset student ID
    }
    if (auth->password != NULL) {
        freeString(auth->password); // Free password
        auth->password = NULL; // Reset password
    }
    if (auth->userData != NULL) {
        FreeUserData(auth->userData); // Free user data
        auth->userData = NULL; // Reset user data
    }
}

void FreeUserData(UserData* userData) {
    if (userData == NULL) {
        return; // Invalid user data
    }
    if (userData->notifications != NULL) {
        FreeNotification(userData->notifications); // Free notifications
    }
    if (userData->calendars != NULL) {
        FreeCalendar(userData->calendars); // Free calendars
    }
    if (userData->leb2 != NULL) {
        FreeLEB2(userData->leb2); // Free LEB2 data
    }
    free(userData); // Free user data structure
}