#include "../home-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    // LogMsg("Welcome to the Main Page!");
    if (!LoginPage(auth)) return;
    printf("Welcome, User");

    //FetchSystem(auth);

    printf("[1] LEB2\n[2] Calendar\n[3] Notification\n[e] Exit\n");
    while(1) {
        char cmd;
        printf("\ncommand: ");
        scanf(" %c",&cmd);
        cmd = toupper(cmd);
        switch (cmd){
            case '1': leb2page();
                break;
            // case '2': Calendar(auth);
            //     break;
            // case '3': Notification(auth);
            //     break;
            case 'E': 
                printf("Exiting the system. Goodbye!\n");
                exit(200);
            
            default:
                printf("Invalid Command. Please Enter Again\n");
                break;
        }
    }
}

void FetchSystem(Auth* auth) {
    if (auth->studentId == NULL) return;

    Status status;
    status = SetUpDataBase(auth);
    if (!LogFatal(&status)) {
        return;
    }
    PrintDataPath(auth->dataPath);
    LogMsg("Database setup successful!");
}

//updatetee