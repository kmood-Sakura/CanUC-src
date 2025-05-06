#ifndef LEB2_SERVICE_H
#define LEB2_SERVICE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"
#include "../utils/common/request.h"

#include "../utils/struct/leb2.h"
#include "../utils/struct/auth.h"
#include "../utils/struct/path.h"
#include "../utils/struct/file.h"

#include "../utils/datatype/date-type.h"
#include "../utils/datatype/string-type.h"
#include "../utils/datatype/int-type.h"

Status AddAssignmentService(Auth* auth, Class* classnode, Assignment* assignment);
Status RemoveAssignmentService(Auth* auth, Class* classnode, const string title);
Status UpdateAssignmentData(Auth* auth, Class* classnode);

Status AddLearningActivityService(Auth* auth, Class* classnode, LearningActivity* activity);
Status RemoveLearningActivityService(Auth* auth, Class* classnode, const string title);
Status UpdateLearningActivityData(Auth* auth, Class* classnode);

#endif