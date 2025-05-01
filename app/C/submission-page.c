#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void submission_page(myAssignment* assignment) {
    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1m%s\033[0m\n", assignment->title);
        printf("Status: ");
        if (assignment->done) {
            printf("\033[32m[done]\033[0m\n");
        } else {
            printf("[not done]\n");
        }

        printf("\n  [s] Submit  [b] Back\n\n");

        char cmd;
        printf("command: ");
        scanf(" %c", &cmd);
        cmd = tolower(cmd);

        if (cmd == 't') {
            assignment->done = !assignment->done;
        } else if (cmd == 'b') {
            return;
        } else {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
        }
    }
}
