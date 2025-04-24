#include "../file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

File* initFile() {
    File* file = (File*)malloc(sizeof(File));
    if (file == NULL) {
        Error("Memory allocation failed for File");
        return NULL; // Memory allocation failed
    }
    
    return file; // Success
}

File* createFile(const string filename, const string dir, const string fileType) {
    if (filename == NULL || dir == NULL || fileType == NULL) {
        Warning("Filename, directory, and file type are required");
        return NULL; // Invalid parameters
    }
    
    File* newFile = initFile();
    if (newFile == NULL) {
        return NULL; // Memory allocation failed
    }
    
    newFile->filename = createPath(filename);
    if (newFile->filename.path == NULL) {
        Error("Failed to copy filename path string");
        FreeFile(newFile);
        return NULL; // Memory allocation failed
    }
    newFile->dirPath = createPath(dir); // Copy dirPath
    if (newFile->dirPath.path == NULL) {
        Error("Failed to copy directory path string");
        FreeFile(newFile);
        return NULL; // Memory allocation failed
    }
    newFile->fileType = createPath(fileType);
    if (newFile->fileType.path == NULL) {
        Error("Failed to copy file type path");
        FreeFile(newFile);
        return NULL; // Memory allocation failed
    }
    
    return newFile; // Success
}

File* createFileWithPath(const Path filename, const Path dirPath, const Path fileType) {
    if (filename.path == NULL || dirPath.path == NULL || fileType.path == NULL) {
        Warning("Filename, directory path, and file type are required");
        return NULL; // Invalid parameters
    }
    
    File* newFile = initFile();
    if (newFile == NULL) {
        return NULL; // Memory allocation failed
    }
    
    newFile->filename = filename;
    newFile->dirPath = dirPath; // Copy dirPath
    newFile->fileType = fileType;

    return newFile; // Success
}

code MakeFile(const File* file) {
    if (file == NULL) {
        Error("file required");
        return 0;
    }
    string filepath = createFilePath(file->filename, file->dirPath, file->fileType).path;
    return MakeFilePath(filepath);
}

code MakeFolder(const Folder* folder) {
    if (folder == NULL) {
        Error("folder required");
        return 0;
    }
    string folderPath = createDirPath(folder->folderName, folder->dirPath).path;
    return MakeFolderPath(folderPath);
}

code MakeFilePath(const string filepath) {
    if (FileExist(filepath)) {
        Warning("File have already Exist");
        return -1;
    }
    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        Error("Failed to create file");
        return 0; // File creation failed
    }

    fclose(file);
    return 1; // File created successfully
}

code MakeFolderPath(const string folderPath) {
    if (folderPath == NULL) {
        Error("folderPath is NULL");
        return 0; // Invalid folder path
    }

    if (mkdir(folderPath) == -1) {
        Error("Failed to create folder");
        return 0; // Folder creation failed
    }

    return 1; // Folder created successfully
}

code FileExist(const string filePath) {
    if (filePath == NULL) {
        Error("filepath is NULL");
        return 0; // Invalid file path
    }

    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        return 0;
    }
    return 1;
}

code FolderExist(const string folderPath) {
    if (folderPath == NULL) {
        Error("folderPath is NULL");
        return 0; // Invalid folder path
    }

    DIR* dir = opendir(folderPath);
    if (dir == NULL) {
        return 0; // Folder does not exist
    }

    closedir(dir);
    return 1; // Folder exists
}

void FreeFile(File* file) {
    if (file != NULL) {
        FreePath(&file->filename);
        FreePath(&file->dirPath);
        FreePath(&file->fileType);
    }
}