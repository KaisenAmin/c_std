#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../encoding/encoding.h"
#include "dir.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <direct.h>
#include <shlwapi.h>

#define getcwd _getcwd
#define change_directory _wchdir
#define remove_directory _wrmdir

char* get_last_dir_name_win(const wchar_t* wpath) {
    wchar_t* lastSlash = wcsrchr(wpath, L'\\');
    if (lastSlash == NULL) {
        lastSlash = wcsrchr(wpath, L'/');
    }
    if (lastSlash != NULL && lastSlash != wpath) {
        return encoding_wchar_to_utf8(lastSlash + 1);
    }
    return NULL;
}

// Helper function for Windows to get full path
static char* get_full_path_win(const char* path) {
    wchar_t* wpath = encoding_utf8_to_wchar(path);
    wchar_t wfullpath[_MAX_PATH];

    if (_wfullpath(wfullpath, wpath, _MAX_PATH) != NULL) {
        char* fullpath = encoding_wchar_to_utf8(wfullpath);
        free(wpath);
        return fullpath;
    }

    free(wpath);
    return NULL;
}

static int remove_directory_recursive_win(const wchar_t* dir) {
    int len = wcslen(dir) + 2;  // Extra space for double null termination
    wchar_t* tempDir = (wchar_t*) malloc(len * sizeof(wchar_t));
    if (!tempDir) return -1;  // Allocation failed

    memset(tempDir, 0, len * sizeof(wchar_t));
    wcscpy(tempDir, dir);

    SHFILEOPSTRUCTW fileOp = {
        NULL,
        FO_DELETE,
        tempDir,
        NULL,
        FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT,
        false,
        0,
        L"" };

    int ret = SHFileOperationW(&fileOp);
    free(tempDir);
    return ret; // 0 on success, non-zero on failure
}

static long long dir_get_size_win(const wchar_t* dirPath) {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    long long size = 0;
    wchar_t searchPath[MAX_PATH];

    // Prepare the search path
    wsprintfW(searchPath, L"%s\\*", dirPath);
    hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                wchar_t fullPath[MAX_PATH];
                wsprintfW(fullPath, L"%s\\%s", dirPath, findFileData.cFileName);

                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    // Recursively calculate the size of directories
                    size += dir_get_size_win(fullPath);
                } else {
                    // Accumulate the size of each file
                    LARGE_INTEGER fileSize;
                    fileSize.LowPart = findFileData.nFileSizeLow;
                    fileSize.HighPart = findFileData.nFileSizeHigh;
                    size += fileSize.QuadPart;
                }
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    return size;
}

static long long get_file_size_win(const wchar_t* filePath) {
    LARGE_INTEGER size;
    HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return -1; // Error opening file
    }

    if (!GetFileSizeEx(hFile, &size)) {
        CloseHandle(hFile);
        return -1; // Error getting file size
    }

    CloseHandle(hFile);
    return size.QuadPart;
}

void dir_list_contents_win(const wchar_t* dirPath, DirListOption option) {
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wchar_t searchPath[MAX_PATH];

    // Create a search pattern
    wsprintfW(searchPath, L"%ls\\*", dirPath);
    hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                bool isDirectory = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

                if ((option == LIST_FILES && !isDirectory) || 
                    (option == LIST_DIRECTORIES && isDirectory) || 
                    (option == LIST_ALL)) {
                    wprintf(L"%ls\n", findFileData.cFileName); // Print only the file or directory name
                }
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    } else {
        wprintf(L"Failed to open directory: %ls\n", dirPath);
    }
}

#else 
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <limits.h> // For PATH_MAX
#include <errno.h>
#include <libgen.h>
#include <dirent.h>

#define change_directory chdir
#define remove_directory rmdir

static char* get_full_path_posix(const char* path) {
    char fullpath[PATH_MAX];

    if (realpath(path, fullpath) != NULL) {
        return strdup(fullpath);
    }

    return NULL;
}

static int remove_directory_recursive_posix(const char* dirPath) {
    DIR* d = opendir(dirPath);
    if (d == NULL) return -1; // Failed to open directory

    struct dirent *entry;
    int ret = 0;

    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char fullPath[PATH_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (entry->d_type == DT_DIR) {
                ret = remove_directory_recursive_posix(fullPath);
            } else {
                ret = remove(fullPath);
            }

            if (ret != 0) break; // Error occurred
        }
    }

    closedir(d);
    if (ret == 0) ret = rmdir(dirPath); // Delete the directory itself

    return ret;
}

static long long dir_get_size_posix(const char* dirPath) {
    DIR* dir = opendir(dirPath);
    struct dirent* entry;
    struct stat statbuf;
    long long size = 0;

    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char fullPath[1024];
                snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

                if (stat(fullPath, &statbuf) == -1) {
                    continue;
                }

                if (S_ISDIR(statbuf.st_mode)) {
                    // Recursively calculate the size of directories
                    size += dir_get_size_posix(fullPath);
                } else {
                    // Accumulate the size of each file
                    size += statbuf.st_size;
                }
            }
        }
        closedir(dir);
    }
    return size;
}

static long long get_file_size_posix(const char* filePath) {
    struct stat statbuf;
    if (stat(filePath, &statbuf) != 0) {
        return -1; // Error getting file size
    }
    return statbuf.st_size;
}

void dir_list_contents_posix(const char* dirPath, DirListOption option) {
    DIR* dir;
    struct dirent* entry;
    struct stat statbuf;
    char fullPath[1024];

    dir = opendir(dirPath);
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (stat(fullPath, &statbuf) == -1) {
                continue;
            }

            if (option == LIST_FILES && !S_ISDIR(statbuf.st_mode)) {
                printf("%s\n", entry->d_name);
            } else if (option == LIST_DIRECTORIES && S_ISDIR(statbuf.st_mode)) {
                printf("%s\n", entry->d_name);
            } else if (option == LIST_ALL) {
                printf("%s\n", entry->d_name);
            }
        }
        closedir(dir);
    }
}
#endif 


static char* my_strdup(const char* s) 
{
    if (s == NULL) 
        return NULL;
        
    char* new_str = malloc(strlen(s) + 1);
    if (new_str) 
        strcpy(new_str, s);

    return new_str;
}

bool dir_make_directory(const char* dirpath) {
    if (dirpath == NULL) {
        return false;
    }
    
    encoding_initialize();
    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* dirpath_wchar = encoding_utf8_to_wchar(dirpath);
    if (dirpath_wchar == NULL) {
        return false;
    }

    bool result = CreateDirectoryW(dirpath_wchar, NULL) != 0;
    free(dirpath_wchar);  // Remember to free the allocated memory
    return result;
    #else
    // For POSIX systems, use the input directly
    mode_t mode = 0755; // default permissions
    return mkdir(dirpath, mode) == 0;
    #endif
}

char* dir_dir_name(const char* dirpath) {
    if (dirpath == NULL) {
        return NULL;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wpath = encoding_utf8_to_wchar(dirpath);
    if (wpath == NULL) {
        return NULL;
    }

    char* name = NULL;
    if (wcscmp(wpath, L".") == 0) {
        wchar_t cwd[1024];
        if (_wgetcwd(cwd, sizeof(cwd) / sizeof(wchar_t)) != NULL) {
            name = get_last_dir_name_win(cwd);
        }
    } else {
        name = get_last_dir_name_win(wpath);
    }

    free(wpath);
    return name ? name : my_strdup("");

    #else
    // POSIX systems
    if (strcmp(dirpath, ".") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            const char* lastSlash = strrchr(cwd, '/');
            return lastSlash ? my_strdup(lastSlash + 1) : my_strdup(cwd);
        }
        return my_strdup("");
    } else {
        const char* lastSlash = strrchr(dirpath, '/');
        return lastSlash ? my_strdup(lastSlash + 1) : my_strdup(dirpath);
    }
    #endif
}

char* dir_current_path(void) {
    char* path = NULL;

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t wpath[1024];

    if (_wgetcwd(wpath, sizeof(wpath) / sizeof(wchar_t)) != NULL) {
        path = encoding_wchar_to_utf8(wpath);
    }
    #else 
    char buffer[1024];

    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        path = my_strdup(buffer);
    }
    #endif

    return path; 
}

int dir_count(const char* dirpath) {
    if (dirpath == NULL) {
        return -1;
    }
    int count = 0;

    #if defined(_WIN32) || defined(_WIN64)
    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    // Create a search string for all files and directories
    wchar_t wSearchPath[1024];
    MultiByteToWideChar(CP_UTF8, 0, dirpath, -1, wSearchPath, 1024);
    wcscat(wSearchPath, L"\\*"); // Append wildcard for search

    hFind = FindFirstFileW(wSearchPath, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0 && 
                    wcscmp(findFileData.cFileName, L" ") != 0) {
                count++;
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    }
    #else 
    DIR *dir = opendir(dirpath);
    if (dir == NULL) {
        return -1;
    }

    int count = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            count++;
        }
    }
    closedir(dir);
    #endif 

    return count;
}

char* dir_absolute_file_path(const char* relative_path) {
    if (relative_path == NULL) {
        return NULL;
    }

    char* absolutePath = NULL;

    #if defined(_WIN32) || defined(_WIN64)
    // For Windows
    absolutePath = get_full_path_win(relative_path);
    #else
    // For POSIX systems
    absolutePath = get_full_path_posix(relative_path);
    #endif

    return absolutePath;
}

bool dir_cd(const char* dirName) {
    if (dirName == NULL) {
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (wDirName == NULL) {
        return false;
    }

    bool result = change_directory(wDirName) == 0;
    free(wDirName);
    return result;
    #else
    return change_directory(dirName) == 0;
    #endif
}

bool dir_cd_up() {
    return dir_cd("..");
}

bool dir_remove_directory(const char* dirName) {
    if (dirName == NULL) {
        return false;
    }

    // Check if the directory is empty
    if (!dir_is_empty(dirName)) {
        printf("Directory is not empty.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (wDirName == NULL) {
        return false;
    }

    bool result = remove_directory(wDirName) == 0;
    free(wDirName);
    return result;
    #else
    return remove_directory(dirName) == 0;
    #endif
}

bool dir_is_empty(const char* dirName) {
    if (dirName == NULL) {
        printf("Directory name is NULL.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    return PathIsDirectoryEmptyA(dirName) == TRUE;
    #else
    DIR *d = opendir(dirName);
    if (d == NULL) {
        printf("Failed to open directory for checking contents.\n");
        return false;
    }

    struct dirent *entry;
    bool isEmpty = true;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            isEmpty = false;
            break;
        }
    }

    closedir(d);
    return isEmpty;
    #endif
}

bool dir_remove_directory_recursive(const char* dirPath) {
    if (dirPath == NULL) {
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t wPath[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, dirPath, -1, wPath, MAX_PATH);

    return remove_directory_recursive_win(wPath) == 0;
    #else
    return remove_directory_recursive_posix(dirPath) == 0;
    #endif
}

bool dir_rename(const char* oldName, const char* newName) {
    if (oldName == NULL || newName == NULL) {
        return false;
    }
    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* oldName_wchar = encoding_utf8_to_wchar(oldName);
    wchar_t* newName_wchar = encoding_utf8_to_wchar(newName);

    if (oldName_wchar == NULL || newName_wchar == NULL) {
        free(oldName_wchar);
        free(newName_wchar);
        return false;
    }

    bool result = MoveFileW(oldName_wchar, newName_wchar) != 0;

    free(oldName_wchar);
    free(newName_wchar);

    return result;
    #else
    if (oldName == NULL || newName == NULL) {
        return false;
    }

    int result = rename(oldName, newName);

    if (result != 0) {
        // If rename fails and errno is EXDEV, try to copy and remove
        if (errno == EXDEV) {
            // Implement copy and remove logic here if needed
            // For example, use a function to copy the file/directory and then remove the old one
            // This part is system-dependent and can be quite complex
        }
        return false;
    }

    return true;
    #endif
}

bool dir_is_directory_exists(const char* dirPath) {
    if (dirPath == NULL) {
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (wPath == NULL) {
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false;
    }

    return (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    #else
    struct stat statbuf;
    if (stat(dirPath, &statbuf) != 0) {
        return false;
    }

    return S_ISDIR(statbuf.st_mode);
    #endif

}

bool dir_is_file_exists(const char* filePath) {
    if (filePath == NULL) {
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (wPath == NULL) {
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    return (attribs != INVALID_FILE_ATTRIBUTES);
    #else
    struct stat statbuf;
    return (stat(filePath, &statbuf) == 0);
    #endif
}

bool dir_copy_file(const char* srcPath, const char* destPath) {
    if (srcPath == NULL || destPath == NULL) {
        perror("srcPath is null or destPath is null in dir_copy_file");
        return false;
    }

    // On Windows, use wide character APIs for Unicode support
    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcPath = encoding_utf8_to_wchar(srcPath);
    wchar_t* wDestPath = encoding_utf8_to_wchar(destPath);

    if (wSrcPath == NULL || wDestPath == NULL) {
        free(wSrcPath);
        free(wDestPath);
        return false;
    }

    bool result = CopyFileW(wSrcPath, wDestPath, FALSE) != 0;

    free(wSrcPath);
    free(wDestPath);

    return result;
    #else
    // POSIX implementation
    FILE* srcFile = fopen(srcPath, "rb");
    if (srcFile == NULL) {
        return false;
    }

    FILE* destFile = fopen(destPath, "wb");
    if (destFile == NULL) {
        fclose(srcFile);
        return false;
    }

    char buffer[4096];
    size_t bytesRead;
    bool success = true;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
        if (fwrite(buffer, 1, bytesRead, destFile) != bytesRead) {
            success = false;
            break;
        }
    }

    fclose(srcFile);
    fclose(destFile);

    return success;
    #endif
}

bool dir_copy_directory(const char* srcDir, const char* destDir) {
    if (srcDir == NULL || destDir == NULL) {
        perror("srcDir is null or destDir is null in dir_copy_directory");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcDir = encoding_utf8_to_wchar(srcDir);
    wchar_t* wDestDir = encoding_utf8_to_wchar(destDir);

    if (wSrcDir == NULL || wDestDir == NULL) {
        free(wSrcDir);
        free(wDestDir);
        return false;
    }

    // Append wildcard to source directory path
    wchar_t wSrcDirWildcard[MAX_PATH];
    wsprintfW(wSrcDirWildcard, L"%s\\*", wSrcDir);

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSrcDirWildcard, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        free(wSrcDir);
        free(wDestDir);
        return false;
    }

    // Create destination directory
    if (!CreateDirectoryW(wDestDir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        FindClose(hFind);
        free(wSrcDir);
        free(wDestDir);
        return false;
    }

    bool success = true;

    do {
        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
            continue;
        }

        wchar_t srcPath[MAX_PATH];
        wchar_t destPath[MAX_PATH];

        wsprintfW(srcPath, L"%s\\%s", wSrcDir, findFileData.cFileName);
        wsprintfW(destPath, L"%s\\%s", wDestDir, findFileData.cFileName);

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Recursive call to copy subdirectory
            char* srcPathUtf8 = encoding_wchar_to_utf8(srcPath);
            char* destPathUtf8 = encoding_wchar_to_utf8(destPath);
            success = dir_copy_directory(srcPathUtf8, destPathUtf8);
            free(srcPathUtf8);
            free(destPathUtf8);
        } 
        else {
            // Copy file
            char* srcPathUtf8 = encoding_wchar_to_utf8(srcPath);
            char* destPathUtf8 = encoding_wchar_to_utf8(destPath);
            success = dir_copy_file(srcPathUtf8, destPathUtf8);
            free(srcPathUtf8);
            free(destPathUtf8);
        }
    } while (FindNextFileW(hFind, &findFileData) && success);

    FindClose(hFind);
    free(wSrcDir);
    free(wDestDir);

    return success;
    #else 
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(srcDir))) {
        return false;
    }

    if (mkdir(destDir, 0755) != 0) {
        closedir(dir);
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char srcPath[1024];
        snprintf(srcPath, sizeof(srcPath), "%s/%s", srcDir, entry->d_name);

        char destPath[1024];
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, entry->d_name);

        struct stat statbuf;
        if (stat(srcPath, &statbuf) == -1) {
            closedir(dir);
            return false;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (!dir_copy_directory(srcPath, destPath)) {
                closedir(dir);
                return false;
            }
        }
        else {
            if (!dir_copy_file(srcPath, destPath)) {
                closedir(dir);
                return false;
            }
        }
    }

    closedir(dir);
    return true;

    #endif 
}

long long dir_get_directory_size(const char* dirPath) {
    if (dirPath == NULL) {
        perror("dirPath is null in dir_get_directory_size");
        return -1;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    long long size = dir_get_size_win(wPath);

    free(wPath);
    return size;
    #else
    return dir_get_size_posix(dirPath);
    #endif
}

long long dir_get_file_size(const char* filePath) {
    if (filePath == NULL) {
        perror("filePath is null in dir_get_file_size");
        return -1;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    long long size = get_file_size_win(wPath);

    free(wPath);
    return size;
    #else
    return get_file_size_posix(filePath);
    #endif
}

void dir_list_contents(const char* dirPath, DirListOption option) {
    if (dirPath == NULL) {
        perror("dirPath is null in dir_list_contents");
        return;
    }

    #if defined(_WIN32) || defined(_WIN64)

    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    dir_list_contents_win(wPath, option);
    free(wPath);
    #else
    dir_list_contents_posix(dirPath, option);
    #endif
}

bool dir_is_file(const char *filePath) {
    if (filePath == NULL) {
        perror("filePath is null");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    DWORD attribs = GetFileAttributesW(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false; // The path does not exist or error
    }
    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    free(wPath);
    return result;
    #else 
    struct stat path_stat;
    if (stat(filePath, &path_stat) != 0) {
        return false; // The path does not exist or error
    }
    return S_ISREG(path_stat.st_mode);
    #endif 
}

bool dir_is_directory(const char* dirPath) {
    if (dirPath == NULL) {
        perror("dirPath is null in dir_is_directory");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    DWORD attribs = GetFileAttributesW(wPath);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        return false; // The path does not exist or error
    }
    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    free(wPath);
    return result;
    #else 
    struct stat path_stat;
    if (stat(filePath, &path_stat) != 0) {
        return false; // The path does not exist or error
    }
    return S_ISDIR(path_stat.st_mode);
    #endif 
}

bool dir_move_file(const char* srcPath, const char* destPath) {
    if (srcPath == NULL) {
        perror("srcPath is null in dir_move");
        return false;
    }
    if (destPath == NULL) {
        perror("destPath is null in dir_move");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);
    bool result = MoveFileW(source, destination) != 0;

    free(source);
    free(destination);
    return result;
    #else 
    return rename(srcPath, destPath) == 0;
    #endif
}

bool dir_move_directory(const char* srcPath, const char* destPath) {
    if (srcPath == NULL) {
        perror("srcPath is null in dir_move");
        return false;
    }
    if (destPath == NULL) {
        perror("destPath is null in dir_move");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);
    SHFILEOPSTRUCTW fileOp = {
        NULL,
        FO_MOVE,
        source,
        destination,
        FOF_NOCONFIRMMKDIR | FOF_NOCONFIRMATION,
        FALSE,
        NULL,
        NULL
    };

    bool result = SHFileOperationW(&fileOp) == 0;
    free(source);
    free(destination);
    return result;
    #else 
    return rename(srcPath, destPath) == 0;
    #endif
}

char* dir_get_modified_time(const char* dirPath) {
    if (dirPath == NULL) {
        perror("dirPath is null in dir_get_modified_time");
        return NULL;
    }

    #if defined(_WIN32) || defined(_WIN64)
    // Convert to wide-character string
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (wPath == NULL) {
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        free(wPath);
        return NULL;
    }

    FILETIME lastModified = fileInfo.ftLastWriteTime;
    SYSTEMTIME sysTime;
    wchar_t timeString[256];
    FILETIME localFileTime;

    FileTimeToLocalFileTime(&lastModified, &localFileTime);
    FileTimeToSystemTime(&localFileTime, &sysTime); 
    wsprintfW(timeString, L"%04d-%02d-%02d %02d:%02d:%02d",
              sysTime.wYear, sysTime.wMonth, sysTime.wDay,
              sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    // Convert back to UTF-8
    char* lastTime = encoding_wchar_to_utf8(timeString);
    free(wPath); // Free the wide-character string
    return lastTime;

    #else // POSIX
    struct stat fileInfo;
    if (stat(dirPath, &fileInfo) != 0) {
        return NULL;
    }

    time_t lastModifiedTime = fileInfo.st_mtime;
    struct tm* timeinfo = localtime(&lastModifiedTime);
    char* timeString = malloc(sizeof(char) * 256);

    if (timeString == NULL) {
        return NULL;
    }

    strftime(timeString, 256, "%Y-%m-%d %H:%M:%S", timeinfo);
    return timeString;
    #endif
}

char* dir_get_creation_time(const char* dirPath) {
    if (dirPath == NULL) {
        return NULL;
    }

    #if defined(_WIN32) || defined(_WIN64)

    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        return NULL;
    }

    FILETIME creationTime = fileInfo.ftCreationTime;
    SYSTEMTIME sysTime;
    wchar_t timeString[256];
    FILETIME localFileTime;

    FileTimeToLocalFileTime(&creationTime, &localFileTime);
    FileTimeToSystemTime(&localFileTime, &sysTime);
    wsprintfW(timeString, L"%04d-%02d-%02d %02d:%02d:%02d",
              sysTime.wYear, sysTime.wMonth, sysTime.wDay,
              sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

    // Convert back to UTF-8
    char* lastTime = encoding_wchar_to_utf8(timeString);
    free(wPath);
    return lastTime;
    #else 
    printf("POSIX systems typically don't have a creation time.You can use get_last_modified_time\n");
    return NULL; 
    #endif 
}
