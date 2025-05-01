#ifndef LEB2_PAGE_H
#define LEB2_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"

#include "../utils/datatype/int-type.h"
#include "../utils/datatype/string-type.h"

#include "../utils/struct/path.h"
#include "../utils/struct/file.h"
#include "../utils/struct/auth.h"

#define MAX_CLASSES 100
#define MAX_SEMESTERS 10
#define MAX_ASSIGNMENTS 10

typedef struct {
    char name[100];
} myClass;

typedef struct {
    myClass classes[MAX_CLASSES];
    int count;
} SemesterData;

typedef struct {
    SemesterData data;
    int priority;
    char label[100];
} mySemester;

typedef struct {
    char title[100];
    int done;
} myAssignment;

typedef struct {
    char name[20];
    myAssignment assignments[MAX_ASSIGNMENTS];
    int assignmentCount;
} ClassData;

void leb2page();

#endif