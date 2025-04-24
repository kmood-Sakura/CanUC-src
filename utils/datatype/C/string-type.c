#include "../string-type.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32 stringLen(const string str) {
    if (str == NULL) {
        return 0;
    }
    
    uint32 length = 0;
    while (str[length] != '\0') {
        length++;
    }
    
    return length;
}

code stringCmp(const string str1, const string str2) {
    if (str1 == NULL || str2 == NULL) {
        fprintf(stderr, "NULL string\n");
        return 0; // Invalid comparison
    }
    
    int32 result = strcmp(str1, str2);
    return (result == 0) ? 1 : 0; // 1 for equal, 0 for not equal
}

// Base function
string createString(const string str) {
    if (str == NULL) {
        return NULL;
    }
    uint32 length = strlen(str) + 1; // +1 for null terminator
    string newStr = (string)malloc(length * sizeof(char));
    if (newStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    strcpy(newStr, str);
    return newStr;
}

string copyString(const string referStr, const uint32 length) {
    if (referStr == NULL || length == 0) {
        fprintf(stderr, "Invalid string or length\n");
        return NULL;
    }
    string newStr = (string)malloc(length + 1 * sizeof(char));
    if (newStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    strcpy(newStr, referStr);
    return newStr;
}

string mergeTwoStrings(const string str1, const string str2) {
    if (str1 == NULL && str2 == NULL) {
        return NULL;
    }
    if (str1 == NULL) {
        return createString(str2);
    }
    if (str2 == NULL) {
        return createString(str1);
    }

    uint32 totalLength = stringLen(str1) + stringLen(str2);
    
    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strcpy(mergedStr, str1);
    strcat(mergedStr, str2);
    
    return mergedStr;
}

string mergeThreeStrings(const string str1, const string str2, const string str3) {
    if (str1 == NULL && str2 == NULL && str3 == NULL) {
        return NULL;
    }
    if (str1 == NULL) {
        return mergeTwoStrings(str2, str3);
    }
    if (str2 == NULL) {
        return mergeTwoStrings(str1, str3);
    }
    if (str3 == NULL) {
        return mergeTwoStrings(str1, str2);
    }

    uint32 totalLength = stringLen(str1) + stringLen(str2) + stringLen(str3);
    
    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strcpy(mergedStr, str1);
    strcat(mergedStr, str2);
    strcat(mergedStr, str3);

    return mergedStr;
}

string mergeStrings(const string* strings,uint8 count) {
    if (strings == NULL || count == 0) {
        return NULL;
    }

    uint32 totalLength = 0;
    for (uint8 i = 0; i < count; i++) {
        totalLength += stringLen(strings[i]);
    }

    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    mergedStr[0] = '\0'; // Initialize the merged string

    for (uint8 i = 0; i < count; i++) {
        strcat(mergedStr, strings[i]);
    }

    return mergedStr;
}

string mergeTwoStringsLen(const string str1, const string str2, const uint32 totalLength) {
    if (str1 == NULL && str2 == NULL) {
        return NULL;
    }
    if (str1 == NULL) {
        return createString(str2);
    }
    if (str2 == NULL) {
        return createString(str1);
    }

    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strncpy(mergedStr, str1, totalLength);
    mergedStr[totalLength] = '\0'; // Null-terminate the first part
    strcat(mergedStr, str2);

    return mergedStr;
}

string mergeThreeStringsLen(const string str1, const string str2, const string str3, const uint32 totalLength) {
    if (str1 == NULL && str2 == NULL && str3 == NULL) {
        return NULL;
    }
    if (str1 == NULL) {
        return mergeTwoStringsLen(str2, str3, totalLength);
    }
    if (str2 == NULL) {
        return mergeTwoStringsLen(str1, str3, totalLength);
    }
    if (str3 == NULL) {
        return mergeTwoStringsLen(str1, str2, totalLength);
    }

    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    strncpy(mergedStr, str1, totalLength);
    mergedStr[totalLength] = '\0'; // Null-terminate the first part
    strcat(mergedStr, str2);
    strcat(mergedStr, str3);

    return mergedStr;
}

string mergeStringsLen(const string* strings, uint8 count, const uint32 totalLength) {
    if (strings == NULL || count == 0) {
        return NULL;
    }

    string mergedStr = (string)malloc(totalLength + 1 * sizeof(char));
    if (mergedStr == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }

    mergedStr[0] = '\0'; // Initialize the merged string

    for (uint8 i = 0; i < count; i++) {
        strcat(mergedStr, strings[i]);
    }

    return mergedStr;
}

void freeString(string str) {
    if (str != NULL) {
        free(str);
    }
}