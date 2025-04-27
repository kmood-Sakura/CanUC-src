#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "../utils/struct/auth.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"

#include "../service/load-user-data.h"

code LoginPage(Auth* auth); // login page
code LoginField(Auth* auth); // login field
code Login(Auth* auth); // login function
code Logout(Auth* auth); // logout function

#endif // LOGIN_PAGE_H