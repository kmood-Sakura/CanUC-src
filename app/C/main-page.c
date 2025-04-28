#include "../main-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void MainPage() {
    LogMsg("Welcome to the Main Page!");
    Auth auth;
    initAuth(&auth);
    if (!LoginPage(&auth)) {
        return;
    }
    LogMsg("Login successful!");

    error err = NULL;
    Status status;

    initStatus(&status); // Initialize status structure
    
    Path canUCdir;
    initPath(&canUCdir); // Initialize CanUC directory path
    
    err = getCurrentPath(&canUCdir); // Get CanUC directory path
    if (err != NULL) {
        LogMsg("Failed to get CanUC directory path"); // Log message
        Error(err); // Log error
        return; // Failed to get CanUC directory path
    }

    printf("Path : ");
    LogMsg(canUCdir.path); // Log CanUC directory path

    err = UpdateDB(&auth, &status, canUCdir); // Update database
    if (err != NULL) {
        LogMsg("Failed to update database"); // Log message
        Error(err); // Log error
        return; // Failed to update database
    }
}