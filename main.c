#include <stdio.h>

#include "utils/struct/auth.h"
#include "utils/common/log.h"
#include "app/home.h"

int main() {
    printf("\n");
    Auth auth;
    initAuth(&auth); // Initialize auth structure

    HomePage(&auth); // Start the main page

    FreeAuthContent(&auth); // Free auth structure
    LogMsg("Exiting the app..."); // Log exit message
    return 0;
}