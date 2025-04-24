#include "../login-page.h"
#include <stdio.h>
#include <stdlib.h>

code LoginPage(Auth* auth) {
    if (auth->userData != NULL) {
        return -1; // Auth already verified
    }
    uint8 times = 0;
    while(times < 3) {
        if(!getAuth(auth)) {
            // permission (????)
            return 0; // Not get auth -> Skip
        }
        if (Login(auth)) {
            return 1; // Success
        }
        times++;
    }
    
    return 0; // Failed to login after 3 attempts
}

code Login(Auth* auth) {
    Status* status = LoadUserDataAPI(auth);
    if (!LogFetal(status)) {
        ResetAuth(auth); // Free auth data
        return 0; // Failed to load user data
    }
    return 1; // Success
}

code Logout(Auth* auth) {
    if (!verifyAuth(auth)) {
        return -1; // Auth not verified
    }
    FreeAuth(auth); // Free auth data
    return 1; // Success
}