#include "../home-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void HomePage(Auth* auth) {
    if (auth == NULL) return;
    if (AuthenPage(auth) != 1) return;
    FetchBaseSystem(auth);
    Status status = LoadUserDataAPI(auth);
    if (!LogFatal(&status)) {
        FreeAuthContent(auth);
        FreeStatusContent(&status);
        return;
    }
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mHome\033[0m\n\n");

        printf("  [1] LEB2\n  [2] Calendar\n  [3] Notification\n\n  [e] Exit\n\n");
        char cmd;
        if(!requestCommand(&cmd)){
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        switch (cmd) {
            case '1': LEB2Page(auth);
                break;
            case '2': CalendarPage(auth);
                break;
            case '3': printf("\n\033[0;31mNotification feature is not implemented yet.\033[0m\n");
                break;
            case 'e': 
                printf("\nExiting the system, Byes 🫡\n");
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

    status = LoadAllUserAppDataPathAPI(auth);
    if (!LogFatal(&status)) {
        return;
    }
    DataPath* baseLeb2 = NULL;
    error err = findDataPathByFilename(auth->dataPath, "LEB2", &baseLeb2);
    if (err != NULL) {
        LogMsg("Failed to find LEB2 data path\n");
        return;
    }
    if (baseLeb2->sizeDir == 0) {
        status = MakeTempData(auth);
        if (!LogFatal(&status)) {
            return;
        }
        return;
    }
}
//updatetee
