#include "../authenticate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status Authenticate(Auth* auth) {
    if (auth == NULL) return SetStatus(0, "Failed to Authenticate", NULL);
    if (auth->studentId == NULL || auth->password == NULL) return SetStatus(0, "Failed to Authenticate", "Student ID and Password are required");
    if (auth->dataPath == NULL) return SetStatus(0, "Failed to Authenticate", "Havn't Setup DB path");
    string pswpath = (string)malloc(sizeof(char) * auth->dataPath->path.length + 6);
    if (pswpath == NULL) return SetStatus(0, "Failed to Authenticate", NULL);

    snprintf(pswpath, auth->dataPath->path.length + 6, "%se.env", auth->dataPath->path.path);
    FILE* file = fopen(pswpath, "r");
    if (file == NULL) {
        free(pswpath);
        return SetStatus(-1, "Failed to Authenticate", "This user haven't signed up yet");
    }
    
    char line[MAX_PASSWORD_LEN + 9];
    int result = 0;
    
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* prefix = "USER_PSW=";
        if (strncmp(line, prefix, strlen(prefix)) == 0) {
            char* stored_password = line + strlen(prefix);
            result = (strcmp(auth->password, stored_password) == 0) ? 1 : 0;
            memset(stored_password, 0, strlen(stored_password));
        }
    }
    
    FreeString(&auth->password);
    auth->password = NULL;

    memset(line, 0, sizeof(line));
    fclose(file);
    free(pswpath);

    if (result == 0) return SetStatus(0, "Failed to Authenticate", "Invalid password");
    return SetStatus(1, "Authenticated successfully", NULL);
}

Status SetupAuthenticate(Auth* auth) {
    if (auth == NULL) return SetStatus(0, "Failed to Sign-up", "Invalid auth pointer");
    uint32 studentIdLength = stringLen(auth->studentId); uint32 passwordLength = stringLen(auth->password);
    if (studentIdLength == 0 || passwordLength == 0) return SetStatus(0, "Failed to Sign-up", "Student ID and Password are required");
    if (auth->dataPath->path.path == NULL) return SetStatus(0, "Failed to Sign-up", "Havn't Setup DB path");

    if (studentIdLength > MAX_STUDENT_ID) return SetStatus(0, "Failed to Sign-up", "Student ID is too long");
    if (passwordLength > MAX_PASSWORD_LEN) return SetStatus(0, "Failed to Sign-up", "Password is too long");

    string pswpath = (string)malloc(sizeof(char) * auth->dataPath->path.length + 6);
    if (pswpath == NULL) return SetStatus(0, "Failed to Sign-up", NULL);

    snprintf(pswpath, auth->dataPath->path.length + 6, "%se.env", auth->dataPath->path.path);
    FILE* file = NULL;
    file = fopen(pswpath, "r");
    if (file != NULL) {
        fclose(file); 
        free(pswpath); 
        return SetStatus(0, "Failed to Sign-up", "This Account have already exist");
    }

    file = fopen(pswpath, "w");
    if (file == NULL) {
        free(pswpath);
        return SetStatus(0, "Failed to Sign-up", "Failed to create password file");
    }
    fprintf(file, "USER_PSW=%s\n", auth->password);
    fclose(file);

    free(pswpath);

    return SetStatus(1, "Sign-up successful", NULL);
}
