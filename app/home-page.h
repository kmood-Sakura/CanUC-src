#ifndef HOME_PAGE_H
#define HOME_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"

#include "../utils/datatype/int-type.h"
#include "../utils/datatype/string-type.h"

#include "login-page.h"
#include "leb2-page.h"
#include "calendar-page.h"

void HomePage(Auth* auth); // Main page function
void FetchSystem(Auth* auth); // Fetch system function
//update
#endif // MAIN_PAGE_H