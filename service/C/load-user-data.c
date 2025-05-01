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
    auth->userData = (UserData*)malloc(sizeof(UserData)); // Allocate memory for user data
    if (auth->userData == NULL) {
        status = SetStatus(0, "Failed to allocate memory for user data", "Memory allocation failed"); // Create failure status
        return status;
    }

    // Load user data process

    status = SetStatus(1, "User data loaded successfully", "User data loaded successfully"); // Create success status

    return status;
}

Status LoadLEB2Data(Auth* auth) {
    Status status;
    initStatus(&status);
    if (auth->userData->leb2 != NULL) {
        // Reload LEB2 data
        FreeLEB2(auth->userData->leb2); // Free existing LEB2 data
        auth->userData->leb2 = NULL; // Set leb2 pointer to NULL
    }

    error err = allocateLEB2(&auth->userData->leb2); // Allocate memory for LEB2 data
    if (err != NULL) return SetStatus(0, "Failed to allocate memory for LEB2 data", err);

    
    return status; // Success
}

Status LoadSemesterData(Auth* auth) {
    Status status;
    initStatus(&status);
    error err = NULL;
    DataPath* leb2Path = auth->dataPath->Dir[0]; // Get LEB2 path
    Semester Sem; Sem.classList = NULL; // Initialize Semester class list
    for(uint16 i = 0; i < leb2Path->sizeDir; i++) { // loop every dir in leb2 path
        if (leb2Path->Dir[i]->isFolder == 0) continue; // skip if not folder

        // load classlist data
        
        // create Semester
        err = createSemester(&Sem, leb2Path->Dir[i]->filename.path, NULL); // !!! change NULL to classlist data
        if (err != NULL) {
            Warning("Failed to create Semester");
            continue; // Skip to next folder
        }
    }
    
    return status; // Success
}