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
        code status = LoginField(auth); 
        if (status != 1) {
            FreeAuthContent(auth);
            return status;
        }
        if (Login(auth)) {
            LogMsg("Login successful"); // Log success message
            return 1; // Login successful
        }
        LogMsg("Login failed, please try again"); // Log failure message
        FreeAuthContent(auth); // Free auth data
        times++;
    }
    
    return 0; // Failed to login after 3 attempts
}

code LoginField(Auth* auth) {
    if (auth->studentId != NULL || auth->password != NULL) {
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
    Status status;
    initStatus(&status); // Initialize status structure
    status = LoadUserDataAPI(auth); // Load user data API

    if (!LogFatal(&status)) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        return 0; // Failed to load user data
    }
    // error err = NULL;
    return 1; // Success
}

code Logout(Auth* auth) {
    if (auth == NULL) {
        return 0; // Invalid auth data
    }
    FreeAuthContent(auth); // Free auth data
    return 1; // Success
}

//update