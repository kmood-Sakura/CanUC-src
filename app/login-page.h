#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "../utils/struct/auth.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"

#include "../service/load-user-data.h"
#include "../service/update-db.h"
#include "../service/authenticate.h"

code AuthenPage(Auth* auth);
code SignupPage(Auth* auth);
code LoginPage(Auth* auth);
code AuthInputField(Auth* auth);
code Login(Auth* auth);
code Signup(Auth* auth);
code Logout(Auth* auth);

#endif