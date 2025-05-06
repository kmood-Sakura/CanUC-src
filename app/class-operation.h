#ifndef CLASS_OPERATIONS_H
#define CLASS_OPERATIONS_H

#include "../utils/datatype/int-type.h"
#include "../utils/datatype/string-type.h"
#include "../utils/datatype/date-type.h"
#include "../utils/datatype/float-type.h"

#include "../utils/common/status.h"
#include "../utils/common/log.h"
#include "../utils/common/request.h"

#include "../utils/struct/auth.h"
#include "../utils/struct/leb2.h"
#include "../utils/struct/file.h"
#include "../utils/struct/path.h"

#include "../service/leb2-service.h"

Status ShowAssignmentDetails(Auth* auth, Class* classnode);
Status AddAssignment(Auth* auth, Class* classnode, uint8 year, uint8 term);
Status RemoveAssignment(Auth* auth, Class* classnode, uint8 year, uint8 term);

Status ShowLearningActivityDetails(Auth* auth, Class* classnode);
Status AddLearningActivity(Auth* auth, Class* classnode);
Status RemoveLearningActivity(Auth* auth, Class* classnode);

void ClassAssignmentPage(Auth* auth, Class* classnode, uint8 year, uint8 term);
void ClassLearningActivityPage(Auth* auth, Class* classnode);

#endif