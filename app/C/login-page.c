#include "../login-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

code AuthenPage(Auth* auth) {
    if (auth == NULL) return 0;
    if (auth->userData != NULL) return -1;

    char cmd;
    while(1){
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mAuthentication\033[0m\n\n");
        printf("  [1] Login\n  [2] Sign up\n\n  [e] Exit\n\n");
        printf("  \033[0;93mIf you already have an account please login... please 🥺\033[0m\n\n");
        if(!requestCommand(&cmd)){
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }
        switch (cmd)
        {
            case '1': return LoginPage(auth);
            case '2': return SignupPage(auth);
            case 'e': return 0;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n\n");
                continue;
        }
    }
    return 0;
    return 0;
}

code LoginPage(Auth* auth) {
    code result = 0;
    for(uint8 i=0; i<3; i++) {
        printf("\n--------------------------------------------------------\n\n");
        printf("  \033[1mLogin\033[0m\n\n");
        
        if (auth->studentId != NULL) {
            FreeString(&auth->studentId);
        }
        if (auth->password != NULL) {
            FreeString(&auth->password);
        }
        
        if(AuthInputField(auth)) {
            result = Login(auth);
            if (result == 1) {
                return 1;
            }
            if (result == -1) {
                return Signup(auth);
            }
            if (i < 2) {
                printf("\n\033[0;31mLogin failed. Please try again.\033[0m\n\n");
            }
            continue;
        }
        return 0;
    }
    printf("\n\033[0;31mToo many attempts. Exit Login page\033[0m\n\n");
    
    FreeAuthContent(auth);
    return AuthenPage(auth);
}

code SignupPage(Auth* auth) {
    for(uint8 times=0;times<3;times++){
        printf("\n--------------------------------------------------------\n\n");
        printf("  \033[1mSign up page\033[0m\n\n");
        if(AuthInputField(auth)){
            if (!Signup(auth)) {
                if (times < 2) {
                    printf("\n\033[0;31mSign-up failed. Please try again.\033[0m\n\n");
                }
                continue;
            }
            return 1;
        }
        return AuthenPage(auth);
    }
    printf("\n\033[0;31mToo many attempts. Exit Sign-up page\033[0m\n\n");
    return 0; 
}

code AuthInputField(Auth* auth) {
    if (auth == NULL) {
        return 0;
    }
    
    if (auth->studentId != NULL || auth->password != NULL) {
        FreeString(&auth->studentId);
        FreeString(&auth->password);
    }
    
    printf("\033[0;34mPlease enter your student ID and password (nothing default to exit)\033[0m\n\n");
    
    code status = requestString(&auth->studentId, MAX_STUDENT_ID, "  Student ID");
    if (status != 1) {
        if (auth->studentId != NULL) {
            FreeString(&auth->studentId);
        }
        
        printf("Confirm exit\n");
        if (requestConfirm()) return 0;
        return AuthInputField(auth);
    }
    
    status = requestString(&auth->password, MAX_PASSWORD_LEN, "  Password");
    if (status != 1) {
        if (auth->studentId != NULL) {
            FreeString(&auth->studentId);
        }
        if (auth->password != NULL) {
            FreeString(&auth->password);
        }
        
        printf("Confirm exit\n");
        if (requestConfirm()) return 0;
        return AuthInputField(auth);
    }
    
    return 1;
}

code Login(Auth* auth) {
    Status status;
    initStatus(&status);
    if (auth == NULL) return 0;
    if (auth->dataPath == NULL) {
        status = SetUpDataBase(auth);
        if (!LogFatal(&status)) {
            FreeAuthContent(auth);
            return 0;
        }
    }
    
    status = Authenticate(auth);
    if (status.code != 1) {
        Error(status.details);
        return 0;
    }
    
    if(status.code == -1) {
        // Request Sign up process
        printf("\033[0;34mDo you want to sign up?\033[0m\n\n");
        if (requestConfirm()) return -1;
        return 0;
    }
    
    return 1;
}

code Signup(Auth* auth) {
    Status status;
    initStatus(&status);
    if (auth == NULL) return 0;
    if (auth->dataPath == NULL) {
        status = SetUpDataBase(auth);
        if (!LogFatal(&status)) {
            return 0;
        }
    }
    status = SetupAuthenticate(auth);
    if (!LogFatal(&status)) {
        return 0;
    }
    return 1;
}

code Logout(Auth* auth) {
    if (auth == NULL) {
        return 0;
    }
    FreeAuthContent(auth);
    return 1;
}