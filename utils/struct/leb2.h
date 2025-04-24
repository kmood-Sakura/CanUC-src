#ifndef LEB2_H
#define LEB2_H

#define MAX_SEMESTER_CODE 4 //  `{years}-{term}`
#define SEMESTER_FORMAT "%d-%d" //  `{years}-{term}`
#define MAX_CLASS_CODE 10 // `{subjectID}{id}` Ex MTH101
#define CLASS_FORMAT "%s%d" // `{subjectID}{id}` Ex MTH101

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"
#include "../datatype/date-type.h"

#include "../common/status.h"
#include "../common/log.h"
#include "../common/request.h"

#include "file.h"

typedef string SemesterCode;
typedef string ClassCode;

typedef struct Assignment {
    string head;
    string description;
    DateTime assignDate;
    DateTime dueDate;
} Assignment;

typedef struct AssignmentActivity {
    Assignment* assignments; // list of assignments
} AssignmentActivity;

typedef struct Dashboard {
    string description;
} Dashboard;

typedef struct Syllabus {
    string description;
} Syllabus;

typedef struct LearningActivity {
    string name;
    string description;
    string imageUrl;
} LearningActivity;

typedef struct Attendance {
    string name;
    string description;
    string imageUrl;
} Attendance;

typedef struct ScoreBook {
    string name;
    string description;
    string imageUrl;
} ScoreBook;

typedef struct LearnIt {
    string name;
    string description;
    string imageUrl;
} LearnIt;

typedef struct Survey {
    string name;
    string description;
    string imageUrl;
} Survey;

typedef struct Members {
    string name;
    string description;
    string imageUrl;
} Members;

typedef struct Class {
    Dashboard* dashboard;
    Syllabus* syllabus;
    AssignmentActivity* assignmentActivity;
    LearningActivity* learningActivity;
    Attendance* attendance;
    ScoreBook* scoreBook;
    LearnIt* learnIt;
    Survey* survey;
    File* files;
    Members* members;
} Class;

typedef struct Semester {
    Class* classes;
} Semester;

typedef struct LEB2 {
    Semester* semesters; // semester code
} LEB2;

Dashboard* createDashboard(const string description);
Syllabus* createSyllabus(const string description);
Assignment* createAssignment(const string head, const string description, DateTime assignDate, DateTime dueDate);
AssignmentActivity* createAssignmentActivity(const Assignment* assignments);
LearningActivity* createLearningActivity(const string name, const string description, const string imageUrl);
Attendance* createAttendance(const string name, const string description, const string imageUrl);
ScoreBook* createScoreBook(const string name, const string description, const string imageUrl);
LearnIt* createLearnIt(const string name, const string description, const string imageUrl);
Survey* createSurvey(const string name, const string description, const string imageUrl);
Members* createMembers(const string name, const string description, const string imageUrl);
Class* createClass(Dashboard* dashboard, Syllabus* syllabus, AssignmentActivity* assignmentActivity,
                   LearningActivity* learningActivity, Attendance* attendance, ScoreBook* scoreBook,
                   LearnIt* learnIt, Survey* survey, File* files, Members* members);
Semester* createSemester(const Class* classdata);
LEB2* createLEB2(const Semester* semesters);

// Function declarations for freeing allocated structures
void FreeDashboard(Dashboard* dashboard);
void FreeSyllabus(Syllabus* syllabus);
void FreeAssignment(Assignment* assignment);
void FreeAssignmentActivity(AssignmentActivity* activity);
void FreeLearningActivity(LearningActivity* activity);
void FreeAttendance(Attendance* attendance);
void FreeScoreBook(ScoreBook* scoreBook);
void FreeLearnIt(LearnIt* learnIt);
void FreeSurvey(Survey* survey);
void FreeMembers(Members* members);
void FreeClass(Class* class);
void FreeSemester(Semester* semester);
void FreeLEB2(LEB2* leb2);

#endif // LEB2_H