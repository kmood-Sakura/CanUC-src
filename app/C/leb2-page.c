#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void class_page(ClassData* classData);

mySemester semesters[2];
ClassData classObjects[2][MAX_CLASSES]; // [semester][classIndex]

void push_class(SemesterData *data, int semIndex, const char *name) {
    if (data->count < MAX_CLASSES) {
        strcpy(data->classes[data->count].name, name); // myClass
        strcpy(classObjects[semIndex][data->count].name, name); // ClassData
        classObjects[semIndex][data->count].assignmentCount = 0;
        data->count++;
    }
}

void push_assignment(int semIndex, int classIndex, const char* title) {
    ClassData* classData = &classObjects[semIndex][classIndex];
    if (classData->assignmentCount < MAX_ASSIGNMENTS) {
        strcpy(classData->assignments[classData->assignmentCount].title, title);
        classData->assignments[classData->assignmentCount].done = 0;
        classData->assignmentCount++;
    }
}

void leb2page() {
    semesters[0].data.count = 0;
    semesters[0].priority = 2;
    strcpy(semesters[0].label, "Semester 1");

    semesters[1].data.count = 0;
    semesters[1].priority = 1;
    strcpy(semesters[1].label, "Semester 2");

    push_class(&semesters[0].data, 0, "MTH101");
    push_assignment(0, 0, "Limits and Derivatives HW");
    push_assignment(0, 0, "Quiz: Integration");

    push_class(&semesters[0].data, 0, "CPE111");
    push_assignment(0, 1, "Lab 1: Hello World in C");

    push_class(&semesters[0].data, 0, "CPE123");

    push_class(&semesters[0].data, 0, "CPE100");

    push_class(&semesters[0].data, 0, "CPE101");

    push_class(&semesters[0].data, 0, "LNG221");

    // --------------------------------------------------------

    push_class(&semesters[1].data, 1, "MTH102");
    push_assignment(1, 0, "Series and Sequences HW");
    push_assignment(1, 0, "Differential Equations Quiz");

    push_class(&semesters[1].data, 1, "PHY103");
    push_assignment(1, 1, "Lab: Newton’s Laws");
    push_assignment(1, 1, "Project: Gravity Simulator");

    push_class(&semesters[1].data, 1, "CPE121");
    push_assignment(1, 2, "Assembler Exercise");
    push_assignment(1, 2, "Machine Code Challenge");

    push_class(&semesters[1].data, 1, "CPE112");
    push_assignment(1, 3, "Teamwork Reflection");
    push_assignment(1, 3, "Presentation Slides");

    push_class(&semesters[1].data, 1, "GEN121");
    push_assignment(1, 4, "Critical Thinking Report");
    push_assignment(1, 4, "Case Study Analysis");

    push_class(&semesters[1].data, 1, "LNG222");
    push_assignment(1, 5, "Listening Practice");
    push_assignment(1, 5, "Speaking Assessment");

    if (semesters[0].priority > semesters[1].priority) {
        mySemester temp = semesters[0];
        semesters[0] = semesters[1];
        semesters[1] = temp;

        ClassData tempClasses[MAX_CLASSES];
        memcpy(tempClasses, classObjects[0], sizeof(tempClasses));
        memcpy(classObjects[0], classObjects[1], sizeof(tempClasses));
        memcpy(classObjects[1], tempClasses, sizeof(tempClasses));
    }

    int current = 0;

    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1mLEB2\033[0m\n\n");
        printf("\033[1mClasses: %s\033[0m\n\n", semesters[current].label);
        for (int i = 0; i < semesters[current].data.count; i++) {
            printf("  [%d] %s\n", i + 1, semesters[current].data.classes[i].name);
        }

        printf("\n  [p] Previous  [n] Next  [b] Back\n\n");
        printf("command: ");

        char input[10];
        scanf("%s", input);

        if (strlen(input) == 1 && isdigit(input[0])) {
            int idx = input[0] - '0' - 1;
            if (idx >= 0 && idx < semesters[current].data.count) {
                class_page(&classObjects[current][idx]);
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
