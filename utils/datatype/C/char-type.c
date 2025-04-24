#include "../char-type.h"

#include <stdio.h>
#include <stdlib.h>

void toLowerCase(char *c) {
    if (c == NULL) {
        Warning("Character pointer is NULL");
        return;
    }
    if (*c >= 'A' && *c <= 'Z') {
        *c += 32; // Convert to lowercase
    }
}

char getLowerCaseChar() {
    char c = getchar();
    if (c == EOF) {
        Warning("End of file reached");
        return '\0'; // Return null character on EOF
    }
    if (c >= 'A' && c <= 'Z') {
        return c + 32; // Convert to lowercase
    }
    return c; // Return as is if not uppercase
}