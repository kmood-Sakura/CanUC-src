#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void submission_page(Assignment* assignment);

void class_page(Class* classData) {
    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1m%s - Assignments\033[0m\n\n", classData->classId);

        AssignmentList* current = classData->assignmentList;
        int index = 1;

        if (!current) {
            printf("\033[38;5;250mNo assignment\033[0m\n");
        } else {
            while (current) {
                if (current->assignment.done)
                    printf("  [%d] %s \033[32m[done]\033[0m\n", index, current->assignment.head);
                else
                    printf("  [%d] %s\n", index, current->assignment.head);

                current = current->next;
                index++;
            }
        }

        printf("\n  [b] Back\n\n");

        char input[10];
        printf("command: ");
        scanf("%s", input);

        if (strlen(input) == 1 && isdigit(input[0])) {
            int selected = input[0] - '0';
            current = classData->assignmentList;
            index = 1;

            while (current && index < selected) {
                current = current->next;
                index++;
            }

            if (current && index == selected) {
                submission_page(&current->assignment); // 🔗 open selected assignment
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
