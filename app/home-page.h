#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"

#include "../utils/datatype/int-type.h"
#include "../utils/datatype/string-type.h"
#include "../utils/datatype/char-type.h"

#include "../utils/struct/path.h"
#include "../utils/struct/file.h"
#include "../utils/struct/auth.h"

#include "../service/load-user-data.h"
#include "../service/update-db.h"

#include "login-page.h"
#include "leb2-page.h"
#include "calendar-page.h"

void HomePage(Auth* auth);
void FetchBaseSystem(Auth* auth);

#endif