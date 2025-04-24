#ifndef INT_TYPE_H
#define INT_TYPE_H
// Unsigned integer types
typedef unsigned char  uint8;  // 0 to 255
typedef unsigned short uint16; // 0 to 65535
typedef unsigned int   uint32; // 0 to 4294967295
typedef unsigned long long uint64; // 0 to 18446744073709551615

// Signed integer types
typedef signed char  int8;   // -128 to 127
typedef signed short int16;  // -32768 to 32767
typedef signed int   int32;  // -2147483648 to 2147483647
typedef signed long long int64;  // -9223372036854775808 to 9223372036854775807

typedef int8 code; // 0-failed, 1-success, -1-warning

#endif // INT_TYPE_H