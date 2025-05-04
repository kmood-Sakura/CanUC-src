#include "../authenticate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Status Authenticate(Auth* auth) {
    if (auth == NULL) return SetStatus(0, "Failed to Authenticate", NULL); // Create failure status
    if (auth->studentId == NULL || auth->password == NULL) return SetStatus(0, "Failed to Authenticate", "Student ID and Password are required"); // Create failure status
    if (auth->dataPath == NULL) return SetStatus(0, "Failed to Authenticate", "Havn't Setup DB path"); // Create failure status

    string pswpath = (string)malloc(sizeof(char) * auth->dataPath->path.length + 6); // Allocate memory for password path
    if (pswpath == NULL) return SetStatus(0, "Failed to Authenticate", NULL); // Memory allocation failed

    snprintf(pswpath, auth->dataPath->path.length + 6, "%se.env", auth->dataPath->path.path); // Create password path
    FILE* file = fopen(pswpath, "r"); // Open password file for writing
    if (file == NULL) {
        free(pswpath); // Free allocated memory
        return SetStatus(-1, "Failed to Authenticate", "This user haven't signed up yet"); // File creation failed
    }
    
    char line[MAX_PASSWORD_LEN + 9];
    int result = 0;
    
    if (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* prefix = "USER_PSW=";
        if (strncmp(line, prefix, strlen(prefix)) == 0) {
            char* stored_password = line + strlen(prefix);
            
            result = (strcmp(auth->password, stored_password) == 0) ? 1 : 0; // Compare passwords
            
            memset(stored_password, 0, strlen(stored_password));
        }
    }
    
    free(auth->password);

    memset(line, 0, sizeof(line));
    fclose(file);
    free(pswpath);

    if (result == 0) return SetStatus(0, "Failed to Authenticate", "Invalid password"); // Password mismatch
    return SetStatus(1, "Authenticated successfully", NULL); // Authentication successful
}

Status SetupAuthenticate(Auth* auth) {
    if (auth == NULL) return SetStatus(0, "Failed to Sign-up", "Invalid auth pointer"); // Create failure status
    uint32 studentIdLength = stringLen(auth->studentId); uint32 passwordLength = stringLen(auth->password);
    if (studentIdLength == 0 || passwordLength == 0) return SetStatus(0, "Failed to Sign-up", "Student ID and Password are required"); // Create failure status
    if (auth->dataPath->path.path == NULL) return SetStatus(0, "Failed to Sign-up", "Havn't Setup DB path"); // Create failure status

    if (studentIdLength > MAX_STUDENT_ID) return SetStatus(0, "Failed to Sign-up", "Student ID is too long"); // Create failure status
    if (passwordLength > MAX_PASSWORD_LEN) return SetStatus(0, "Failed to Sign-up", "Password is too long"); // Create failure status

    string pswpath = (string)malloc(sizeof(char) * auth->dataPath->path.length + 6);
    if (pswpath == NULL) return SetStatus(0, "Failed to Sign-up", NULL); // Create failure status

    snprintf(pswpath, auth->dataPath->path.length + 6, "%se.env", auth->dataPath->path.path);
    FILE* file = NULL;
    file = fopen(pswpath, "r");
    if (file != NULL) {
        fclose(file); 
        free(pswpath); 
        return SetStatus(0, "Failed to Sign-up", "This Account have already exist"); // Create failure status
    }

    file = fopen(pswpath, "w");
    if (file == NULL) {
        free(pswpath);
        return SetStatus(0, "Failed to Sign-up", "Failed to create password file"); // Create failure status
    }
    fprintf(file, "USER_PSW=%s\n", auth->password);
    fclose(file);

    free(pswpath);

    return SetStatus(1, "Sign-up successful", NULL); // Create success status
}