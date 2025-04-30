#include "../load-user-data.h"

#include <stdio.h>
#include <stdlib.h>

Status LoadUserDataAPI(Auth* auth) {
    Status status;
    initStatus(&status); // Initialize status
    if (auth == NULL) {
        status = SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer"); // Create failure status
        return status;
    }
    if (auth->userData != NULL) {
        // reload user data
        FreeUserData(auth->userData); // Free existing user data
        auth->userData = NULL; // Set userData pointer to NULL
    }
    // Load user data
    return status;
}

Status LoadLEB2Data(Auth* auth) {
    Status status;
    initStatus(&status);
    if (auth == NULL) {
        status = SetStatus(0, "Auth pointer is NULL", "Invalid auth pointer"); // Create failure status
        return status;
    }
    if (auth->userData == NULL) {
        status = SetStatus(0, "Failed to load LEB2 data", "Failed to load LEB2 data"); // Create failure status
        return status;
    }

    // Load LEB2 data
    
    return status; // Success
}
//update 