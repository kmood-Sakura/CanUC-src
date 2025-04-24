#ifndef PATH_H  
#define PATH_H

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"

#include "../common/status.h"
#include "../common/log.h"
#include "../common/request.h"

typedef struct Path {
    string path;
    uint32 length;
} Path; // Path is a string

Path initPath(); // initialize path
Path createFilePath(const Path filename, const Path dirPath, const Path fileType);
Path createDirPath(const Path folderName, const Path dirPath);
Path createPath(const string path); // create path from string
Path createPathLen(const string path, const uint32 length); // create path from string with length
Path mergeTwoPaths(const Path path1, const Path path2); // merge two paths
Path mergeThreePaths(const Path path1, const Path path2, const Path path3); // merge three paths
Path mergePaths(const Path* paths, const uint8 count); // merge paths

void FreePath(Path* path); // free path
uint32 countTotalLength(const Path* paths, const uint8 count); // count total length of paths

#endif // PATH_H