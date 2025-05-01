#include "../home-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    // LogMsg("Welcome to the Main Page!");

    // 1. Login
<<<<<<< HEAD
    if (!LoginPage(auth)) {
        return;
    }
    LogMsg("Welcome back User\n");
=======
    if (!LoginPage(auth)) return;
    LogMsg("Login successful!");
>>>>>>> 3bda8079c57d01782d3129135f5569d416389602

    // 2.fetch
    //FetchSystem(auth);

    // 3.loop input command
    char cmd;
    cmd = toupper(cmd);
    while(requestCommand(&cmd, "[1] LEB2\n[2] Calendar\n[3] Notification\n[e] Exit\n")) {
        switch (cmd) {
            case '1':
                LogMsg("LEB2 module selected!\n");
                break;
            case '2':
                LogMsg("Calendar module selected!\n");
                break;
            case '3':
                LogMsg("Notification module is not available yet!\n");
                break;
            default:
                LogMsg("Invalid command, please try again!\n");
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