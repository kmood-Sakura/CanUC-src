#ifndef REQUEST_H
#define REQUEST_H

#include "log.h" 
#include "../datatype/char-type.h"
#include "../datatype/int-type.h"
#include "../datatype/string-type.h"

code requestConfirm(); // request confirmation from user
char requestCommand(const string commandKeys); // request command from user
string requestString(const uint32 maxLength, const string prompt); // request string from user

#endif // REQUEST_H