#include "../main-page.h"

#include <stdio.h>

void MainPage() {
    Auth auth;
    initAuth(&auth); // Initialize auth structure

    if (!LoginPage(&auth)) {
        return;
    }
    // call notification fetch


}