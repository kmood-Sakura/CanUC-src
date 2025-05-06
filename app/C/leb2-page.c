#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Auth* auth = NULL;

void LEB2Page(Auth* userAuth) {
    if (userAuth == NULL || userAuth->userData == NULL || userAuth->userData->leb2 == NULL) {
        LogMsg("Error: Invalid auth data or LEB2 data not loaded");
        return;
    }

    auth = userAuth;

    LEB2* leb2 = userAuth->userData->leb2;
    if (leb2->semesterList == NULL) {
        printf("\n\033[0;31mNo semesters available\033[0m\n");
        return;
    }

    SemesterList* current = leb2->semesterList;
    SemesterList* lastSemester = current;
    
    while (current != NULL) {
        if (current->semester.year > lastSemester->semester.year || 
            (current->semester.year == lastSemester->semester.year && 
             current->semester.term > lastSemester->semester.term)) {
            lastSemester = current;
        }
        current = current->next;
    }
    
    current = lastSemester;
    ShowSemester(current);
}

void ShowSemester(SemesterList* currentSemester) {
    if (currentSemester == NULL) {
        printf("\n\033[0;31mInvalid semester\033[0m\n");
        return;
    }

    char cmd;
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mLEB2\033[0m\n\n");
        printf("\033[1mSemester %d/%d\033[0m\n", 
               currentSemester->semester.year, 
               currentSemester->semester.term);
        
        ShowClassSection(&(currentSemester->semester));
        
        printf("\n  [n] Next  [p] Previous  [s] Select  [b] Back\n\n");
        
        if(!requestCommand(&cmd)){
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }

        switch (cmd) {
            case 'n':
                if (currentSemester->next != NULL) {
                    return ShowSemester(currentSemester->next);
                } else {
                    printf("\n\033[0;31mNo next semester available\033[0m\n");
                }
                break;
            case 'p':
                if (currentSemester->prev != NULL) {
                    return ShowSemester(currentSemester->prev);
                } else {
                    printf("\n\033[0;31mNo previous semester available\033[0m\n");
                }
                break;
            case 's':
                ClassSelectionPage(&(currentSemester->semester));
                break;
            case 'b':
                return;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }
}

void ShowClassSection(Semester* semester) {
    if (semester == NULL || semester->classList == NULL) {
        printf("  No classes available for this semester\n");
        return;
    }

    printf("Available Classes: \n\n");
    
    ClassList* current = semester->classList;
    int index = 1;
    
    while (current != NULL) {
        printf("  [%d] %s\n", index++, current->class.classId);
        current = current->next;
    }
}

void ClassSelectionPage(Semester* semester) {
    if (semester == NULL || semester->classList == NULL) {
        printf("No classes available to select\n");
        return;
    }

    int classCount = 0;
    ClassList* current = semester->classList;
    while (current != NULL) {
        classCount++;
        current = current->next;
    }

    char input[10];
    int selection;
    printf("\nEnter class number (1-%d): ", classCount);
    if (scanf("%s", input) != 1) {
        printf("\n\033[0;31mInvalid input\033[0m\n");
        while (getchar() != '\n');
        return;
    }
    
    selection = atoi(input);
    if (selection < 1 || selection > classCount) {
        printf("\n\033[0;31mInvalid class number\033[0m\n");
        return;
    }

    // Find selected class
    current = semester->classList;
    for (int i = 1; i < selection; i++) {
        current = current->next;
    }

    // View class details - pass auth to make it available in ClassDetailsPage
    ClassDetailsPage(&(current->class));
}

void ClassDetailsPage(Class* class) {
    if (class == NULL) {
        printf("Error: Invalid class data\n");
        return;
    }

    if (auth == NULL) {
        printf("Error: Authentication data not available\n");
        return;
    }

    char cmd;
    
    while(1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mClass: %s\033[0m\n\n", class->classId);
        
        printf("  [1] Dashboard\n");
        printf("  [2] Syllabus\n");
        printf("  [3] Assignments\n");
        printf("  [4] Learning Activities\n");
        printf("  [5] Attendance\n");
        printf("  [6] ScoreBook\n");
        printf("  [7] LearnIt\n");
        printf("  [8] Files\n");
        printf("  [9] Survey\n");
        printf("  [0] Class Members\n");
        printf("\n  [b] Back\n\n");
        
        if(!requestCommand(&cmd)) {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
            continue;
        }
        
        // Check if command is 'b' for back
        if (cmd == 'b') {
            return;
        }
        
        switch (cmd) {
            case '1':
                DashboardPage(class->dashboard);
                break;
            case '2':
                SyllabusPage(class->syllabus);
                break;
            case '3':
                ClassAssignmentPage(auth, class);  // Using the stored auth pointer
                break;
            case '4':
                ClassLearningActivityPage(auth, class);  // Using the stored auth pointer
                break;
            case '5':
                AttendanceListPage(class->attendanceList);
                break;
            case '6':
                ScoreBookListPage(class->scoreBookList);
                break;
            case '7':
                LearnItListPage(class->learnItList);
                break;
            case '8':
                FileListPage(class->fileList);
                break;
            case '9':
                SurveyPage(class->survey);
                break;
            case '0':
                MemberListPage(class->memberList);
                break;
            default:
                printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                continue;
        }
    }
}

// The following functions remain unchanged
void DashboardPage(Dashboard* dashboard) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mDashboard\033[0m\n\n");
    
    if (dashboard == NULL) {
        printf("  No dashboard available\n");
    } else {
        printf("  %s\n", dashboard->description);
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void SyllabusPage(Syllabus* syllabus) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mSyllabus\033[0m\n\n");
    
    if (syllabus == NULL) {
        printf("  No syllabus available\n");
    } else {
        printf("  %s\n", syllabus->description);
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

// Update this function to call the class-operation functions instead
// Function is no longer needed as we're using ClassAssignmentPage from class-operation.c
/*
void AssignmentListPage(AssignmentList* assignmentList) {
    // This function is now replaced by ClassAssignmentPage from class-operation.c
}
*/

// Update this function to call the class-operation functions instead
// Function is no longer needed as we're using ClassLearningActivityPage from class-operation.c
/*
void LearningActivityListPage(LearningActivityList* learningActivityList) {
    // This function is now replaced by ClassLearningActivityPage from class-operation.c
}
*/

void AttendanceListPage(AttendanceList* attendanceList) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mAttendance\033[0m\n\n");
    
    if (attendanceList == NULL) {
        printf("  No attendance records available\n");
    } else {
        AttendanceList* current = attendanceList;
        int index = 1;
        
        while (current != NULL) {
            string strDate = NULL;
            error err = dateTimeToString(&strDate, current->attendance.date);
            if (err != NULL) {
                Error(err);
                return;
            }
            
            printf("  [%d] Date: %s\n", index++, strDate);
            printf("     Status: %s\n", current->attendance.joined == 1 ? "Present" : "Absent");
            
            FreeString(&strDate);
            current = current->next;
        }
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void ScoreBookListPage(ScoreBookList* scoreBookList) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mScoreBook\033[0m\n\n");
    
    if (scoreBookList == NULL) {
        printf("  No scores available\n");
    } else {
        ScoreBookList* current = scoreBookList;
        int index = 1;
        
        while (current != NULL) {
            printf("  [%d] %s\n", index++, current->scoreBook.head);
            printf("     Description: %s\n", current->scoreBook.description);
            printf("     Score: %.2f\n", current->scoreBook.score);
            current = current->next;
        }
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void LearnItListPage(LearnItList* learnItList) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mLearnIt Resources\033[0m\n\n");
    
    if (learnItList == NULL) {
        printf("  No LearnIt resources available\n");
    } else {
        LearnItList* current = learnItList;
        int index = 1;
        
        while (current != NULL) {
            printf("  [%d] %s\n", index++, current->learnIt.name);
            printf("     Description: %s\n", current->learnIt.description);
            printf("     URL: %s\n", current->learnIt.url);
            current = current->next;
        }
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void FileListPage(FileList* fileList) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mFiles\033[0m\n\n");
    
    if (fileList == NULL) {
        printf("  No files available\n");
    } else {
        FileList* current = fileList;
        int index = 1;
        
        while (current != NULL) {
            printf("  [%d] %s\n", index++, current->file.filename.path);
            current = current->next;
        }
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void SurveyPage(Survey* survey) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mSurvey\033[0m\n\n");
    
    if (survey == NULL) {
        printf("  No survey available\n");
    } else {
        printf("  Survey information not implemented yet\n");
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}

void MemberListPage(MemberList* memberList) {
    printf("\n--------------------------------------------------------\n\n");
    printf("\033[1mClass Members\033[0m\n\n");
    
    if (memberList == NULL) {
        printf("  No members available\n");
    } else {
        MemberList* current = memberList;
        int index = 1;
        
        while (current != NULL) {
            printf("  [%d] %s\n", index++, current->member.name);
            printf("     Student ID: %s\n", current->member.studentId);
            current = current->next;
        }
    }
    
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
}