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

#include "../utils/datatype/string-type.h"

Status LoadUserDataAPI(Auth* auth);
Status LoadAllUserAppDataPathAPI(Auth* auth);
Status LoadLEB2Data(Auth* auth);
error LoadSemesterData(Auth* auth, DataPath* leb2DataPath);
error LoadClassData(Auth* auth, DataPath* semesterDataPath, Semester* semester);
Status LoadLearningActivities(Auth* auth, Class* class, DataPath* classDataPath);
Status LoadAttendance(Auth* auth, Class* class, DataPath* classDataPath);
Status LoadScoreBook(Auth* auth, Class* class, DataPath* classDataPath);
Status LoadAllClassData(Auth* auth, Class* class, DataPath* classDataPath);
error LoadAssignment(DataPath* classDataPath, Class* class);

#endif // LOAD_USER_DATA_H
