#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_SUBJECTS 6
#define MAX_NAME_LEN 100

typedef struct {
    char name[MAX_NAME_LEN];
    int priority;
} Subject;

typedef struct {
    Subject subjects[MAX_SUBJECTS];
    int count;
} Semester;

Semester semester1 = {.count = 0};
Semester semester2 = {.count = 0};

void push_subject(Semester *semester, const char *name, int priority) {
    if (semester->count < MAX_SUBJECTS) {
        strcpy(semester->subjects[semester->count].name, name);
        semester->subjects[semester->count].priority = priority;
        semester->count++;
    }
}

void sort_by_priority(Semester *semester) {
    for (int i = 0; i < semester->count - 1; i++) {
        for (int j = i + 1; j < semester->count; j++) {
            if (semester->subjects[i].priority < semester->subjects[j].priority) {
                Subject temp = semester->subjects[i];
                semester->subjects[i] = semester->subjects[j];
                semester->subjects[j] = temp;
            }
        }
    }
}

void leb2page() {
    semester1.count = 0;
    semester2.count = 0;

    push_subject(&semester1, "Mathematics I", 1);
    push_subject(&semester1, "Physics I", 2);
    push_subject(&semester1, "Programming I", 3);
    push_subject(&semester1, "English I", 4);
    push_subject(&semester1, "Chemistry I", 5);
    push_subject(&semester1, "Engineering Drawing", 6);

    push_subject(&semester2, "Mathematics II", 7);
    push_subject(&semester2, "Physics II", 8);
    push_subject(&semester2, "Programming II", 9);
    push_subject(&semester2, "English II", 10);
    push_subject(&semester2, "Digital Logic", 11);
    push_subject(&semester2, "Mechanics", 12);

    printf("\n--- Semester Selection ---\n");
    printf("[1] Semester 1\n");
    printf("[2] Semester 2 (default)\n");
    printf("[b] Back to menu\n");

    char cmd;
    printf("Choose semester: ");
    scanf(" %c", &cmd);
    cmd = toupper(cmd);

    Semester *selected = NULL;

    if (cmd == '1') selected = &semester1;
    else if (cmd == '2') selected = &semester2;
    else return;

    sort_by_priority(selected);

    printf("\n--- Subjects (Newest First) ---\n");
    for (int i = 0; i < selected->count; i++) {
        printf("%d. %s\n", i + 1, selected->subjects[i].name);
    }
}
