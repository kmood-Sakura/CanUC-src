#include "../home-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void HomePage(Auth* auth) {
    // LogMsg("Welcome to the Main Page!");
    if (AuthenPage(auth) != 1) return;
    printf("Welcome, User\n");

    FetchBaseSystem(auth); // Fetch base system data
    LogMsg("Loading user data...\n");
    Status status = LoadUserDataAPI(auth); // Load user data API
    if (!LogFatal(&status)) {
        FreeAuthContent(auth);
        FreeStatusContent(&status);
        return;
    }
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mHome page\033[0m\n\n");

        // FetchSystem(auth);

        printf("  [1] LEB2\n  [2] Calendar\n  [3] Notification\n\n  [e] Exit\n\n");
        char cmd;
        if(!requestCommand(&cmd)){
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        switch (cmd) {
            case '1': LEB2Page(auth);
                break;
            case '2':
                break;
            case '3': 
                break;
            case 'e': 
                printf("\nExiting the system. Goodbye!\n");
                printf("\n--------------------------------------------------------\n\n");
                return;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }    
}

void FetchBaseSystem(Auth* auth) {
    if (auth == NULL) return;
    Status status;
    initStatus(&status);

    status = MakeTempData(auth); // Make temporary data
    if (!LogFatal(&status)) {
        return;
    }
    LogMsg("Temporary data created successfully!\n");
    status = LoadAllUserAppDataPathAPI(auth); // Load all user app data path
    if (!LogFatal(&status)) {
        return;
    }
    LogMsg("Loaded base system data successfully!\n");
    PrintDataPath(auth->dataPath);
    LogMsg("Database load successful!");
}
//updatetee
