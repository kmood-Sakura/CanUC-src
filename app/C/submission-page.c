#include "../leb2-page.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* format_date(DateTime dt) {
    static char buf[20];
    time_t t = dt;
    struct tm *tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%d/%m/%Y", tm_info);
    return buf;
}

void submission_page(Assignment* assignment) {
    while (1) {
        printf("\n--------------------------------------------------------\n\n");
        printf("\033[1m%s\033[0m\n\n", assignment->head);
        printf("\033[1mDescription\033[0m\n%s\n\n", assignment->description);
        printf("\033[1mAssign Date\033[0m\n%s\n\n", format_date(assignment->assignDate));
        printf("\033[1mDue Date\033[0m\n\033[31m%s\033[0m\n\n", format_date(assignment->dueDate));

        printf("\033[1mStatus: \033[0m");
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

        if (cmd == 's') {
            assignment->done = !assignment->done;
        } else if (cmd == 'b') {
            return;
        } else {
            printf("\n\033[0;31mInvalid Command. Please Enter Again\033[0m\n");
        }
    }
}
