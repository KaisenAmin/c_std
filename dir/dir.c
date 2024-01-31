/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Dir
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../encoding/encoding.h"
#include "../crypto/crypto.h"
#include "../string/string.h"
#include "../file_io/file_reader.h"
#include "../file_io/file_writer.h"
#include "../fmt/fmt.h"
#include "dir.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <direct.h>
#include <shlwapi.h>
#include <shlobj.h> 
#include "accctrl.h"
#include "aclapi.h"

#define getcwd _getcwd
#define change_directory _wchdir
#define remove_directory _wrmdir

char* get_last_dir_name_win(const wchar_t* wpath) {
    if (!wpath) {
        fmt_fprintf(stderr, "Error: Path is NULL in get_last_dir_name_win.\n");
        return NULL;
    }

    wchar_t* lastSlash = wcsrchr(wpath, L'\\');
    if (lastSlash == NULL) {
        lastSlash = wcsrchr(wpath, L'/');
    }
    if (lastSlash != NULL && lastSlash != wpath) {
        char* dirName = encoding_wchar_to_utf8(lastSlash + 1);
        if (!dirName) {
            fmt_fprintf(stderr, "Error: Failed to convert directory name to UTF-8 in get_last_dir_name_win.\n");
        }
        return dirName;
    }
    fmt_fprintf(stderr, "Error: Last directory name not found in path in get_last_dir_name_win.\n");

    return NULL;
}

// Helper function for Windows to get full path
static char* get_full_path_win(const char* path) {
    if (!path) {
        fmt_fprintf(stderr, "Error: Path is NULL in get_full_path_win.\n");
        return NULL;
    }

    wchar_t* wpath = encoding_utf8_to_wchar(path);
    if (!wpath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in get_full_path_win.\n");
        return NULL;
    }

    wchar_t wfullpath[_MAX_PATH];
    if (_wfullpath(wfullpath, wpath, _MAX_PATH) != NULL) {
        char* fullpath = encoding_wchar_to_utf8(wfullpath);
        free(wpath);
        if (!fullpath) {
            fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in get_full_path_win.\n");
        }
        return fullpath;
    }
    fmt_fprintf(stderr, "Error: Unable to get full path for %s in get_full_path_win.\n", path);
    free(wpath);
    return NULL;
}

static int remove_directory_recursive_win(const wchar_t* dir) {
    if (!dir) {
        fmt_fprintf(stderr, "Error: Directory is NULL in remove_directory_recursive_win.\n");
        return -1;
    }

    int len = wcslen(dir) + 2;  // Extra space for double null termination
    wchar_t* tempDir = (wchar_t*) malloc(len * sizeof(wchar_t));
    if (!tempDir) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in remove_directory_recursive_win.\n");
        return -1;
    }

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
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_get_size_win.\n");
        return -1;
    }

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    long long size = 0;
    wchar_t searchPath[MAX_PATH];

    // Prepare the search path
    wsprintfW(searchPath, L"%s\\*", dirPath);
    hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: Failed to open directory %ls in dir_get_size_win.\n", dirPath);
        return -1;
    }

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
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in get_file_size_win.\n");
        return -1;
    }

    LARGE_INTEGER size;
    HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: Unable to open file %ls in get_file_size_win.\n", filePath);
        return -1;
    }
    if (!GetFileSizeEx(hFile, &size)) {
        fmt_fprintf(stderr, "Error: Unable to get file size for %ls in get_file_size_win.\n", filePath);
        CloseHandle(hFile);
        return -1;
    }

    CloseHandle(hFile);
    return size.QuadPart;
}

void dir_list_contents_win(const wchar_t* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath || !resultVector) {
        fmt_fprintf(stderr, "Error: Directory path or result vector is NULL in dir_list_contents_win.\n");
        return;
    }

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wchar_t searchPath[MAX_PATH];

    // Create a search pattern
    wsprintfW(searchPath, L"%ls\\*", dirPath);
    hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: Failed to open directory %ls in dir_list_contents_win.\n", dirPath);
        return;
    }

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                bool isDirectory = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

                if ((option == DIR_LIST_FILES && !isDirectory) || 
                    (option == DIR_LIST_DIRECTORIES && isDirectory) || 
                    (option == DIR_LIST_ALL)) {
                        char* utf8FileName = encoding_wchar_to_utf8(findFileData.cFileName);
                        if (!utf8FileName) {
                            fmt_fprintf(stderr, "Error: Failed to convert filename to UTF-8 in dir_list_contents_win.\n");
                            continue;
                        }
                        vector_push_back(resultVector, &utf8FileName);
                    // wprintf(L"%ls\n", findFileData.cFileName); // Print only the file or directory name
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
#include <sys/types.h>
#include <pwd.h>
#include <fnmatch.h>

#define change_directory chdir
#define remove_directory rmdir

static char* get_full_path_posix(const char* path) {
    if (!path) {
        fmt_fprintf(stderr, "Error: Path is NULL in get_full_path_posix.\n");
        return NULL;
    }

    char fullpath[PATH_MAX];
    if (realpath(path, fullpath) == NULL) {
        fmt_fprintf(stderr, "Error: Failed to resolve realpath for %s in get_full_path_posix.\n", path);
        return NULL;
    }

    char* resolvedPath = strdup(fullpath);
    if (!resolvedPath) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in get_full_path_posix for path %s.\n", fullpath);
    }
    return resolvedPath;
}

static int remove_directory_recursive_posix(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in remove_directory_recursive_posix.\n");
        return -1;
    }
    DIR* d = opendir(dirPath);
    if (!d) {
        fmt_fprintf(stderr, "Error: Failed to open directory %s in remove_directory_recursive_posix.\n", dirPath);
        return -1;
    }

    struct dirent *entry;
    int ret = 0;

    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char fullPath[PATH_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (entry->d_type == DT_DIR) {
                ret = remove_directory_recursive_posix(fullPath);
            } 
            else {
                ret = remove(fullPath);
            }

            if (ret != 0) {
                fmt_fprintf(stderr, "Error: Failed to remove %s in remove_directory_recursive_posix.\n", fullPath);
                break; // Error occurred
            }
        }
    }

    closedir(d);
    if (ret == 0 && rmdir(dirPath) != 0) {
        fmt_fprintf(stderr, "Error: Failed to remove directory %s in remove_directory_recursive_posix.\n", dirPath);
        return -1;
    }
    return ret;
}

static long long dir_get_size_posix(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_get_size_posix.\n");
        return -1;
    }

    DIR* dir = opendir(dirPath);
    if (!dir) {
        fmt_fprintf(stderr, "Error: Failed to open directory %s in dir_get_size_posix.\n", dirPath);
        return -1;
    }

    struct dirent* entry;
    struct stat statbuf;
    long long size = 0;

    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char fullPath[1024];
                snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

                if (stat(fullPath, &statbuf) == -1) {
                fmt_fprintf(stderr, "Error: Failed to get stats for file %s in dir_get_size_posix.\n", fullPath);
                continue;
            }

                if (S_ISDIR(statbuf.st_mode)) {
                    // Recursively calculate the size of directories
                    size += dir_get_size_posix(fullPath);
                } 
                else {
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
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in get_file_size_posix.\n");
        return -1;
    }

    struct stat statbuf;
    if (stat(filePath, &statbuf) != 0) {
        fmt_fprintf(stderr, "Error: Failed to get file stats for %s in get_file_size_posix.\n", filePath);
        return -1;
    }
    return statbuf.st_size;
}

void dir_list_contents_posix(const char* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath || !resultVector) {
        fmt_fprintf(stderr, "Error: Directory path or result vector is NULL in dir_list_contents_posix.\n");
        return;
    }

    DIR* dir;
    if (!dir) {
        fmt_fprintf(stderr, "Error: Failed to open directory %s in dir_list_contents_posix.\n", dirPath);
        return;
    }

    struct dirent* entry;
    struct stat statbuf;
    char fullPath[1024];

    dir = opendir(dirPath);
    if (dir) {
        while ((entry = readdir(dir)) != NULL) {
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (stat(fullPath, &statbuf) == -1) {
                fmt_fprintf(stderr, "Error: Failed to get stats for file %s in dir_list_contents_posix.\n", fullPath);
                continue;
            }
            if (option == LIST_FILES && !S_ISDIR(statbuf.st_mode)) {
                printf("%s\n", entry->d_name);
            } 
            else if (option == LIST_DIRECTORIES && S_ISDIR(statbuf.st_mode)) {
                printf("%s\n", entry->d_name);
            } 
            else if (option == LIST_ALL) {
                printf("%s\n", entry->d_name);
            }

            if ((option == LIST_FILES && !S_ISDIR(statbuf.st_mode)) ||
                (option == LIST_DIRECTORIES && S_ISDIR(statbuf.st_mode)) ||
                (option == LIST_ALL)) {
                char* entryNameCopy = string_strdup(entry->d_name);
                vector_push_back(resultVector, &entryNameCopy);
            }
        }
        closedir(dir);
    }
}
#endif 

static void derive_key_from_password(const char* password, uint8_t* key) {
    if (!password || !key) {
        fmt_fprintf(stderr, "Error: Password or key buffer is NULL in derive_key_from_password.\n");
        return;
    }
    size_t pass_len = strlen(password);
    size_t key_len = 8;  // DES key size is 8 bytes

    if (pass_len > key_len) {
        // Truncate the password if it's longer than the key size
        memcpy(key, password, key_len);
    } 
    else {
        // Pad the key with zeros if the password is shorter than the key size
        memcpy(key, password, pass_len);
        memset(key + pass_len, 0, key_len - pass_len);
    }
}

bool dir_make_directory(const char* dirpath) {
    if (!dirpath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_make_directory.\n");
        return false;
    }
    
    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* dirpath_wchar = encoding_utf8_to_wchar(dirpath);
    if (!dirpath_wchar) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_make_directory.\n");
        return false;
    }

   if (!CreateDirectoryW(dirpath_wchar, NULL)) {
        fmt_fprintf(stderr, "Error: Unable to create directory %s in dir_make_directory.\n", dirpath);
        free(dirpath_wchar);
        return false;
    }
    free(dirpath_wchar);
    return true;
    #else
    mode_t mode = 0755; // default permissions
    if (mkdir(dirpath, mode) != 0) {
        fmt_fprintf(stderr, "Error: Unable to create directory %s in dir_make_directory.\n", dirpath);
        return false;
    }
    #endif
}

char* dir_dir_name(const char* dirpath) {
    if (!dirpath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_dir_name.\n");
        return NULL;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wpath = encoding_utf8_to_wchar(dirpath);
    if (!wpath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_dir_name.\n");
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
    return name ? name : string_strdup("");

    #else
    // POSIX systems
    if (strcmp(dirpath, ".") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            const char* lastSlash = strrchr(cwd, '/');
            return lastSlash ? string_strdup(lastSlash + 1) : string_strdup(cwd);
        }
        return string_strdup("");
    } else {
        const char* lastSlash = strrchr(dirpath, '/');
        return lastSlash ? string_strdup(lastSlash + 1) : string_strdup(dirpath);
    }
    #endif
}

char* dir_current_path(void) {
    char* path = NULL;

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t wpath[1024];
    if (_wgetcwd(wpath, sizeof(wpath) / sizeof(wchar_t)) == NULL) {
        fmt_fprintf(stderr, "Error: Unable to get current working directory in dir_current_path on Windows.\n");
        return NULL;
    }
    
    path = encoding_wchar_to_utf8(wpath);
    if (!path) {
        fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in dir_current_path.\n");
    }
    #else 
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        fmt_fprintf(stderr, "Error: Unable to get current working directory in dir_current_path on POSIX.\n");
        return NULL;
    }
    path = string_strdup(buffer);
    if (!path) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in dir_current_path.\n");
    }
    #endif

    return path; 
}

int dir_count(const char* dirpath) {
    if (!dirpath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_count.\n");
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
    if (!dir) {
        fmt_fprintf(stderr, "Error: Failed to open directory %s in dir_count.\n", dirpath);
        return -1;
    }

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
    if (!relative_path) {
        fmt_fprintf(stderr, "Error: Relative path is NULL in dir_absolute_file_path.\n");
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
    if (!dirName) {
        fmt_fprintf(stderr, "Error: Directory name is NULL in dir_cd.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (!wDirName) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_cd for directory %s.\n", dirName);
        return false;
    }
    if (change_directory(wDirName) != 0) {
        fmt_fprintf(stderr, "Error: Change directory failed in dir_cd for directory %s.\n", dirName);
        free(wDirName);
        return false;
    }

    free(wDirName);
    return true;
    #else
    if (change_directory(dirName) != 0) {
        fmt_fprintf(stderr, "Error: Change directory failed in dir_cd for directory %s.\n", dirName);
        return false;
    }
    return true;
    #endif
}

bool dir_cd_up() {
    return dir_cd("..");
}

bool dir_remove_directory(const char* dirName) {
    if (!dirName) {
        fmt_fprintf(stderr, "Error: Directory name is NULL in dir_remove_directory.\n");
        return false;
    }
    if (!dir_is_empty(dirName)) {
        fmt_fprintf(stderr, "Error: Directory %s is not empty in dir_remove_directory.\n", dirName);
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (!wDirName) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_remove_directory for directory %s.\n", dirName);
        return false;
    }

    bool result = remove_directory(wDirName) == 0;
    free(wDirName);

    if (!result) {
        fmt_fprintf(stderr, "Error: remove_directory failed for directory %s in dir_remove_directory.\n", dirName);
    }
    return result;
    #else
    if (remove_directory(dirName) != 0) {
        fmt_fprintf(stderr, "Error: rmdir failed for directory %s in dir_remove_directory.\n", dirName);
        return false;
    }
    return true;
    #endif
}

bool dir_is_empty(const char* dirName) {
    if (!dirName) {
        fmt_fprintf(stderr, "Error: Directory name is NULL in dir_is_empty.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    if (PathIsDirectoryEmptyA(dirName) != TRUE) {
        fmt_fprintf(stderr, "Error: Directory %s is not empty in dir_is_empty.\n", dirName);
        return false;
    }
    return true;
    #else
    DIR *d = opendir(dirName);
    if (!d) {
        fmt_fprintf(stderr, "Error: Failed to open directory %s for checking contents in dir_is_empty.\n", dirName);
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
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_remove_directory_recursive.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t wPath[MAX_PATH];
    // MultiByteToWideChar(CP_UTF8, 0, dirPath, -1, wPath, MAX_PATH);

    if (MultiByteToWideChar(CP_UTF8, 0, dirPath, -1, wPath, MAX_PATH) == 0) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_remove_directory_recursive for directory %s.\n", dirPath);
        return false;
    }

    if (remove_directory_recursive_win(wPath) != 0) {
        fmt_fprintf(stderr, "Error: Recursive directory removal failed in dir_remove_directory_recursive for directory %s.\n", dirPath);
        return false;
    }
    return true;
    #else
    if (remove_directory_recursive_posix(dirPath) != 0) {
        fmt_fprintf(stderr, "Error: Recursive directory removal failed in dir_remove_directory_recursive for directory %s.\n", dirPath);
        return false;
    }
    return true;
    #endif
}

bool dir_rename(const char* oldName, const char* newName) {
    if (!oldName || !newName) {
        fmt_fprintf(stderr, "Error: Old name or new name is NULL in dir_rename.\n");
        return false;
    }
    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* oldName_wchar = encoding_utf8_to_wchar(oldName);
    wchar_t* newName_wchar = encoding_utf8_to_wchar(newName);

    if (!oldName_wchar || !newName_wchar) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_rename.\n");
        free(oldName_wchar);
        free(newName_wchar);
        return false;
    }
    if (!MoveFileW(oldName_wchar, newName_wchar)) {
        fmt_fprintf(stderr, "Error: MoveFileW failed to rename from %s to %s.\n", oldName, newName);
        free(oldName_wchar);
        free(newName_wchar);
        return false;
    }

    free(oldName_wchar);
    free(newName_wchar);

    return true;
    #else
    if (rename(oldName, newName) != 0) {
        fmt_fprintf(stderr, "Error: rename failed to rename from %s to %s.\n", oldName, newName);
        return false;
    }
    return true;
    #endif
}

bool dir_is_directory_exists(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_is_directory_exists.\n");
        return false;
    }

    #if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (wPath == NULL) {
        fmt_fprintf(stderr, "Error: Failed in converting utf8 to wchar in dir_is_directory_exists.\n");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        fmt_fprintf(stderr, "Error: GetFileAttributesW failed or file does not exist in dir_is_directory_exists.\n");
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
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in dir_is_file_exists.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (wPath == NULL) {
        fmt_fprintf(stderr, "Error: Failed in converting utf8 to wchar in dir_is_file_exists.\n");
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
    if (!srcPath || !destPath) {
        fmt_fprintf(stderr, "Error: Source or destination path is NULL in dir_copy_file.\n");
        return false;
    }

    // On Windows, use wide character APIs for Unicode support
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcPath = encoding_utf8_to_wchar(srcPath);
    wchar_t* wDestPath = encoding_utf8_to_wchar(destPath);

    if (!wSrcPath || !wDestPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_copy_file.\n");
        free(wSrcPath);
        free(wDestPath);
        return false;
    }

    bool result = CopyFileW(wSrcPath, wDestPath, FALSE) != 0;
    if (!result) {
        fmt_fprintf(stderr, "Error: CopyFileW failed to copy from %s to %s.\n", srcPath, destPath);
    }

    free(wSrcPath);
    free(wDestPath);

    return result;
#else
    // POSIX implementation
    FileReader* srcFile = file_reader_open(srcPath, READ_BINARY);
    if (!srcFile) {
        fmt_fprintf(stderr, "Error: Unable to open source file %s for reading in dir_copy_file.\n", srcPath);
        return false;
    }

    FileWriter* destFile = file_writer_open(destPath, WRITE_BINARY);
    if (!destFile) {
        file_reader_close(srcFile);
        fmt_fprintf(stderr, "Error: Unable to open destination file %s for writing in dir_copy_file.\n", destPath);
        return false;
    }

    char buffer[4096];
    size_t bytesRead;
    bool success = true;

    while (!file_reader_eof(srcFile)) {
        bytesRead = file_reader_read(buffer, 1, sizeof(buffer), srcFile);
        if (bytesRead == 0) {
            break; // No more data to read
        }

        if (file_writer_write(buffer, 1, bytesRead, destFile) != bytesRead) {
            success = false;
            break;
        }
    }

    file_reader_close(srcFile);
    file_writer_close(destFile);

    return success;
#endif
}

bool dir_copy_directory(const char* srcDir, const char* destDir) {
    if (!srcDir || !destDir) {
        fmt_fprintf(stderr, "Error: Source or destination directory is NULL in dir_copy_directory.\n");
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
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_get_directory_size.\n");
        return -1;
    }
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_directory_size.\n");
        return -1;
    }

    long long size = dir_get_size_win(wPath);
    if (size == -1) {
        fmt_fprintf(stderr, "Error: Failed to get directory size for %s.\n", dirPath);
    }

    free(wPath);
    return size;
#else
    long long size = dir_get_size_posix(dirPath);
    if (size == -1) {
        fmt_fprintf(stderr, "Error: Failed to get directory size for %s.\n", dirPath);
    }
    return size;
#endif
}

long long dir_get_file_size(const char* filePath) {
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in dir_get_file_size.\n");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_file_size.\n");
        return -1;
    }

    long long size = get_file_size_win(wPath);
    if (size == -1) {
        fmt_fprintf(stderr, "Error: Failed to get file size for %s.\n", filePath);
    }

    free(wPath);
    return size;
#else
    long long size = get_file_size_posix(filePath);
    if (size == -1) {
        fmt_fprintf(stderr, "Error: Failed to get file size for %s.\n", filePath);
    }
#endif
}

void dir_list_contents(const char* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_list_contents.\n");
        return;
    }
    if (!resultVector) {
        fmt_fprintf(stderr, "Error: Result vector is NULL in dir_list_contents.\n");
        return;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_list_contents.\n");
        return;
    }

    dir_list_contents_win(wPath, option, resultVector);
    free(wPath);
#else
    dir_list_contents_posix(dirPath, option, resultVector);
#endif
}

bool dir_is_file(const char *filePath) {
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in dir_is_file.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_is_file.\n");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        fmt_fprintf(stderr, "Error: GetFileAttributesW failed or file does not exist in dir_is_file.\n");
        return false; // The path does not exist or error
    }
    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    free(wPath);
    return result;
#else 
    struct stat path_stat;
    if (stat(filePath, &path_stat) != 0) {
        fmt_fprintf(stderr, "Error: stat failed or file does not exist in dir_is_file.\n");
        return false;
    }
    return S_ISREG(path_stat.st_mode);
#endif 
}

bool dir_is_directory(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_is_directory.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_is_directory.\n");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    if (attribs == INVALID_FILE_ATTRIBUTES) {
        fmt_fprintf(stderr, "Error: GetFileAttributesW failed or directory does not exist in dir_is_directory.\n");
        return false; // The path does not exist or error
    }
    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    free(wPath);
    return result;
#else 
    struct stat path_stat;
    if (stat(dirPath, &path_stat) != 0) {
        fmt_fprintf(stderr, "Error: stat failed or directory does not exist in dir_is_directory.\n");
        return false;
    }
    return S_ISDIR(path_stat.st_mode);
#endif 
}

bool dir_move_file(const char* srcPath, const char* destPath) {
    if (!srcPath) {
        fmt_fprintf(stderr, "Error: Source path is NULL in dir_move_file.\n");
        return false;
    }
    if (!destPath) {
        fmt_fprintf(stderr, "Error: Destination path is NULL in dir_move_file.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);
    if (!source || !destination) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_move_file.\n");
        free(source);
        free(destination);
        return false;
    }

    bool result = MoveFileW(source, destination) != 0;
    if (!result) {
        fmt_fprintf(stderr, "Error: MoveFileW failed in dir_move_file.\n");
    }

    free(source);
    free(destination);
    return result;
#else 
    if (rename(srcPath, destPath) != 0) {
        fmt_fprintf(stderr, "Error: rename failed in dir_move_file.\n");
        return false;
    }
#endif
}

bool dir_move_directory(const char* srcPath, const char* destPath) {
    if (!srcPath) {
        fmt_fprintf(stderr, "Error: Source path is NULL in dir_move_directory.\n");
        return false;
    }
    if (!destPath) {
        fmt_fprintf(stderr, "Error: Destination path is NULL in dir_move_directory.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);

    if (!source || !destination) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_move_directory.\n");
        free(source);
        free(destination);
        return false;
    }

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
    if (!result) {
        fmt_fprintf(stderr, "Error: SHFileOperationW failed in dir_move_directory.\n");
    }

    return result;
#else 
    if (rename(srcPath, destPath) != 0) {
        fmt_fprintf(stderr, "Error: rename failed in dir_move_directory for paths %s to %s.\n", srcPath, destPath);
        return false;
    }
#endif
}

char* dir_get_modified_time(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_get_modified_time.\n");
        return NULL;
    }
#if defined(_WIN32) || defined(_WIN64)
    // Convert to wide-character string
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_modified_time.\n");
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        fmt_fprintf(stderr, "Error: GetFileAttributesExW failed in dir_get_modified_time.\n");
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

    if (!lastTime) {
        fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in dir_get_modified_time.\n");
    }
    return lastTime;

#else // POSIX
    struct stat fileInfo;
    if (stat(dirPath, &fileInfo) != 0) {
        fmt_fprintf(stderr, "Error: stat failed in dir_get_modified_time for directory %s.\n", dirPath);
        return NULL;
    }

    time_t lastModifiedTime = fileInfo.st_mtime;
    struct tm* timeinfo = localtime(&lastModifiedTime);
    char* timeString = malloc(sizeof(char) * 256);

    if (timeString == NULL) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in dir_get_modified_time.\n");
        return NULL;
    }

    strftime(timeString, 256, "%Y-%m-%d %H:%M:%S", timeinfo);
    return timeString;
#endif
}

char* dir_get_creation_time(const char* dirPath) {
    if (!dirPath) {
        fmt_fprintf(stderr, "Error: Directory path is NULL in dir_get_creation_time.\n");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_creation_time.\n");
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        fmt_fprintf(stderr, "Error: GetFileAttributesExW failed in dir_get_creation_time.\n");
        free(wPath);
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

    if (!lastTime) {
        fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in dir_get_creation_time.\n");
    }

    return lastTime;
#else 
    fmt_fprintf(stderr, "Error: POSIX systems typically don't have a creation time. Use dir_get_modified_time instead.\n");
    return NULL; 
#endif 
}

char* dir_get_home_directory() {
    char* homeDir = NULL;

#if defined(_WIN32) || defined(_WIN64)
    wchar_t wPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, wPath) == S_OK) {
        homeDir = encoding_wchar_to_utf8(wPath); // Convert from wchar_t to UTF-8
    }
#else
    const char* homeEnv = getenv("HOME");
    if (homeEnv != NULL) {
        homeDir = string_strdup(homeEnv);
    } 
    else {
        struct passwd *pw = getpwuid(getuid());
        if (!pw) {
            fmt_fprintf(stderr, "Error: Unable to retrieve home directory path on POSIX.\n");
            return NULL;
        }
        homeDir = string_strdup(pw->pw_dir);
    }
    if (!homeDir) {
        fmt_fprintf(stderr, "Error: Failed to duplicate home directory string in dir_get_home_directory.\n");
        return NULL;
    }
#endif
    return homeDir; // Caller responsible for freeing this memory
}

DirFileType dir_get_file_type(const char* filePath) {
    if (!filePath) {
        fmt_fprintf(stderr, "Error: File path is NULL in dir_get_file_type.\n");
        return DIR_FILE_TYPE_UNKNOWN;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        fmt_fprintf(stderr, "Error: Failed to convert file path to wide-char in dir_get_file_type.\n");
        return DIR_FILE_TYPE_UNKNOWN;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        fmt_fprintf(stderr, "Error: GetFileAttributesW failed or file does not exist in dir_get_file_type.\n");
        return DIR_FILE_TYPE_UNKNOWN;
    }

    if (attribs & FILE_ATTRIBUTE_DIRECTORY) {
        return DIR_FILE_TYPE_DIRECTORY;
    } 
    else if (attribs & FILE_ATTRIBUTE_REPARSE_POINT) {
        return DIR_FILE_TYPE_SYMLINK;
    } 
    else {
        return DIR_FILE_TYPE_REGULAR;
    }

#else
    struct stat path_stat;
    if (lstat(filePath, &path_stat) != 0) {
        fmt_fprintf(stderr, "Error: lstat failed in dir_get_file_type for file %s.\n", filePath);
        return DIR_FILE_TYPE_UNKNOWN;
    }

    if (S_ISREG(path_stat.st_mode)) {
        return DIR_FILE_TYPE_REGULAR;
    }
    else if (S_ISDIR(path_stat.st_mode)) {
        return DIR_FILE_TYPE_DIRECTORY;
    } 
    else if (S_ISLNK(path_stat.st_mode)) {
        return DIR_FILE_TYPE_SYMLINK;
    } 
    else {
        return DIR_FILE_TYPE_UNKNOWN;
    }
#endif
}

bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv) {
    if (!filePath || !password || !iv) {
        fmt_fprintf(stderr, "Error: Invalid arguments in dir_encrypt_file. filePath, password, or iv is NULL.\n");
        return false;
    }

    FileReader* reader = file_reader_open(filePath, READ_BINARY);
    if (!reader) {
        fmt_fprintf(stderr, "Error: Failed to open file for reading in dir_encrypt_file.\n");
        return false;
    }

    size_t fileLen = file_reader_get_size(reader);
    uint8_t* fileData = (uint8_t*)malloc(fileLen);
    if (!fileData) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in dir_encrypt_file.\n");
        file_reader_close(reader);
        return false;
    }

    if (file_reader_read(fileData, sizeof(uint8_t), fileLen, reader) != fileLen) {
        fmt_fprintf(stderr, "Error: Failed to read file data in dir_encrypt_file.\n");
        free(fileData);
        file_reader_close(reader);
        return false;
    }
    file_reader_close(reader);

    if (strlen(password) > 8) {
        fmt_fprintf(stderr, "Error: Password length exceeds 8 bytes in dir_encrypt_file.\n");
        free(fileData);
        return false;
    }

    uint8_t key[8]; // DES key size
    derive_key_from_password(password, key);

    size_t outLen;
    uint8_t* encryptedData = (uint8_t*)crypto_des_encrypt(fileData, fileLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    free(fileData);

    if (!encryptedData) {
        fmt_fprintf(stderr, "Error: Encryption failed in dir_encrypt_file.\n");
        return false;
    }

    FileWriter* writer = file_writer_open(filePath, WRITE_BINARY);
    if (!writer) {
        fmt_fprintf(stderr, "Error: Failed to open file for writing in dir_encrypt_file.\n");
        free(encryptedData);
        return false;
    }

    if (file_writer_write(encryptedData, sizeof(uint8_t), outLen, writer) != outLen) {
        fmt_fprintf(stderr, "Error: Failed to write encrypted data in dir_encrypt_file.\n");
        free(encryptedData);
        file_writer_close(writer);
        return false;
    }
    free(encryptedData);
    file_writer_close(writer);
    return true;
}

bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv) {
    if (!filePath || !password || !iv) {
        fmt_fprintf(stderr, "Error: Invalid arguments in dir_decrypt_file. filePath, password, or iv is NULL.\n");
        return false;
    }

    FileReader* reader = file_reader_open(filePath, READ_BINARY);
    if (!reader) {
        fmt_fprintf(stderr, "Error: Failed to open file for reading in dir_decrypt_file.\n");
        return false;
    }

    size_t fileLen = file_reader_get_size(reader);
    uint8_t* fileData = (uint8_t*)malloc(fileLen);
    if (!fileData) {
        fmt_fprintf(stderr, "Error: Memory allocation failed in dir_decrypt_file.\n");
        file_reader_close(reader);
        return false;
    }

    if (file_reader_read(fileData, sizeof(uint8_t), fileLen, reader) != fileLen) {
        fmt_fprintf(stderr, "Error: Failed to read file data in dir_decrypt_file.\n");
        free(fileData);
        file_reader_close(reader);
        return false;
    }
    file_reader_close(reader);

    if (strlen(password) > 8) {
        fmt_fprintf(stderr, "Error: Password length exceeds 8 bytes in dir_decrypt_file.\n");
        free(fileData);
        return false;
    }

    uint8_t key[8]; // DES key size
    derive_key_from_password(password, key);

    size_t outLen;
    uint8_t* decryptedData = (uint8_t*)crypto_des_decrypt(fileData, fileLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    free(fileData);

    if (!decryptedData) {
        fmt_fprintf(stderr, "Error: Decryption failed in dir_decrypt_file.\n");
        return false;
    }

    FileWriter* writer = file_writer_open(filePath, WRITE_BINARY);
    if (!writer) {
        fmt_fprintf(stderr, "Error: Failed to open file for writing in dir_decrypt_file.\n");
        free(decryptedData);
        return false;
    }

    if (file_writer_write(decryptedData, sizeof(uint8_t), outLen, writer) != outLen) {
        fmt_fprintf(stderr, "Error: Failed to write decrypted data in dir_decrypt_file.\n");
        free(decryptedData);
        file_writer_close(writer);
        return false;
    }
    free(decryptedData);
    file_writer_close(writer);
    return true;
}

bool dir_get_file_owner(const char* filePath, char* ownerBuffer, size_t bufferSize) {
    if (!filePath || !ownerBuffer || bufferSize == 0) {
        fmt_fprintf(stderr, "Error: Invalid arguments in dir_get_file_owner. filePath, ownerBuffer is NULL, or bufferSize is 0.\n");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFilePath = encoding_utf8_to_wchar(filePath);
    if (wFilePath == NULL) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_file_owner.\n");
        return false;
    }

    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION;
    PSECURITY_DESCRIPTOR pSD = NULL;
    PSID ownerSid = NULL;

    HANDLE hFile = CreateFileW(wFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: Unable to open file handle in dir_get_file_owner for file %s.\n", filePath);
        free(wFilePath);
        return false;
    }

    if (GetSecurityInfo(hFile, SE_FILE_OBJECT, si, &ownerSid, NULL, NULL, NULL, &pSD) != ERROR_SUCCESS) {
        fmt_fprintf(stderr, "Error: GetSecurityInfo failed in dir_get_file_owner.\n");
        CloseHandle(hFile);
        free(wFilePath);
        return false;
    }

    CloseHandle(hFile);

    wchar_t name[512], domain[512];
    DWORD nameSize = 512, domainSize = 512;
    SID_NAME_USE sidUse;

    if (!LookupAccountSidW(NULL, ownerSid, name, &nameSize, domain, &domainSize, &sidUse)) {
        fmt_fprintf(stderr, "Error: LookupAccountSidW failed in dir_get_file_owner.\n");
        free(wFilePath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    char* utf8OwnerName = encoding_wchar_to_utf8(name);
    if (utf8OwnerName == NULL) {
        fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in dir_get_file_owner.\n");
        free(wFilePath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    strncpy(ownerBuffer, utf8OwnerName, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    free(wFilePath);
    free(utf8OwnerName);
    if (pSD) {
        LocalFree(pSD);
    }
    return true;
#else
    struct stat fileInfo;
    if (stat(filePath, &fileInfo) != 0) {
        fmt_fprintf(stderr, "Error: stat failed in dir_get_file_owner for file %s.\n", filePath);
        return false;
    }

    struct passwd *pwd = getpwuid(fileInfo.st_uid);
    if (pwd == NULL) {
        fmt_fprintf(stderr, "Error: getpwuid failed in dir_get_file_owner for UID %d.\n", fileInfo.st_uid);
        return false;
    }

    strncpy(ownerBuffer, pwd->pw_name, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';
    return true;
#endif
}

bool dir_get_directory_owner(const char* dirPath, char* ownerBuffer, size_t bufferSize) {
    if (!dirPath || !ownerBuffer || bufferSize == 0) {
        fmt_fprintf(stderr, "Error: Invalid arguments in dir_get_directory_owner. dirPath, ownerBuffer is NULL or bufferSize is 0.\n");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirPath = encoding_utf8_to_wchar(dirPath);
    if (wDirPath == NULL) {
        fmt_fprintf(stderr, "Error: Conversion to wide-char failed in dir_get_directory_owner.\n");
        return false;
    }

    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION;
    PSECURITY_DESCRIPTOR pSD = NULL;
    PSID ownerSid = NULL;

    HANDLE hDir = CreateFileW(wDirPath, READ_CONTROL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: Unable to open directory handle in dir_get_directory_owner.\n");
        free(wDirPath);
        return false;
    }

    if (GetSecurityInfo(hDir, SE_FILE_OBJECT, si, &ownerSid, NULL, NULL, NULL, &pSD) != ERROR_SUCCESS) {
        fmt_fprintf(stderr, "Error: GetSecurityInfo failed in dir_get_directory_owner.\n");
        CloseHandle(hDir);
        free(wDirPath);
        return false;
    }

    CloseHandle(hDir);

    wchar_t name[512], domain[512];
    DWORD nameSize = 512, domainSize = 512;
    SID_NAME_USE sidUse;

    if (!LookupAccountSidW(NULL, ownerSid, name, &nameSize, domain, &domainSize, &sidUse)) {
        fmt_fprintf(stderr, "Error: LookupAccountSidW failed in dir_get_directory_owner.\n");
        free(wDirPath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    char* utf8OwnerName = encoding_wchar_to_utf8(name);
    if (utf8OwnerName == NULL) {
        fmt_fprintf(stderr, "Error: Conversion from wide-char to UTF-8 failed in dir_get_directory_owner.\n");
        free(wDirPath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    strncpy(ownerBuffer, utf8OwnerName, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    free(wDirPath);
    free(utf8OwnerName);
    if (pSD) {
        LocalFree(pSD);
    }
    return true;
#else
    struct stat dirInfo;
    if (stat(dirPath, &dirInfo) != 0) {
        fmt_fprintf(stderr, "Error: stat failed in dir_get_directory_owner for path %s.\n", dirPath);
        return false;
    }

    struct passwd *pwd = getpwuid(dirInfo.st_uid);
    if (pwd == NULL) {
        fmt_fprintf(stderr, "Error: getpwuid failed in dir_get_directory_owner for UID %d.\n", dirInfo.st_uid);
        return false;
    }

    strncpy(ownerBuffer, pwd->pw_name, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';
    return true;
#endif
}

bool dir_search(const char* dirPath, const char* pattern, DirCompareFunc callback, void* userData) {
    if (!dirPath || !pattern || !callback) {
        fmt_fprintf(stderr, "Error: Invalid arguments in dir_search. dirPath, pattern, or callback is NULL.\n");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirPath = encoding_utf8_to_wchar(dirPath);
    wchar_t* wPattern = encoding_utf8_to_wchar(pattern);
    if (wDirPath == NULL || wPattern == NULL) {
        fmt_fprintf(stderr, "Error: Failed to convert UTF-8 strings to wide-char in dir_search.\n");
        free(wDirPath);
        free(wPattern);
        return false;
    }

    // Allocate memory for the combined path and pattern
    size_t combinedSize = wcslen(wDirPath) + wcslen(wPattern) + 1; // +1 for null terminator
    wchar_t* wSearchPath = (wchar_t*)malloc(combinedSize * sizeof(wchar_t));
    if (wSearchPath == NULL) {
        fmt_fprintf(stderr, "Error: Failed to allocate memory for search path in dir_search.\n");
        free(wDirPath);
        free(wPattern);
        return false;
    }

    // Combine dirPath and pattern
    wcscpy_s(wSearchPath, combinedSize, wDirPath);
    wcscat_s(wSearchPath, combinedSize, wPattern);

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSearchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fmt_fprintf(stderr, "Error: FindFirstFileW failed in dir_search.\n");
        free(wDirPath);
        free(wPattern);
        free(wSearchPath);
        return false;
    }

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // Skip directories if you only want files
            continue;
        }

        char* utf8FileName = encoding_wchar_to_utf8(findFileData.cFileName);
        if (!callback(utf8FileName, userData) || !utf8FileName) {
            free(utf8FileName);
            break;
        }
        free(utf8FileName);
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
    free(wDirPath);
    free(wPattern);
    free(wSearchPath);
    return true;
#else
    DIR* dir = opendir(dirPath);
    if (!dir) {
        fmt_fprintf(stderr, "Error: Failed to open directory '%s' in dir_search.\n", dirPath);
        return false;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name, 0) == 0) {
            if (!callback(entry->d_name, userData)) {
                break;
            }
        }
    }

    closedir(dir);
    return true;
#endif
    
}