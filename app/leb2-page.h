#ifndef LEB2_PAGE_H
#define LEB2_PAGE_H

#include "../utils/common/log.h"
#include "../utils/common/status.h"
#include "../utils/common/request.h"

#include "../utils/datatype/string-type.h"
#include "../utils/datatype/int-type.h"
#include "../utils/datatype/char-type.h"
#include "../utils/datatype/date-type.h"

#include "../utils/struct/path.h"
#include "../utils/struct/file.h"
#include "../utils/struct/auth.h"
#include "../utils/struct/leb2.h"

#include "../service/update-db.h"

#include "class-operation.h"

void LEB2Page(Auth* auth);

void ShowSemester(SemesterList* currentSemester);
void ShowClassSection(Semester* semester);
void ClassSelectionPage(Semester* semester, uint8 year, uint8 term);

void ClassDetailsPage(Class* class, uint8 year, uint8 term);

void DashboardPage(Dashboard* dashboard);
void SyllabusPage(Syllabus* syllabus);
void AttendanceListPage(AttendanceList* attendanceList);
void ScoreBookListPage(ScoreBookList* scoreBookList);
void LearnItListPage(LearnItList* learnItList);
void FileListPage(FileList* fileList);
void SurveyPage(Survey* survey);
void MemberListPage(MemberList* memberList);

#endif