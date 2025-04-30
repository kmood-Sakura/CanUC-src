#include "../home-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../service/update-db.h"
#include "../../utils/struct/path.h"

void HomePage(Auth* auth) {
    LogMsg("Welcome to the Main Page!");

    if (!LoginPage(auth)) {
        return;
    }

    LogMsg("Login successful!");

    Status status = SetUpDataBase(auth);
    if (!LogFatal(&status)) {
        return;
    }
    LogMsg("Exiting the Home Page...");
}