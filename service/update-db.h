#ifndef UPDATE_DB_H
#define UPDATE_DB_H

#include "../utils/struct/auth.h"
#include "../utils/struct/path.h"
#include "../utils/struct/leb2.h"
#include "../utils/struct/calendar.h"
#include "../utils/struct/notification.h"
#include "../utils/struct/file.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"

#include "../utils/datatype/string-type.h"

Status SetUpDataBase(Auth* auth);

#endif // UPDATE_DB_H