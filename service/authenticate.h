#ifndef AUTHENTICATE_H
#define AUTHENTICATE_H

#include "../utils/struct/auth.h"
#include "../utils/struct/path.h"

#include "../utils/datatype/string-type.h"
#include "../utils/datatype/int-type.h"

#include "../utils/common/log.h"
#include "../utils/common/status.h"

Status Authenticate(Auth* auth); // Authenticate function
Status SetupAuthenticate(Auth* auth); // Setup authentication function

#endif