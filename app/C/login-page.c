#include "../login-page.h"
#include <stdio.h>
#include <stdlib.h>

code LoginPage(Auth* auth) {
    if (auth == NULL) {
        return 0;
    }

    if (auth->userData != NULL) {
        return -1; // Auth already verified
    }
    uint8 times = 0;
    while(times < 3) {
        code status = LoginField(auth); // Get login details
        if (status != 1) {
            FreeAuthContent(auth); // Free auth data
            return status; // Failed to get login details
        }
        status = Login(auth); // Attempt to login
        if (status == 1) return 1; // Success
        FreeAuthContent(auth); // Free auth data
        LogMsg("Login failed, please try again"); // Log failure message

        times++;
    }
    
    return 0; // Failed to login after 3 attempts
}

code LoginField(Auth* auth) {
    if (auth->studentId == NULL || auth->password == NULL) {
        return -1; // Invalid auth data
    }
    code status = requestString(&auth->studentId, MAX_STUDENT_ID, "Student ID");
    if (status != 1) {
        FreeString(&auth->studentId); // Free student ID string
        return status;
    }
    status = requestString(&auth->password, MAX_PASSWORD_LEN, "Password");
    if (status != 1) {
        FreeString(&auth->studentId); // Free student ID string
        FreeString(&auth->password); // Free password string
        return status;
    }
    return 1;
}

code Login(Auth* auth) {
    error err = NULL;
    Status status;
    err = LoadUserDataAPI(auth, &status); // Load user data
    if (err != NULL) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        Error(err); // Log error
        return 0; // Failed to load user data
    }
    if (!LogFatal(&status)) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        return 0; // Failed to load user data
    }
    return 1; // Success
}

code Logout(Auth* auth) {
    if (auth == NULL) {
        return 0; // Invalid auth data
    }
    FreeAuth(auth); // Free auth data
    return 1; // Success
}