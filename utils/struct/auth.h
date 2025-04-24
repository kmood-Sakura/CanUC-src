#ifndef AUTH_H
#define AUTH_H

#include "../datatype/string-type.h"
#include "../datatype/int-type.h"
#include "../datatype/char-type.h"
#include "../datatype/date-type.h"

#include "../common/status.h"
#include "../common/log.h"
#include "../common/request.h"

#include "leb2.h"
#include "path.h"
#include "calendar.h"
#include "notification.h"

#include "file.h"

#define MAX_STUDENT_ID 11+1 // max student ID length
#define MAX_PASSWORD_LEN 30 // max password length

typedef struct UserData {
    Notification* notifications; // list of notifications
    Calendar* calendars; // list of calendars
    LEB2* leb2; // list of courses
} UserData;

typedef struct Auth {
    string studentId; // student ID
    string password; // password
    UserData* userData; // user data
} Auth;

void initAuth(Auth* auth); // initialize auth
void ResetAuth(Auth* auth); // reset auth

code getAuth(Auth* auth); // get auth from user
code verifyAuth(const Auth* auth); // verify auth

void FreeAuth(Auth* auth); // free auth
void FreeUserData(UserData* userData); // free user data

#endif // AUTH_H