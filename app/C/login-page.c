#include "../login-page.h"
#include <stdio.h>
#include <stdlib.h>

code AuthenPage(Auth* auth) {
    if (auth == NULL) return 0;
    if (auth->userData != NULL) return -1; // User data already exists

    char cmd;
    while(requestCommand(&cmd, "Please choose your option:\n[1] Login\n[2] Sign up\n[e] Exit\n")){
        switch (cmd)
        {
            case '1': return LoginPage(auth);
            case '2': return SignupPage(auth);
            case 'e': return 0;
            default:
                printf("Invalid command. Please try again.\n");
                continue;
        }
    }
    return 1;
}

code LoginPage(Auth* auth) {
    code code = 0;
    for(uint8 i=0;i<3;i++) {
        if(AuthInputField(auth)){
            LogMsg("Input field success\n");
            code = Login(auth);
            if (code == 1) {
                LogMsg("Login success");
                return 1;
            }
            if (code == -1) return Signup(auth);
            if (code == 0) {
                printf("Login failed. Please try again.\n");
                continue;
            }
        }
        // invalid input -> exit login page
        return 0;
    }
    LogMsg("Too many attempts. Exit Login page\n");
    return AuthenPage(auth); // Too many attempts, return to authentication page
}

code SignupPage(Auth* auth) {
    for(uint8 times=0;times<3;times++){
        if(AuthInputField(auth)){
            LogMsg("Input field success\n");
            if (!Signup(auth)) {
                printf("Sign up failed. Please try again.\n");
                continue;
            }
            LogMsg("Sign up success");
            return 1;
        }
        // invalid input -> exit sign up page
        return AuthenPage(auth);
    }
    LogMsg("Too many attempts. Exit Sign-up page\n");
    return 0; 
}

code AuthInputField(Auth* auth) {
    if (auth->studentId != NULL || auth->password != NULL) {
        return -1; // Invalid auth data
    }
    printf("Please enter your student ID and password (nothing default to exit) \n");
    
    code status = requestString(&auth->studentId, MAX_STUDENT_ID, "Student ID");
    if (status != 1) {
        FreeString(&auth->studentId); // Free student ID string
        printf("Confirm exit\n");
        if (requestConfirm()) return 0;
        return AuthInputField(auth); // Retry input
    }
    status = requestString(&auth->password, MAX_PASSWORD_LEN, "Password");
    if (status != 1) {
        FreeString(&auth->studentId); // Free student ID string
        FreeString(&auth->password); // Free password string
        printf("Confirm exit\n");
        if (requestConfirm()) return 0;
        return AuthInputField(auth); // Retry input
    }
    printf("\n");
    return 1;
}

code Login(Auth* auth) {
    Status status;
    initStatus(&status);
    status = SetUpDataBase(auth); // Setup database
    if (!LogFatal(&status)) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        return 0; // Failed to setup database
    }
    LogMsg("Doing authentication...\n");
    status = Authenticate(auth);
    if (!LogFatal(&status)) {
        FreeAuthContent(auth);
        FreeStatusContent(&status); 
        return 0;
    }
    if(status.code == -1) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        printf("Do you want to sign up?\n");
        if (requestConfirm()) return -1;
        return 0;
    }

    status = LoadUserDataAPI(auth); // Load user data API

    if (!LogFatal(&status)) {
        FreeAuthContent(auth);
        FreeStatusContent(&status);
        return 0;
    }
    return 1; // Success
}

code Signup(Auth* auth) {
    LogMsg("Doing sign up...\n");
    Status status;
    initStatus(&status);
    status = SetUpDataBase(auth); // Setup database
    if (!LogFatal(&status)) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        return 0; // Failed to setup database
    }
    status = SetupAuthenticate(auth); // Setup authentication
    if (!LogFatal(&status)) {
        FreeAuthContent(auth); // Free auth data
        FreeStatusContent(&status); // Free status
        return 0; // Failed to setup authentication
    }
    return 1;
}

code Logout(Auth* auth) {
    if (auth == NULL) {
        return 0; // Invalid auth data
    }
    FreeAuthContent(auth); // Free auth data
    return 1; // Success
}

//update