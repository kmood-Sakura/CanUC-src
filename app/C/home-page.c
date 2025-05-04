#include "../home-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    // LogMsg("Welcome to the Main Page!");
    if (AuthenPage(auth) != 1) return;
    printf("\033[1mHome page\033[0m\n\n");

    FetchBaseSystem(auth); // Fetch base system data

    printf("  [1] LEB2\n  [2] Calendar\n  [3] Notification\n\n  [e] Exit\n");
    while(1) {
        char cmd;
        printf("\ncommand: ");
        scanf(" %c",&cmd);
        cmd = toupper(cmd);
        switch (cmd){
            case '1': 
                leb2page();
                break;
            case '2': showMenu(auth);
                break;
            case '3': 
                break;
            case 'E': 
                printf("Exiting the system. Goodbye!\n");
                exit(200);
            
            default:
                printf("Invalid Command. Please Enter Again\n");
                break;
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
    status = LoadAllUserAppDataPathAPI(auth); // Load all user app data path
    if (!LogFatal(&status)) {
        return;
    }
    // PrintDataPath(auth->dataPath);
    // LogMsg("Database load successful!");
}
//updatetee