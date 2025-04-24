#ifndef FILE_H
#define FILE_H

#include "../datatype/int-type.h"
#include "../datatype/string-type.h"

#include "../common/status.h"
#include "../common/log.h"

#include "path.h"

typedef struct File {
    Path filename; // file name
    Path dirPath; // file path
    Path fileType; // file type
} File;

typedef struct Folder {
    Path folderName; // folder name
    Path dirPath; // folder path
} Folder;

File* initFile();
File* createFile(const string filename, const string dir, const string fileType);
File* createFileWithPath(const Path filename, const Path dirPath, const Path fileType);
code MakeFile(const File* file);
code MakeFolder(const Folder* folder);
code MakeFilePath(const string filepath);
code MakeFolderPath(const string folderPath);
code FileExist(const string filePath);
code FolderExist(const string folderPath);
void FreeFile(File* file);

#endif // FILE_H