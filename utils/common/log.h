#ifndef LOG_H
#define LOG_H

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"

#include "../common/status.h"

void Log(const Status* status);
void Error(const string msg);
void Warning(const string msg);
void Success(const string msg);
void LogMsg(const string msg);
void Details(const string detail);
void LogStatus(const Status* status);
void LogStatusMsg(const Status* status);
code LogFetal(const Status* status);

#endif// LOG_STATUS_H