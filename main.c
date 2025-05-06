#include <stdio.h>

#include "utils/struct/auth.h"
#include "utils/common/log.h"
#include "app/home-page.h"

int main() {
    printf("\n");
    Auth auth;
    initAuth(&auth);

    HomePage(&auth);

    Logout(&auth);
    return 0;
}