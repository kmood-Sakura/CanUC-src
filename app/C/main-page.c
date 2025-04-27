#include "../main-page.h"

#include <stdio.h>

void MainPage() {
    LogMsg("Welcome to the Main Page!");
    Auth auth;
    initAuth(&auth); // Initialize auth structure

    if (!LoginPage(&auth)) {
        return;
    }
    // call notification fetch
}