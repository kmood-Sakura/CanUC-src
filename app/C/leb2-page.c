// #include "../leb2-page.h"

// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>

// void class_page(Class* classData);

// Semester semesters[2];
// Class classObjects[2][MAX_CLASSES]; // [semester][classIndex]

// void push_class(SemesterList *data, int semIndex, const char *name) {
//     if (data->count < MAX_CLASSES) {
//         strcpy(data->classes[data->count].name, name); // myClass
//         strcpy(classObjects[semIndex][data->count].name, name); // ClassData
//         classObjects[semIndex][data->count].assignmentCount = 0;
//         data->count++;
//     }
// }

// void push_assignment(int semIndex, int classIndex, const char* title, const char* desc, const char* assignDate, const char* dueDate) {
//     Class* class = &classObjects[semIndex][classIndex];
//     if (class->assignmentCount < MAX_ASSIGNMENTS) {
//         Assignment* a = &class->assignments[class->assignmentCount];
//         strcpy(a->head, title);
//         strcpy(a->description, desc);
//         strcpy(a->assignDate, assignDate);
//         strcpy(a->dueDate, dueDate);
//         a->done = 0;
//         class->assignmentCount++;
//     }
// }


// void leb2page() {
//     semesters[0].data.count = 0;
//     semesters[0].priority = 2;
//     strcpy(semesters[0].label, "Semester 1");

//     semesters[1].data.count = 0;
//     semesters[1].priority = 1;
//     strcpy(semesters[1].label, "Semester 2");

//     push_class(&semesters[0].data, 0, "MTH101");
//     push_assignment(0, 0, "Limits and Derivatives HW", "Solve problems on limits and basic derivatives from textbook", "10/01/2025", "17/01/2025");
//     push_assignment(0, 0, "Quiz: Integration", "Review integration techniques for upcoming quiz", "15/01/2025", "18/01/2025");

//     push_class(&semesters[0].data, 0, "CPE111");
//     push_assignment(0, 1, "Lab 1: Hello World in C", "Write and compile your first C program printing 'Hello World'", "11/01/2025", "13/01/2025");

//     push_class(&semesters[0].data, 0, "CPE123");
//     push_class(&semesters[0].data, 0, "CPE100");
//     push_class(&semesters[0].data, 0, "CPE101");
//     push_class(&semesters[0].data, 0, "LNG221");

//     // --------------------------------------------------------

//     push_class(&semesters[1].data, 1, "MTH102");
//     push_assignment(1, 0, "Series and Sequences HW", "Complete all sequence-related exercises from textbook", "01/05/2025", "07/05/2025");
//     push_assignment(1, 0, "Differential Equations Quiz", "Revise topics from lecture 5 to 8", "01/05/2025", "05/05/2025");

//     push_class(&semesters[1].data, 1, "PHY103");
//     push_assignment(1, 1, "Lab: Newton’s Laws", "Lab report on Newton’s Second Law experiment", "02/05/2025", "09/05/2025");
//     push_assignment(1, 1, "Project: Gravity Simulator", "Build a gravity simulator in Python", "02/05/2025", "16/05/2025");

//     push_class(&semesters[1].data, 1, "CPE121");
//     push_assignment(1, 2, "Assembler Exercise", "Translate simple C code to assembly", "03/05/2025", "08/05/2025");
//     push_assignment(1, 2, "Machine Code Challenge", "Manually write machine instructions for given tasks", "03/05/2025", "10/05/2025");

//     push_class(&semesters[1].data, 1, "CPE112");
//     push_assignment(1, 3, "Teamwork Reflection", "Write a 1-page reflection on team collaboration", "04/05/2025", "06/05/2025");
//     push_assignment(1, 3, "Presentation Slides", "Create slides for next week's team project", "04/05/2025", "11/05/2025");

//     push_class(&semesters[1].data, 1, "GEN121");
//     push_assignment(1, 4, "Critical Thinking Report", "Analyze a social issue using logical reasoning", "05/05/2025", "12/05/2025");
//     push_assignment(1, 4, "Case Study Analysis", "Break down a real-world case study provided in class", "05/05/2025", "14/05/2025");

//     push_class(&semesters[1].data, 1, "LNG222");
//     push_assignment(1, 5, "Listening Practice", "Watch a TED Talk and take notes", "06/05/2025", "09/05/2025");
//     push_assignment(1, 5, "Speaking Assessment", "Prepare a 2-minute speech about your weekend", "06/05/2025", "10/05/2025");



//     if (semesters[0].priority > semesters[1].priority) {
//         Semester temp = semesters[0];
//         semesters[0] = semesters[1];
//         semesters[1] = temp;

//         Class tempClasses[MAX_CLASSES];
//         memcpy(tempClasses, classObjects[0], sizeof(tempClasses));
//         memcpy(classObjects[0], classObjects[1], sizeof(tempClasses));
//         memcpy(classObjects[1], tempClasses, sizeof(tempClasses));
//     }

//     int current = 0;

//     while (1) {
//         printf("\n--------------------------------------------------------\n\n");
//         printf("\033[1mLEB2\033[0m\n\n");
//         printf("  Classes: %s\n\n", semesters[current].label);
//         for (int i = 0; i < semesters[current].data.count; i++) {
//             printf("  [%d] %s\n", i + 1, semesters[current].data.classes[i].name);
//         }

//         printf("\n  [p] Previous  [n] Next  [b] Back\n\n");
//         printf("command: ");

//         char input[10];
//         scanf("%s", input);

//         if (strlen(input) == 1 && isdigit(input[0])) {
//             int idx = input[0] - '0' - 1;
//             if (idx >= 0 && idx < semesters[current].data.count) {
//                 class_page(&classObjects[current][idx]);
//             } else {
//                 printf("\n\033[0;31mInvalid Class Number\033[0m\n");
//             }
//         } else {
//             char cmd = tolower(input[0]);
//             switch (cmd) {
//                 case 'p':
//                     if (current < 1) current++;
//                     else printf("\n\033[0;31mNo previous semester.\033[0m\n");
//                     break;
//                 case 'n':
//                     if (current > 0) current--;
//                     else printf("\n\033[0;31mNo next semester.\033[0m\n");
//                     break;
//                 case 'b':
//                     return;
//                 default:
//                     printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
//                     break;
//             }
//         }
//     }
// }


#include "../leb2-page.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void class_page(Class* classData);

LEB2 leb2;

Class* create_class(const char* name) {
    Class* newClass = (Class*)malloc(sizeof(Class));
    newClass->classId = strdup(name);
    newClass->assignmentList = NULL;
    newClass->dashboard = NULL;
    newClass->syllabus = NULL;
    newClass->learningActivityList = NULL;
    newClass->attendanceList = NULL;
    newClass->scoreBookList = NULL;
    newClass->learnItList = NULL;
    newClass->fileList = NULL;
    newClass->survey = NULL;
    newClass->memberList = NULL;
    return newClass;
}

void push_class(SemesterList* sem, const char* name) {
    ClassList* newNode = (ClassList*)malloc(sizeof(ClassList));
    newNode->class = *create_class(name);
    newNode->next = NULL;
    newNode->prev = NULL;

    if (!sem->semester.classList) {
        sem->semester.classList = newNode;
    } else {
        ClassList* tail = sem->semester.classList;
        while (tail->next) tail = tail->next;
        tail->next = newNode;
        newNode->prev = tail;
    }
}

void push_assignment(Class* classObj, const char* title, const char* desc, DateTime assignDate, DateTime dueDate) {
    AssignmentList* newAssign = (AssignmentList*)malloc(sizeof(AssignmentList));
    newAssign->assignment.head = strdup(title);
    newAssign->assignment.description = strdup(desc);
    newAssign->assignment.assignDate = assignDate;
    newAssign->assignment.dueDate = dueDate;
    newAssign->assignment.done = false;
    newAssign->next = NULL;
    newAssign->prev = NULL;

    if (!classObj->assignmentList) {
        classObj->assignmentList = newAssign;
    } else {
        AssignmentList* tail = classObj->assignmentList;
        while (tail->next) tail = tail->next;
        tail->next = newAssign;
        newAssign->prev = tail;
    }
}

void leb2page() {
    SemesterList* sem1 = (SemesterList*)malloc(sizeof(SemesterList));
    SemesterList* sem2 = (SemesterList*)malloc(sizeof(SemesterList));

    sem1->semester.semesterId = strdup("Semester 1");
    sem1->semester.priority = 2;
    sem1->semester.classList = NULL;
    sem1->next = sem2;
    sem1->prev = NULL;

    sem2->semester.semesterId = strdup("Semester 2");
    sem2->semester.priority = 1;
    sem2->semester.classList = NULL;
    sem2->next = NULL;
    sem2->prev = sem1;

    leb2.semesterList = sem1;

    push_class(sem1, "MTH101");
    push_class(sem1, "CPE111");
    push_class(sem1, "CPE123");
    push_class(sem1, "CPE100");
    push_class(sem1, "CPE101");
    push_class(sem1, "LNG221");

    push_class(sem2, "MTH102");
    push_class(sem2, "PHY103");
    push_class(sem2, "CPE121");
    push_class(sem2, "CPE112");
    push_class(sem2, "GEN121");
    push_class(sem2, "LNG222");

    DateTime d1 = {10, 1, 2025};
    DateTime d2 = {17, 1, 2025};
    push_assignment(&sem1->semester.classList->class, "Limits and Derivatives HW", "Solve problems on limits and basic derivatives from textbook", d1, d2);
    
    int current = 0;
    SemesterList* semesters[] = {sem1, sem2};

    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mLEB2\033[0m\n\n");
        printf("  Classes: %s\n\n", semesters[current]->semester.semesterId);

        int index = 1;
        for (ClassList* cl = semesters[current]->semester.classList; cl != NULL; cl = cl->next, index++) {
            printf("  [%d] %s\n", index, cl->class.classId);
        }

        printf("\n  [p] Previous  [n] Next  [b] Back\n\n");
        printf("command: ");

        char input[10];
        scanf("%s", input);

        if (strlen(input) == 1 && isdigit(input[0])) {
            int idx = input[0] - '0';
            ClassList* cl = semesters[current]->semester.classList;
            while (cl && --idx > 0) cl = cl->next;
            if (cl) {
                class_page(&cl->class);
            } else {
                printf("\n\033[0;31mInvalid Class Number\033[0m\n");
            }
        } else {
            char cmd = tolower(input[0]);
            switch (cmd) {
                case 'p':
                    if (current < 1) current++;
                    else printf("\n\033[0;31mNo previous semester.\033[0m\n");
                    break;
                case 'n':
                    if (current > 0) current--;
                    else printf("\n\033[0;31mNo next semester.\033[0m\n");
                    break;
                case 'b':
                    return;
                default:
                    printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
                    break;
            }
        }
    }
}
