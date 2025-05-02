#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "../utils/struct/auth.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"

#include "../service/load-user-data.h"
#include "../service/update-db.h"
#include "../service/authenticate.h"

code AuthenPage(Auth* auth); // authentication page
code SignupPage(Auth* auth); // sign up page
code LoginPage(Auth* auth); // login page
code AuthInputField(Auth* auth); // login field
code Login(Auth* auth); // login function
code Signup(Auth* auth); // sign up function
code Logout(Auth* auth); // logout function
//update
#endif // LOGIN_PAGE_H