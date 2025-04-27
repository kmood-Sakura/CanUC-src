#include "../load-user-data.h"

#include <stdio.h>
#include <stdlib.h>

error LoadUserDataAPI(Auth* auth, Status* status) {
    FreeStatus(status); // Free existing status
    error err = allocateStatus(&status); // Allocate status
    if (err != NULL) {
        return err; // Memory allocation failed
    }

    err = createStatus(status, 0, "Failed to load user data", "Failed to load user data");
    if (err != NULL) {
        return err; // Memory allocation failed
    }
    
    if (auth == NULL) {
        err = createStatus(status, 0, "Invalid request Auth", "You need to input Auth parameter"); // Create status
        if (err != NULL) {
            return err; // Memory allocation failed
        }
        return NULL;
    }
    err = createUserData(&auth->userData, NULL, NULL, NULL); // Load user data
    if (err != NULL) {
        return err; // Failed to load user data
    }
    
    err = createStatus(status, 1, "User data loaded successfully", "User data loaded successfully"); // Create success status
    if (err != NULL) {
        return err; // Memory allocation failed
    }

    return NULL; // Success
}