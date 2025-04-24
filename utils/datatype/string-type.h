#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include "int-type.h"

typedef char* string; // string size upon uint32

uint32 stringLen(const string str); // get string length
code stringCmp(const string str1, const string str2); // compare two strings

string createString(const string str); // create string
string copyString(const string referStr, const uint32 length); // copy string
string mergeTwoStrings(const string str1, const string str2); // merge two strings
string mergeThreeStrings(const string str1, const string str2, const string str3); // merge three strings
string mergeStrings(const string* strings,uint8 count); // merge strings
string mergeTwoStringsLen(const string str1, const string str2, const uint32 totalLength); // merge two strings with length
string mergeThreeStringsLen(const string str1, const string str2, const string str3, const uint32 totalLength); // merge three strings with length
string mergeStringsLen(const string* strings,uint8 count, const uint32 totalLength); // merge strings with length

void freeString(string str); // free string

#endif // STRING_TYPE_H