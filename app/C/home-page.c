#include "../home-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    printf("--------------------------------------------------------\n\n");

    if (!LoginPage(auth)) return;

    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mHome page\033[0m\n\n");

        // FetchSystem(auth);

        printf("  [1] LEB2\n  [2] Calendar\n  [3] Notification\n\n  [e] Exit\n\n");
        
        char cmd;
        printf("command: ");
        scanf(" %c", &cmd);
        cmd = tolower(cmd);  // Convert input to uppercase for uniformity

        switch (cmd) {
            case '1': 
                leb2page(); 
                break;
            // case '2': Calendar(auth); 
            //     printf("\n--------------------------------------------------------\n\n");
            //     break;
            // case '3': Notification(auth); 
            //     printf("\n--------------------------------------------------------\n\n");
            //     break;
            case 'e': 
                printf("\nExiting the system. Goodbye!\n");
                printf("\n--------------------------------------------------------\n\n");
                exit(200);
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                break;
        }
    }    
}

// Function to fetch system data (Example: database setup, etc.)
void FetchSystem(Auth* auth) {
    if (auth->studentId == NULL) return;

    Status status;
    status = SetUpDataBase(auth);
    if (!LogFatal(&status)) {
        return;
    }
    PrintDataPath(auth->dataPath);
    printf("Database setup successful!");
}
