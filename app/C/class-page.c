#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void submission_page(myAssignment* assignment);

void class_page(ClassData* classData) {
    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1m%s - Assignments\033[0m\n\n", classData->name);
        if (classData->assignmentCount == 0) {
            printf("\033[38;5;250mNo assignment\033[0m\n");
        } else {
            for (int i = 0; i < classData->assignmentCount; i++) {
                if (classData->assignments[i].done)
                    printf("  [%d] %s \033[32m[done]\033[0m\n", i + 1, classData->assignments[i].title);
                else
                    printf("  [%d] %s\n", i + 1, classData->assignments[i].title);
            }
        }
        printf("\n  [b] Back\n\n");

        char input[10];
        printf("command: ");
        scanf("%s", input);

        if (strlen(input) == 1 && isdigit(input[0])) {
            int idx = input[0] - '0' - 1;
            if (idx >= 0 && idx < classData->assignmentCount) {
                submission_page(&classData->assignments[idx]); // 🔗 open assignment
            } else {
                printf("\n\033[0;31mInvalid Assignment Number\033[0m\n");
            }
        } else if (tolower(input[0]) == 'b') {
            return;
        } else {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
        }
    }
}
