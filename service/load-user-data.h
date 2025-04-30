#ifndef LOAD_USER_DATA_H
#define LOAD_USER_DATA_H

#include "../utils/struct/auth.h"
#include "../utils/struct/path.h"
#include "../utils/struct/leb2.h"
#include "../utils/struct/calendar.h"
#include "../utils/struct/notification.h"
#include "../utils/struct/file.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"
#include "../utils/common/request.h"

Status LoadUserDataAPI(Auth* auth);
//update
#endif // LOAD_USER_DATA_H
