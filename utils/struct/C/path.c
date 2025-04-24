#include "../path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32 countTotalLength(const Path* paths, const uint8 count) {
    if (count == 0 || paths == NULL) {
        Warning("Path is required");
        return 0; // Invalid paths
    }
    
    uint32 totalLength = 0;
    for (uint8 i = 0; i < count; i++) {
        if (paths[i].path != NULL) {
            totalLength += paths[i].length;
        }
    }
    
    return totalLength; // Success
}

Path initPath(){
    Path initPath;
    initPath.path = NULL;
    initPath.length = 0;
    return initPath;
}

Path createFilePath(const Path filename, const Path dirPath, const Path fileType) {
    Path filePath = initPath();

    if(filename.length == 0 || dirPath.length == 0 || fileType.length == 0) {
        Error("required filename, dirpath, filetype");
        return filePath;
    }

    filePath.path = mergeThreeStringsLen(dirPath.path, filename.path, fileType.path, dirPath.length + filename.length + fileType.length + 1);
    if (filePath.path == NULL) {
        Error("Failed to create file path string");
        return filePath; // Memory allocation failed
    }
    filePath.length = dirPath.length + filename.length + fileType.length;
    
    return filePath; // Success
}

Path createDirPath(const Path folderName, const Path dirPath) {
    Path folderPath = initPath();
    
    if (folderName.length == 0 || dirPath.length == 0) {
        Error("required foldername, dirPath");
        return folderPath;
    }

    folderPath.path = mergeThreeStringsLen(dirPath.path, folderName.path, "/", dirPath.length + folderName.length + 1 + 1);
    if (folderPath.path == NULL) {
        Error("Failed to create folder path string");
        return folderPath; // Memory allocation failed
    }
    folderPath.length = dirPath.length + folderName.length + 1; // +1 for the '/' character
    
    return folderPath; // Success
}

Path createPath(const string path) {
    Path newPath = initPath();
    uint16 length = stringLen(path);
    if (length == 0) {
        Warning("Path is required");
        return newPath; // Invalid path
    }
    
    newPath.path = copyString(path, length);
    if (newPath.path == NULL) {
        Error("Failed to copy path string");
        return newPath; // Memory allocation failed
    }
    newPath.length = length;
    
    return newPath; // Success
}

Path createPathLen(const string path, const uint32 length) {
    Path newPath = initPath();
    if (length == 0 || path == NULL) {
        Warning("Path length is required");
        return newPath; // Invalid length
    }
    
    newPath.path = copyString(path, length);
    if (newPath.path == NULL) {
        Error("Failed to copy path string");
        return newPath; // Memory allocation failed
    }
    newPath.length = length;
    
    return newPath; // Success
}

Path mergeTwoPaths(const Path path1, const Path path2) {
    Path newPath = initPath();
    if (path1.path == NULL || path2.path == NULL) {
        Warning("Path is required");
        return newPath; // Invalid paths
    }
    
    string mergedPath = mergeTwoStringsLen(path1.path, path2.path, path1.length + path2.length);
    if (mergedPath == NULL) {
        Error("Failed to merge paths");
        return newPath; // Memory allocation failed
    }
    newPath = createPathLen(mergedPath, path1.length + path2.length);
    free(mergedPath);
    return newPath; // Success
}

Path mergeThreePaths(const Path path1, const Path path2, const Path path3) {
    Path newPath = initPath();
    if (path1.path == NULL || path2.path == NULL || path3.path == NULL) {
        Warning("Path is required");
        return newPath; // Invalid paths
    }
    
    string mergedPath = mergeThreeStringsLen(path1.path, path2.path, path3.path, path1.length + path2.length + path3.length);
    if (mergedPath == NULL) {
        Error("Failed to merge paths");
        return newPath; // Memory allocation failed
    }
    newPath = createPathLen(mergedPath, path1.length + path2.length + path3.length);
    free(mergedPath);
    return newPath; // Success
}

Path mergePaths(const Path* paths, const uint8 count) {
    Path newPath = initPath();
    if (count == 0 || paths == NULL) {
        Warning("Path is required");
        return newPath; // Invalid paths
    }
    
    uint32 totalLength = countTotalLength(paths, count);
    if (totalLength == 0) {
        Warning("Total length is zero");
        return newPath; // Invalid paths
    }

    string* totolStrings = (string*)malloc(sizeof(string) * count);
    if (totolStrings == NULL) {
        Error("Failed to allocate memory for total strings");
        return newPath; // Memory allocation failed
    }
    for (uint8 i = 0; i < count; i++) {
        totolStrings[i] = paths[i].path;
    }
    
    string mergedPath = mergeStringsLen(totolStrings, count, totalLength);
    if (mergedPath == NULL) {
        Error("Failed to merge paths");
        return newPath; // Memory allocation failed
    }
    
    newPath = createPathLen(mergedPath, totalLength);
    free(mergedPath);
    return newPath; // Success
}

void FreePath(Path* path) {
    if (path != NULL && path->path != NULL) {
        free(path->path);
        path->path = NULL;
        path->length = 0;
    }
}
