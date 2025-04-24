#include "../leb2.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Assuming Error is a function that handles error reporting
// Assuming string is defined as typedef char* string;

// Function to create Dashboard structure
Dashboard* createDashboard(const string description) {
    Dashboard* dashboard = (Dashboard*)malloc(sizeof(Dashboard));
    if (dashboard == NULL) {
        Error("Memory allocation failed for Dashboard");
        return NULL;
    }
    
    dashboard->description = description ? createString(description) : NULL;
    return dashboard;
}

// Function to create Syllabus structure
Syllabus* createSyllabus(const string description) {
    Syllabus* syllabus = (Syllabus*)malloc(sizeof(Syllabus));
    if (syllabus == NULL) {
        Error("Memory allocation failed for Syllabus");
        return NULL;
    }
    
    syllabus->description = description ? createString(description) : NULL;
    return syllabus;
}

// Function to create Assignment structure
Assignment* createAssignment(const string head, const string description, 
                            DateTime assignDate, DateTime dueDate) {
    Assignment* assignment = (Assignment*)malloc(sizeof(Assignment));
    if (assignment == NULL) {
        Error("Memory allocation failed for Assignment");
        return NULL;
    }
    
    assignment->head = head ? createString(head) : NULL;
    assignment->description = description ? createString(description) : NULL;
    assignment->assignDate = assignDate;
    assignment->dueDate = dueDate;
    
    return assignment;
}

// Function to create AssignmentActivity structure
AssignmentActivity* createAssignmentActivity(const Assignment* assignments) {
    AssignmentActivity* activity = (AssignmentActivity*)malloc(sizeof(AssignmentActivity));
    if (activity == NULL) {
        Error("Memory allocation failed for AssignmentActivity");
        return NULL;
    }
    
    activity->assignments = (Assignment*)assignments;
    return activity;
}

// Function to create LearningActivity structure
LearningActivity* createLearningActivity(const string name, const string description, const string imageUrl) {
    LearningActivity* activity = (LearningActivity*)malloc(sizeof(LearningActivity));
    if (activity == NULL) {
        Error("Memory allocation failed for LearningActivity");
        return NULL;
    }
    
    activity->name = name ? createString(name) : NULL;
    activity->description = description ? createString(description) : NULL;
    activity->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return activity;
}

// Function to create Attendance structure
Attendance* createAttendance(const string name, const string description, const string imageUrl) {
    Attendance* attendance = (Attendance*)malloc(sizeof(Attendance));
    if (attendance == NULL) {
        Error("Memory allocation failed for Attendance");
        return NULL;
    }
    
    attendance->name = name ? createString(name) : NULL;
    attendance->description = description ? createString(description) : NULL;
    attendance->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return attendance;
}

// Function to create ScoreBook structure
ScoreBook* createScoreBook(const string name, const string description, const string imageUrl) {
    ScoreBook* scoreBook = (ScoreBook*)malloc(sizeof(ScoreBook));
    if (scoreBook == NULL) {
        Error("Memory allocation failed for ScoreBook");
        return NULL;
    }
    
    scoreBook->name = name ? createString(name) : NULL;
    scoreBook->description = description ? createString(description) : NULL;
    scoreBook->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return scoreBook;
}

// Function to create LearnIt structure
LearnIt* createLearnIt(const string name, const string description, const string imageUrl) {
    LearnIt* learnIt = (LearnIt*)malloc(sizeof(LearnIt));
    if (learnIt == NULL) {
        Error("Memory allocation failed for LearnIt");
        return NULL;
    }
    
    learnIt->name = name ? createString(name) : NULL;
    learnIt->description = description ? createString(description) : NULL;
    learnIt->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return learnIt;
}

// Function to create Survey structure
Survey* createSurvey(const string name, const string description, const string imageUrl) {
    Survey* survey = (Survey*)malloc(sizeof(Survey));
    if (survey == NULL) {
        Error("Memory allocation failed for Survey");
        return NULL;
    }
    
    survey->name = name ? createString(name) : NULL;
    survey->description = description ? createString(description) : NULL;
    survey->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return survey;
}

// Function to create Members structure
Members* createMembers(const string name, const string description, const string imageUrl) {
    Members* members = (Members*)malloc(sizeof(Members));
    if (members == NULL) {
        Error("Memory allocation failed for Members");
        return NULL;
    }
    
    members->name = name ? createString(name) : NULL;
    members->description = description ? createString(description) : NULL;
    members->imageUrl = imageUrl ? createString(imageUrl) : NULL;
    
    return members;
}

// Function to create Class structure
Class* createClass(Dashboard* dashboard, Syllabus* syllabus, AssignmentActivity* assignmentActivity,
                  LearningActivity* learningActivity, Attendance* attendance, ScoreBook* scoreBook,
                  LearnIt* learnIt, Survey* survey, File* files, Members* members) {
    Class* class = (Class*)malloc(sizeof(Class));
    if (class == NULL) {
        Error("Memory allocation failed for Class");
        return NULL;
    }
    
    class->dashboard = dashboard;
    class->syllabus = syllabus;
    class->assignmentActivity = assignmentActivity;
    class->learningActivity = learningActivity;
    class->attendance = attendance;
    class->scoreBook = scoreBook;
    class->learnIt = learnIt;
    class->survey = survey;
    class->files = files;
    class->members = members;
    
    return class;
}

// Function to create Semester structure
Semester* createSemester(const Class* classdata) {
    if (classdata == NULL) {
        return NULL;
    }
    
    Semester* sem = (Semester*)malloc(sizeof(Semester));
    if (sem == NULL) {
        Error("Memory allocation failed for Semester");
        return NULL;
    }

    sem->classes = (Class*)classdata;
    
    return sem;
}

// Function to create LEB2 structure
LEB2* createLEB2(const Semester* semesterdata) {
    if (semesterdata == NULL) {
        return NULL;
    }

    LEB2* leb2 = (LEB2*)malloc(sizeof(LEB2));
    if (leb2 == NULL) {
        Error("Memory allocation failed for LEB2");
        return NULL;
    }

    leb2->semesters = (Semester*)semesterdata;
    return leb2;
}

// Function to free Dashboard structure
void FreeDashboard(Dashboard* dashboard) {
    if (dashboard == NULL) return;
    
    if (dashboard->description) free(dashboard->description);
    free(dashboard);
}

// Function to free Syllabus structure
void FreeSyllabus(Syllabus* syllabus) {
    if (syllabus == NULL) return;
    
    if (syllabus->description) free(syllabus->description);
    free(syllabus);
}

// Function to free Assignment structure
void FreeAssignment(Assignment* assignment) {
    if (assignment == NULL) return;
    
    if (assignment->head) free(assignment->head);
    if (assignment->description) free(assignment->description);
    free(assignment);
}

// Function to Free AssignmentActivity structure
void FreeAssignmentActivity(AssignmentActivity* activity) {
    if (activity == NULL) return;
    
    if (activity->assignments) {
        Assignment* current = activity->assignments;
        while (current && current->head) {
            Assignment* next = current + 1;
            FreeAssignment(current);
            current = next;
        }
    }
    
    free(activity);
}

// Function to Free LearningActivity structure
void FreeLearningActivity(LearningActivity* activity) {
    if (activity == NULL) return;
    
    if (activity->name) free(activity->name);
    if (activity->description) free(activity->description);
    if (activity->imageUrl) free(activity->imageUrl);
    free(activity);
}

// Function to Free Attendance structure
void FreeAttendance(Attendance* attendance) {
    if (attendance == NULL) return;
    
    if (attendance->name) free(attendance->name);
    if (attendance->description) free(attendance->description);
    if (attendance->imageUrl) free(attendance->imageUrl);
    free(attendance);
}

// Function to Free ScoreBook structure
void FreeScoreBook(ScoreBook* scoreBook) {
    if (scoreBook == NULL) return;
    
    if (scoreBook->name) free(scoreBook->name);
    if (scoreBook->description) free(scoreBook->description);
    if (scoreBook->imageUrl) free(scoreBook->imageUrl);
    free(scoreBook);
}

// Function to Free LearnIt structure
void FreeLearnIt(LearnIt* learnIt) {
    if (learnIt == NULL) return;
    
    if (learnIt->name) free(learnIt->name);
    if (learnIt->description) free(learnIt->description);
    if (learnIt->imageUrl) free(learnIt->imageUrl);
    free(learnIt);
}

// Function to Free Survey structure
void FreeSurvey(Survey* survey) {
    if (survey == NULL) return;
    
    if (survey->name) free(survey->name);
    if (survey->description) free(survey->description);
    if (survey->imageUrl) free(survey->imageUrl);
    free(survey);
}

// Function to Free Members structure
void FreeMembers(Members* members) {
    if (members == NULL) return;
    
    if (members->name) free(members->name);
    if (members->description) free(members->description);
    if (members->imageUrl) free(members->imageUrl);
    free(members);
}

// Function to Free Class structure
void FreeClass(Class* class) {
    if (class == NULL) return;
    
    FreeDashboard(class->dashboard);
    FreeSyllabus(class->syllabus);
    FreeAssignmentActivity(class->assignmentActivity);
    FreeLearningActivity(class->learningActivity);
    FreeAttendance(class->attendance);
    FreeScoreBook(class->scoreBook);
    FreeLearnIt(class->learnIt);
    FreeSurvey(class->survey);
    FreeFile(class->files);
    FreeMembers(class->members);
    
    free(class);
}

// Function to Free Semester structure
void FreeSemester(Semester* semester) {
    if (semester == NULL) return;
    
    if (semester->classes) {
        Class* current = semester->classes;
        while (current) {
            Class* next = current + 1;
            FreeClass(current);
            current = next;
        }
    }
    
    free(semester);
}

// Function to Free LEB2 structure
void FreeLEB2(LEB2* leb2) {
    if (leb2 == NULL) return;
    
    if (leb2->semesters) {
        Semester* current = leb2->semesters;
        while (current) {
            Semester* next = current + 1;
            FreeSemester(current);
            current = next;
        }
    }
    free(leb2);
}
