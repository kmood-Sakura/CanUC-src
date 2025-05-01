#include "../home-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    LogMsg("Welcome to the Main Page!");

    // 1. Login
    if (!LoginPage(auth)) return;
    LogMsg("Login successful!");

    // 2.fetch
    FetchSystem(auth);

    // 3.loop input command
    char cmd;
    while(requestCommand(&cmd, "e : exit\nl : LEB2\nc : calendar\nn : notification\n")) {
        switch (cmd) {
            case 'l':
                LogMsg("LEB2 module selected!");
                break;
            case 'c':
                LogMsg("Calendar module selected!");
                break;
            case 'n':
                LogMsg("Notification module is not available yet!");
                break;
            default:
                LogMsg("Invalid command, please try again!");
                break;
        }
    }
    LogMsg("Exiting the Home Page...");
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