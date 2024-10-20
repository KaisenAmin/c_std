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
#include "../string/std_string.h"
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
        DIR_LOG("[get_last_dir_name_win] Error: Path is NULL.");
        return NULL;
    }

    wchar_t* lastSlash = wcsrchr(wpath, L'\\');
    if (lastSlash == NULL) {
        lastSlash = wcsrchr(wpath, L'/');
    }

    if (lastSlash != NULL && lastSlash != wpath) {
        char* dirName = encoding_wchar_to_utf8(lastSlash + 1);
        if (!dirName) {
            DIR_LOG("[get_last_dir_name_win] Error: Failed to convert directory name to UTF-8.");
        } 
        else {
            DIR_LOG("[get_last_dir_name_win] Successfully retrieved directory name: %s.", dirName);
        }
        return dirName;
    }

    DIR_LOG("[get_last_dir_name_win] Error: Last directory name not found in path.");
    return NULL;
}

// Helper function for Windows to get full path
static char* get_full_path_win(const char* path) {
    if (!path) {
        DIR_LOG("[get_full_path_win] Error: Path is NULL.");
        return NULL;
    }

    wchar_t* wpath = encoding_utf8_to_wchar(path);
    if (!wpath) {
        DIR_LOG("[get_full_path_win] Error: Conversion to wide-char failed.");
        return NULL;
    }

    wchar_t wfullpath[_MAX_PATH];
    if (_wfullpath(wfullpath, wpath, _MAX_PATH) != NULL) {
        char* fullpath = encoding_wchar_to_utf8(wfullpath);
        free(wpath);
        if (!fullpath) {
            DIR_LOG("[get_full_path_win] Error: Conversion from wide-char to UTF-8 failed.");
        } 
        else {
            DIR_LOG("[get_full_path_win] Successfully retrieved full path: %s.", fullpath);
        }
        return fullpath;
    }

    DIR_LOG("[get_full_path_win] Error: Unable to get full path for %s.", path);
    free(wpath);

    return NULL;
}

static int remove_directory_recursive_win(const wchar_t* dir) {
    if (!dir) {
        DIR_LOG("[remove_directory_recursive_win] Error: Directory is NULL.");
        return -1;
    }

    int len = wcslen(dir) + 2;  // Extra space for double null termination
    wchar_t* tempDir = (wchar_t*) malloc(len * sizeof(wchar_t));
    if (!tempDir) {
        DIR_LOG("[remove_directory_recursive_win] Error: Memory allocation failed.");
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
        L"" 
    };

    int ret = SHFileOperationW(&fileOp);
    free(tempDir);

    if (ret == 0) {
        DIR_LOG("[remove_directory_recursive_win] Successfully removed directory: %ls", dir);
    } 
    else {
        DIR_LOG("[remove_directory_recursive_win] Error: Failed to remove directory: %ls", dir);
    }

    return ret; // 0 on success, non-zero on failure
}

static long long dir_get_size_win(const wchar_t* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_get_size_win] Error: Directory path is NULL.");
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
        DIR_LOG("[dir_get_size_win] Error: Failed to open directory %ls.", dirPath);
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
                } 
                else {
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

    DIR_LOG("[dir_get_size_win] Total size of directory %ls: %lld bytes.", dirPath, size);
    return size;
}

static long long get_file_size_win(const wchar_t* filePath) {
    if (!filePath) {
        DIR_LOG("[get_file_size_win] Error: File path is NULL.");
        return -1;
    }

    LARGE_INTEGER size;
    HANDLE hFile = CreateFileW(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        DIR_LOG("[get_file_size_win] Error: Unable to open file %ls.", filePath);
        return -1;
    }
    if (!GetFileSizeEx(hFile, &size)) {
        DIR_LOG("[get_file_size_win] Error: Unable to get file size for %ls.", filePath);
        CloseHandle(hFile);
        return -1;
    }

    DIR_LOG("[get_file_size_win] File %ls size: %lld bytes.", filePath, size.QuadPart);
    CloseHandle(hFile);

    return size.QuadPart;
}

static void dir_list_contents_win(const wchar_t* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath || !resultVector) {
        DIR_LOG("[dir_list_contents_win] Error: Directory path or result vector is NULL.");
        return;
    }

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    wchar_t searchPath[MAX_PATH];

    // Create a search pattern
    wsprintfW(searchPath, L"%ls\\*", dirPath);
    hFind = FindFirstFileW(searchPath, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_list_contents_win] Error: Failed to open directory %ls.", dirPath);
        return;
    }

    do {
        if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
            bool isDirectory = (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

            if ((option == DIR_LIST_FILES && !isDirectory) || 
                (option == DIR_LIST_DIRECTORIES && isDirectory) || 
                (option == DIR_LIST_ALL)) {
                
                char* utf8FileName = encoding_wchar_to_utf8(findFileData.cFileName);
                if (!utf8FileName) {
                    DIR_LOG("[dir_list_contents_win] Error: Failed to convert filename to UTF-8.");
                    continue;
                }
                vector_push_back(resultVector, &utf8FileName);
                DIR_LOG("[dir_list_contents_win] Added entry: %s.", utf8FileName);
            }
        }
    } while (FindNextFileW(hFind, &findFileData) != 0);

    FindClose(hFind);
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
        DIR_LOG("[get_full_path_posix] Error: Path is NULL.");
        return NULL;
    }

    char fullpath[PATH_MAX];
    if (realpath(path, fullpath) == NULL) {
        DIR_LOG("[get_full_path_posix] Error: Failed to resolve realpath for %s.", path);
        return NULL;
    }

    char* resolvedPath = strdup(fullpath);
    if (!resolvedPath) {
        DIR_LOG("[get_full_path_posix] Error: Memory allocation failed for path %s.", fullpath);
    } 
    else {
        DIR_LOG("[get_full_path_posix] Resolved full path: %s.", resolvedPath);
    }

    return resolvedPath;
}

static int remove_directory_recursive_posix(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[remove_directory_recursive_posix] Error: Directory path is NULL.");
        return -1;
    }

    DIR* d = opendir(dirPath);
    if (!d) {
        DIR_LOG("[remove_directory_recursive_posix] Error: Failed to open directory %s.", dirPath);
        return -1;
    }

    struct dirent* entry;
    int ret = 0;

    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char fullPath[PATH_MAX];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (entry->d_type == DT_DIR) {
                DIR_LOG("[remove_directory_recursive_posix] Recursively removing directory: %s", fullPath);
                ret = remove_directory_recursive_posix(fullPath);
            } 
            else {
                DIR_LOG("[remove_directory_recursive_posix] Removing file: %s", fullPath);
                ret = remove(fullPath);
            }

            if (ret != 0) {
                DIR_LOG("[remove_directory_recursive_posix] Error: Failed to remove %s.", fullPath);
                break; // Error occurred
            }
        }
    }

    closedir(d);
    if (ret == 0 && rmdir(dirPath) != 0) {
        DIR_LOG("[remove_directory_recursive_posix] Error: Failed to remove directory %s.", dirPath);
        return -1;
    }

    DIR_LOG("[remove_directory_recursive_posix] Successfully removed directory %s.", dirPath);
    return ret;
}


static long long dir_get_size_posix(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_get_size_posix] Error: Directory path is NULL.");
        return -1;
    }

    DIR* dir = opendir(dirPath);
    if (!dir) {
        DIR_LOG("[dir_get_size_posix] Error: Failed to open directory %s.", dirPath);
        return -1;
    }

    struct dirent* entry;
    struct stat statbuf;
    long long size = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            char fullPath[1024];
            snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

            if (stat(fullPath, &statbuf) == -1) {
                DIR_LOG("[dir_get_size_posix] Error: Failed to get stats for file %s.", fullPath);
                continue;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                // Recursively calculate the size of directories
                size += dir_get_size_posix(fullPath);
                DIR_LOG("[dir_get_size_posix] Accumulated size for directory %s: %lld bytes.", fullPath, size);
            } 
            else {
                // Accumulate the size of each file
                size += statbuf.st_size;
                DIR_LOG("[dir_get_size_posix] File %s size: %lld bytes.", fullPath, statbuf.st_size);
            }
        }
    }

    closedir(dir);
    DIR_LOG("[dir_get_size_posix] Total size for directory %s: %lld bytes.", dirPath, size);

    return size;
}

static long long get_file_size_posix(const char* filePath) {
    if (!filePath) {
        DIR_LOG("[get_file_size_posix] Error: File path is NULL.");
        return -1;
    }

    struct stat statbuf;
    if (stat(filePath, &statbuf) != 0) {
        DIR_LOG("[get_file_size_posix] Error: Failed to get file stats for %s.", filePath);
        return -1;
    }

    DIR_LOG("[get_file_size_posix] File %s size: %lld bytes.", filePath, statbuf.st_size);
    return statbuf.st_size;
}

void dir_list_contents_posix(const char* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath || !resultVector) {
        DIR_LOG("[dir_list_contents_posix] Error: Directory path or result vector is NULL.");
        return;
    }

    DIR* dir = opendir(dirPath);
    if (!dir) {
        DIR_LOG("[dir_list_contents_posix] Error: Failed to open directory %s.", dirPath);
        return;
    }

    struct dirent* entry;
    struct stat statbuf;
    char fullPath[1024];

    while ((entry = readdir(dir)) != NULL) {
        snprintf(fullPath, sizeof(fullPath), "%s/%s", dirPath, entry->d_name);

        if (stat(fullPath, &statbuf) == -1) {
            DIR_LOG("[dir_list_contents_posix] Error: Failed to get stats for file %s.", fullPath);
            continue;
        }

        if (option == LIST_FILES && !S_ISDIR(statbuf.st_mode)) {
            DIR_LOG("[dir_list_contents_posix] Listing file: %s", entry->d_name);
        } 
        else if (option == LIST_DIRECTORIES && S_ISDIR(statbuf.st_mode)) {
            DIR_LOG("[dir_list_contents_posix] Listing directory: %s", entry->d_name);
        } 
        else if (option == LIST_ALL) {
            DIR_LOG("[dir_list_contents_posix] Listing all: %s", entry->d_name);
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
#endif 

static void derive_key_from_password(const char* password, uint8_t* key) {
    if (!password || !key) {
        DIR_LOG("[derive_key_from_password] Error: Password or key buffer is NULL.");
        return;
    }

    size_t pass_len = strlen(password);
    size_t key_len = 8;  // DES key size is 8 bytes

    if (pass_len > key_len) {
        // Truncate the password if it's longer than the key size
        memcpy(key, password, key_len);
        DIR_LOG("[derive_key_from_password] Password truncated to fit DES key size.");
    } 
    else {
        // Pad the key with zeros if the password is shorter than the key size
        memcpy(key, password, pass_len);
        memset(key + pass_len, 0, key_len - pass_len);
        DIR_LOG("[derive_key_from_password] Password padded with zeros to fit DES key size.");
    }
}

/**
 * @brief Creates a directory at the specified path.
 *
 * @param dirpath The path of the directory to create.
 * @return `true` if the directory was successfully created, `false` otherwise.
 */
bool dir_make_directory(const char* dirpath) {
    if (!dirpath) {
        DIR_LOG("[dir_make_directory] Error: Directory path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* dirpath_wchar = encoding_utf8_to_wchar(dirpath);

    if (!dirpath_wchar) {
        DIR_LOG("[dir_make_directory] Error: Conversion to wide-char failed for directory %s.", dirpath);
        return false;
    }
    if (!CreateDirectoryW(dirpath_wchar, NULL)) {
        DIR_LOG("[dir_make_directory] Error: Unable to create directory %s.", dirpath);
        free(dirpath_wchar);
        return false;
    }

    DIR_LOG("[dir_make_directory] Directory created successfully: %s", dirpath);
    free(dirpath_wchar);

    return true;

#else
    mode_t mode = 0755; // default permissions
    if (mkdir(dirpath, mode) != 0) {
        DIR_LOG("[dir_make_directory] Error: Unable to create directory %s.", dirpath);
        return false;
    }

    DIR_LOG("[dir_make_directory] Directory created successfully: %s", dirpath);
    return true;
#endif
}

/**
 * @brief Gets the name of the directory from a specified path.
 *
 * @param dirpath The path of the directory.
 * @return A string representing the directory name, or `NULL` on error.
 */
char* dir_dir_name(const char* dirpath) {
    if (!dirpath) {
        DIR_LOG("[dir_dir_name] Error: Directory path is NULL.");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wpath = encoding_utf8_to_wchar(dirpath);
    if (!wpath) {
        DIR_LOG("[dir_dir_name] Error: Conversion to wide-char failed for directory %s.", dirpath);
        return NULL;
    }

    char* name = NULL;
    if (wcscmp(wpath, L".") == 0) {
        wchar_t cwd[1024];
        if (_wgetcwd(cwd, sizeof(cwd) / sizeof(wchar_t)) != NULL) {
            name = get_last_dir_name_win(cwd);
        }
    } 
    else {
        name = get_last_dir_name_win(wpath);
    }

    free(wpath);
    if (name) {
        DIR_LOG("[dir_dir_name] Directory name for %s: %s", dirpath, name);
    } 
    else {
        DIR_LOG("[dir_dir_name] Error: Failed to retrieve directory name for %s.", dirpath);
        return string_strdup("");
    }

    return name;

#else
    if (strcmp(dirpath, ".") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            const char* lastSlash = strrchr(cwd, '/');
            char* name = lastSlash ? string_strdup(lastSlash + 1) : string_strdup(cwd);
            DIR_LOG("[dir_dir_name] Directory name for current directory: %s", name);
            return name;
        }
        DIR_LOG("[dir_dir_name] Error: Failed to retrieve current directory name.");
        return string_strdup("");
    } 
    else {
        const char* lastSlash = strrchr(dirpath, '/');
        char* name = lastSlash ? string_strdup(lastSlash + 1) : string_strdup(dirpath);
        DIR_LOG("[dir_dir_name] Directory name for %s: %s", dirpath, name);

        return name;
    }
#endif
}

/**
 * @brief Gets the current working directory path.
 *
 * @return A string representing the current working directory path, or `NULL` on error.
 */
char* dir_current_path(void) {
    char* path = NULL;

#if defined(_WIN32) || defined(_WIN64)
    wchar_t wpath[1024];
    if (_wgetcwd(wpath, sizeof(wpath) / sizeof(wchar_t)) == NULL) {
        DIR_LOG("[dir_current_path] Error: Unable to get current working directory on Windows.");
        return NULL;
    }

    path = encoding_wchar_to_utf8(wpath);
    if (!path) {
        DIR_LOG("[dir_current_path] Error: Conversion from wide-char to UTF-8 failed.");
    } 
    else {
        DIR_LOG("[dir_current_path] Current working directory: %s", path);
    }

#else
    char buffer[1024];
    if (getcwd(buffer, sizeof(buffer)) == NULL) {
        DIR_LOG("[dir_current_path] Error: Unable to get current working directory on POSIX.");
        return NULL;
    }

    path = string_strdup(buffer);
    if (!path) {
        DIR_LOG("[dir_current_path] Error: Memory allocation failed.");
    } 
    else {
        DIR_LOG("[dir_current_path] Current working directory: %s", path);
    }
#endif

    return path;
}

/**
 * @brief Counts the number of items in a directory.
 *
 * @param dirpath The path of the directory to count items in.
 * @return The number of items in the directory, or `-1` on error.
 */
int dir_count(const char* dirpath) {
    if (!dirpath) {
        DIR_LOG("[dir_count] Error: Directory path is NULL.");
        return -1;
    }
    int count = 0;

#if defined(_WIN32) || defined(_WIN64)

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    wchar_t wSearchPath[1024];
    MultiByteToWideChar(CP_UTF8, 0, dirpath, -1, wSearchPath, 1024);
    wcscat(wSearchPath, L"\\*");  // Append wildcard for search

    hFind = FindFirstFileW(wSearchPath, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                count++;
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);
        FindClose(hFind);
    } 
    else {
        DIR_LOG("[dir_count] Error: Failed to find files in directory %s.", dirpath);
        return -1;
    }

    DIR_LOG("[dir_count] Number of items in directory %s: %d", dirpath, count);

#else
    DIR *dir = opendir(dirpath);
    if (!dir) {
        DIR_LOG("[dir_count] Error: Failed to open directory %s.", dirpath);
        return -1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            count++;
        }
    }
    closedir(dir);

    DIR_LOG("[dir_count] Number of items in directory %s: %d", dirpath, count);
#endif

    return count;
}


/**
 * @brief Converts a relative file path to an absolute file path.
 *
 * @param relative_path The relative file path to convert.
 * @return A string representing the absolute file path, or `NULL` on error.
 */
char* dir_absolute_file_path(const char* relative_path) {
    if (!relative_path) {
        DIR_LOG("[dir_absolute_file_path] Error: Relative path is NULL.");
        return NULL;
    }

    char* absolutePath = NULL;

#if defined(_WIN32) || defined(_WIN64)
    DIR_LOG("[dir_absolute_file_path] Getting absolute path for %s on Windows.", relative_path);
    // For Windows
    absolutePath = get_full_path_win(relative_path);
#else
    DIR_LOG("[dir_absolute_file_path] Getting absolute path for %s on POSIX.", relative_path);
    // For POSIX systems
    absolutePath = get_full_path_posix(relative_path);
#endif

    if (absolutePath) {
        DIR_LOG("[dir_absolute_file_path] Absolute path for %s: %s", relative_path, absolutePath);
    } 
    else {
        DIR_LOG("[dir_absolute_file_path] Error: Failed to get absolute path for %s.", relative_path);
    }

    return absolutePath;
}

/**
 * @brief Changes the current working directory.
 *
 * @param dirName The name of the directory to change to.
 * @return `true` if the directory was successfully changed, `false` otherwise.
 */
bool dir_cd(const char* dirName) {
    if (!dirName) {
        DIR_LOG("[dir_cd] Error: Directory name is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    DIR_LOG("[dir_cd] Changing directory to %s on Windows.", dirName);
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);

    if (!wDirName) {
        DIR_LOG("[dir_cd] Error: Conversion to wide-char failed for directory %s.", dirName);
        return false;
    }
    if (change_directory(wDirName) != 0) {
        DIR_LOG("[dir_cd] Error: Change directory failed for directory %s.", dirName);
        free(wDirName);
        return false;
    }

    DIR_LOG("[dir_cd] Directory changed successfully to %s.", dirName);
    free(wDirName);

    return true;

#else

    DIR_LOG("[dir_cd] Changing directory to %s on POSIX.", dirName);

    if (change_directory(dirName) != 0) {
        DIR_LOG("[dir_cd] Error: Change directory failed for directory %s.", dirName);
        return false;
    }

    DIR_LOG("[dir_cd] Directory changed successfully to %s.", dirName);
    return true;
#endif
}

/**
 * @brief Changes the current working directory to the parent directory.
 *
 * @return `true` if the directory was successfully changed, `false` otherwise.
 */
bool dir_cd_up() {
    DIR_LOG("[dir_cd_up] Changing directory to parent.");
    return dir_cd("..");
}

/**
 * @brief Removes an empty directory.
 *
 * @param dirName The name of the directory to remove.
 * @return `true` if the directory was successfully removed, `false` otherwise.
 */
bool dir_remove_directory(const char* dirName) {
    if (!dirName) {
        DIR_LOG("[dir_remove_directory] Error: Directory name is NULL.");
        return false;
    }
    if (!dir_is_empty(dirName)) {
        DIR_LOG("[dir_remove_directory] Error: Directory %s is not empty.", dirName);
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (!wDirName) {
        DIR_LOG("[dir_remove_directory] Error: Conversion to wide-char failed for directory %s.", dirName);
        return false;
    }

    DIR_LOG("[dir_remove_directory] Removing directory: %s", dirName);
    bool result = remove_directory(wDirName) == 0;
    free(wDirName);

    if (!result) {
        DIR_LOG("[dir_remove_directory] Error: Failed to remove directory %s.", dirName);
    } 
    else {
        DIR_LOG("[dir_remove_directory] Directory removed successfully: %s", dirName);
    }

    return result;

#else
    DIR_LOG("[dir_remove_directory] Removing directory: %s", dirName);

    if (remove_directory(dirName) != 0) {
        DIR_LOG("[dir_remove_directory] Error: rmdir failed for directory %s.", dirName);
        return false;
    }

    DIR_LOG("[dir_remove_directory] Directory removed successfully: %s", dirName);
    return true;
#endif
}

/**
 * @brief Checks if a directory is empty.
 *
 * @param dirName The name of the directory to check.
 * @return `true` if the directory is empty, `false` otherwise.
 */
bool dir_is_empty(const char* dirName) {
    if (!dirName) {
        DIR_LOG("[dir_is_empty] Error: Directory name is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    DIR_LOG("[dir_is_empty] Checking if directory %s is empty.", dirName);

    if (PathIsDirectoryEmptyA(dirName) != TRUE) {
        DIR_LOG("[dir_is_empty] Error: Directory %s is not empty.", dirName);
        return false;
    }

    DIR_LOG("[dir_is_empty] Directory %s is empty.", dirName);
    return true;

#else
    DIR_LOG("[dir_is_empty] Checking if directory %s is empty.", dirName);

    DIR *d = opendir(dirName);
    if (!d) {
        DIR_LOG("[dir_is_empty] Error: Failed to open directory %s for checking contents.", dirName);
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
    DIR_LOG("[dir_is_empty] Directory %s is %s.", dirName, isEmpty ? "empty" : "not empty");
    return isEmpty;
#endif
}


/**
 * @brief Recursively removes a directory and all its contents.
 *
 * @param dirPath The path of the directory to remove.
 * @return `true` if the directory was successfully removed, `false` otherwise.
 */
bool dir_remove_directory_recursive(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_remove_directory_recursive] Error: Directory path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t wPath[MAX_PATH];

    if (MultiByteToWideChar(CP_UTF8, 0, dirPath, -1, wPath, MAX_PATH) == 0) {
        DIR_LOG("[dir_remove_directory_recursive] Error: Conversion to wide-char failed for directory %s.", dirPath);
        return false;
    }

    DIR_LOG("[dir_remove_directory_recursive] Removing directory recursively: %s", dirPath);
    if (remove_directory_recursive_win(wPath) != 0) {
        DIR_LOG("[dir_remove_directory_recursive] Error: Recursive directory removal failed for directory %s.", dirPath);
        return false;
    }

    DIR_LOG("[dir_remove_directory_recursive] Directory removed successfully: %s", dirPath);
    return true;

#else
    DIR_LOG("[dir_remove_directory_recursive] Removing directory recursively: %s", dirPath);

    if (remove_directory_recursive_posix(dirPath) != 0) {
        DIR_LOG("[dir_remove_directory_recursive] Error: Recursive directory removal failed for directory %s.", dirPath);
        return false;
    }

    DIR_LOG("[dir_remove_directory_recursive] Directory removed successfully: %s", dirPath);
    return true;
#endif
}

/**
 * @brief Renames a file or directory.
 *
 * @param oldName The current name of the file or directory.
 * @param newName The new name for the file or directory.
 * @return `true` if the renaming was successful, `false` otherwise.
 */
bool dir_rename(const char* oldName, const char* newName) {
    if (!oldName || !newName) {
        DIR_LOG("[dir_rename] Error: Old name or new name is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* oldName_wchar = encoding_utf8_to_wchar(oldName);
    wchar_t* newName_wchar = encoding_utf8_to_wchar(newName);

    if (!oldName_wchar || !newName_wchar) {
        DIR_LOG("[dir_rename] Error: Conversion to wide-char failed.");
        free(oldName_wchar);
        free(newName_wchar);
        return false;
    }

    DIR_LOG("[dir_rename] Renaming %s to %s", oldName, newName);

    if (!MoveFileW(oldName_wchar, newName_wchar)) {
        DIR_LOG("[dir_rename] Error: MoveFileW failed to rename from %s to %s.", oldName, newName);
        free(oldName_wchar);
        free(newName_wchar);
        return false;
    }

    DIR_LOG("[dir_rename] Successfully renamed %s to %s", oldName, newName);
    free(oldName_wchar);
    free(newName_wchar);

    return true;

#else
    DIR_LOG("[dir_rename] Renaming %s to %s", oldName, newName);

    if (rename(oldName, newName) != 0) {
        DIR_LOG("[dir_rename] Error: rename failed to rename from %s to %s.", oldName, newName);
        return false;
    }

    DIR_LOG("[dir_rename] Successfully renamed %s to %s", oldName, newName);
    return true;
#endif
}


/**
 * @brief Checks if a directory exists at the specified path.
 *
 * @param dirPath The path of the directory to check.
 * @return `true` if the directory exists, `false` otherwise.
 */
bool dir_is_directory_exists(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_is_directory_exists] Error: Directory path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (wPath == NULL) {
        DIR_LOG("[dir_is_directory_exists] Error: Failed in converting UTF-8 to wchar.");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        DIR_LOG("[dir_is_directory_exists] Error: GetFileAttributesW failed or directory does not exist.");
        return false;
    }

    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    DIR_LOG("[dir_is_directory_exists] Directory exists check for %s: %s", dirPath, result ? "true" : "false");
    return result;

#else
    struct stat statbuf;
    if (stat(dirPath, &statbuf) != 0) {
        DIR_LOG("[dir_is_directory_exists] Error: stat failed or directory does not exist for %s.", dirPath);
        return false;
    }

    bool result = S_ISDIR(statbuf.st_mode);
    DIR_LOG("[dir_is_directory_exists] Directory exists check for %s: %s", dirPath, result ? "true" : "false");
    return result;
#endif
}

/**
 * @brief Checks if a file exists at the specified path.
 *
 * @param filePath The path of the file to check.
 * @return `true` if the file exists, `false` otherwise.
 */
bool dir_is_file_exists(const char* filePath) {
    if (!filePath) {
        DIR_LOG("[dir_is_file_exists] Error: File path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (wPath == NULL) {
        DIR_LOG("[dir_is_file_exists] Error: Failed in converting UTF-8 to wchar.");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    bool result = (attribs != INVALID_FILE_ATTRIBUTES);
    DIR_LOG("[dir_is_file_exists] File exists check for %s: %s", filePath, result ? "true" : "false");
    return result;

#else
    struct stat statbuf;
    bool result = (stat(filePath, &statbuf) == 0);
    DIR_LOG("[dir_is_file_exists] File exists check for %s: %s", filePath, result ? "true" : "false");
    return result;
#endif
}

/**
 * @brief Copies a file from a source path to a destination path.
 *
 * @param srcPath The source file path.
 * @param destPath The destination file path.
 * @return `true` if the file was successfully copied, `false` otherwise.
 */
bool dir_copy_file(const char* srcPath, const char* destPath) {
    if (!srcPath || !destPath) {
        DIR_LOG("[dir_copy_file] Error: Source or destination path is NULL.");
        return false;
    }

    DIR_LOG("[dir_copy_file] Copying file from %s to %s", srcPath, destPath);

    // On Windows, use wide character APIs for Unicode support
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcPath = encoding_utf8_to_wchar(srcPath);
    wchar_t* wDestPath = encoding_utf8_to_wchar(destPath);

    if (!wSrcPath || !wDestPath) {
        DIR_LOG("[dir_copy_file] Error: Conversion to wide-char failed.");
        free(wSrcPath);
        free(wDestPath);
        return false;
    }

    bool result = CopyFileW(wSrcPath, wDestPath, FALSE) != 0;
    if (!result) {
        DIR_LOG("[dir_copy_file] Error: CopyFileW failed to copy from %s to %s.", srcPath, destPath);
    } 
    else {
        DIR_LOG("[dir_copy_file] File copied successfully from %s to %s", srcPath, destPath);
    }

    free(wSrcPath);
    free(wDestPath);

    return result;

#else
    // POSIX implementation
    FileReader* srcFile = file_reader_open(srcPath, READ_BINARY);
    if (!srcFile) {
        DIR_LOG("[dir_copy_file] Error: Unable to open source file %s for reading.", srcPath);
        return false;
    }

    FileWriter* destFile = file_writer_open(destPath, WRITE_BINARY);
    if (!destFile) {
        file_reader_close(srcFile);
        DIR_LOG("[dir_copy_file] Error: Unable to open destination file %s for writing.", destPath);
        return false;
    }

    char buffer[4096];
    size_t bytesRead;
    bool success = true;

    while (!file_reader_eof(srcFile)) {
        bytesRead = file_reader_read(buffer, 1, sizeof(buffer), srcFile);
        if (bytesRead == 0) {
            break; 
        }

        if (file_writer_write(buffer, 1, bytesRead, destFile) != bytesRead) {
            DIR_LOG("[dir_copy_file] Error: Failed to write data to destination file %s.", destPath);
            success = false;
            break;
        }
    }

    file_reader_close(srcFile);
    file_writer_close(destFile);

    if (success) {
        DIR_LOG("[dir_copy_file] File copied successfully from %s to %s", srcPath, destPath);
    }

    return success;
#endif
}

/**
 * @brief Copies a directory and its contents to a new location.
 *
 * @param srcDir The source directory path.
 * @param destDir The destination directory path.
 * @return `true` if the directory was successfully copied, `false` otherwise.
 */
bool dir_copy_directory(const char* srcDir, const char* destDir) {
    if (!srcDir || !destDir) {
        DIR_LOG("[dir_copy_directory] Error: Source or destination directory is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcDir = encoding_utf8_to_wchar(srcDir);
    wchar_t* wDestDir = encoding_utf8_to_wchar(destDir);

    if (wSrcDir == NULL || wDestDir == NULL) {
        DIR_LOG("[dir_copy_directory] Error: Conversion to wide-char failed.");
        free(wSrcDir);
        free(wDestDir);
        return false;
    }

    DIR_LOG("[dir_copy_directory] Copying directory from %s to %s", srcDir, destDir);

    // Append wildcard to source directory path
    wchar_t wSrcDirWildcard[MAX_PATH];
    wsprintfW(wSrcDirWildcard, L"%s\\*", wSrcDir);

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSrcDirWildcard, &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_copy_directory] Error: Failed to find files in %s.", srcDir);
        free(wSrcDir);
        free(wDestDir);
        return false;
    }

    // Create destination directory
    if (!CreateDirectoryW(wDestDir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        DIR_LOG("[dir_copy_directory] Error: Failed to create destination directory %s.", destDir);
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

    if (success) {
        DIR_LOG("[dir_copy_directory] Directory copied successfully from %s to %s", srcDir, destDir);
    } else {
        DIR_LOG("[dir_copy_directory] Error: Failed to copy directory from %s to %s", srcDir, destDir);
    }

    return success;

#else 
    DIR_LOG("[dir_copy_directory] Copying directory from %s to %s", srcDir, destDir);

    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(srcDir))) {
        DIR_LOG("[dir_copy_directory] Error: Failed to open source directory %s.", srcDir);
        return false;
    }

    if (mkdir(destDir, 0755) != 0) {
        DIR_LOG("[dir_copy_directory] Error: Failed to create destination directory %s.", destDir);
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
            DIR_LOG("[dir_copy_directory] Error: Failed to get status of source path %s.", srcPath);
            closedir(dir);
            return false;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            if (!dir_copy_directory(srcPath, destPath)) {
                DIR_LOG("[dir_copy_directory] Error: Failed to copy subdirectory %s.", srcPath);
                closedir(dir);
                return false;
            }
        }
        else {
            if (!dir_copy_file(srcPath, destPath)) {
                DIR_LOG("[dir_copy_directory] Error: Failed to copy file %s.", srcPath);
                closedir(dir);
                return false;
            }
        }
    }

    closedir(dir);
    DIR_LOG("[dir_copy_directory] Directory copied successfully from %s to %s", srcDir, destDir);
    return true;

#endif 
}

/**
 * @brief Gets the size of a directory and its contents.
 *
 * @param dirPath The path of the directory to calculate size.
 * @return The size of the directory in bytes, or `-1` on error.
 */
long long dir_get_directory_size(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_get_directory_size] Error: Directory path is NULL.");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        DIR_LOG("[dir_get_directory_size] Error: Conversion to wide-char failed.");
        return -1;
    }

    DIR_LOG("[dir_get_directory_size] Calculating size of directory: %s", dirPath);
    
    long long size = dir_get_size_win(wPath);
    if (size == -1) {
        DIR_LOG("[dir_get_directory_size] Error: Failed to get directory size for %s.", dirPath);
    }

    free(wPath);
    return size;

#else
    DIR_LOG("[dir_get_directory_size] Calculating size of directory: %s", dirPath);

    long long size = dir_get_size_posix(dirPath);
    if (size == -1) {
        DIR_LOG("[dir_get_directory_size] Error: Failed to get directory size for %s.", dirPath);
    }
    return size;
#endif
}

/**
 * @brief Gets the size of a file.
 *
 * @param filePath The path of the file to calculate size.
 * @return The size of the file in bytes, or `-1` on error.
 */
long long dir_get_file_size(const char* filePath) {
    if (!filePath) {
        DIR_LOG("[dir_get_file_size] Error: File path is NULL.");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        DIR_LOG("[dir_get_file_size] Error: Conversion to wide-char failed.");
        return -1;
    }

    DIR_LOG("[dir_get_file_size] Calculating size of file: %s", filePath);
    
    long long size = get_file_size_win(wPath);
    if (size == -1) {
        DIR_LOG("[dir_get_file_size] Error: Failed to get file size for %s.", filePath);
    }

    free(wPath);
    return size;

#else
    DIR_LOG("[dir_get_file_size] Calculating size of file: %s", filePath);

    long long size = get_file_size_posix(filePath);
    if (size == -1) {
        DIR_LOG("[dir_get_file_size] Error: Failed to get file size for %s.", filePath);
    }
    return size;
#endif
}


/**
 * @brief Lists the contents of a directory based on the specified option.
 *
 * @param dirPath The path of the directory to list.
 * @param option The option specifying what to list (files, directories, or both).
 * @param result A vector to store the results.
 */
void dir_list_contents(const char* dirPath, DirListOption option, Vector* resultVector) {
    if (!dirPath) {
        DIR_LOG("[dir_list_contents] Error: Directory path is NULL.");
        return;
    }
    if (!resultVector) {
        DIR_LOG("[dir_list_contents] Error: Result vector is NULL.");
        return;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        DIR_LOG("[dir_list_contents] Error: Conversion to wide-char failed.");
        return;
    }

    DIR_LOG("[dir_list_contents] Listing contents of directory: %s", dirPath);
    dir_list_contents_win(wPath, option, resultVector);
    free(wPath);

#else
    DIR_LOG("[dir_list_contents] Listing contents of directory: %s", dirPath);
    dir_list_contents_posix(dirPath, option, resultVector);
#endif
}

/**
 * @brief Checks if the specified path is a file.
 *
 * @param filePath The path to check.
 * @return `true` if the path is a file, `false` otherwise.
 */
bool dir_is_file(const char *filePath) {
    if (!filePath) {
        DIR_LOG("[dir_is_file] Error: File path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        DIR_LOG("[dir_is_file] Error: Conversion to wide-char failed.");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        DIR_LOG("[dir_is_file] Error: GetFileAttributesW failed or file does not exist for path %s.", filePath);
        return false;
    }

    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0;
    DIR_LOG("[dir_is_file] File check result for path %s: %s", filePath, result ? "true" : "false");
    return result;

#else 
    struct stat path_stat;
    if (stat(filePath, &path_stat) != 0) {
        DIR_LOG("[dir_is_file] Error: stat failed or file does not exist for path %s.", filePath);
        return false;
    }

    bool result = S_ISREG(path_stat.st_mode);
    DIR_LOG("[dir_is_file] File check result for path %s: %s", filePath, result ? "true" : "false");
    return result;
#endif 
}

/**
 * @brief Checks if the specified path is a directory.
 *
 * @param filePath The path to check.
 * @return `true` if the path is a directory, `false` otherwise.
 */
bool dir_is_directory(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_is_directory] Error: Directory path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        DIR_LOG("[dir_is_directory] Error: Conversion to wide-char failed.");
        return false;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        DIR_LOG("[dir_is_directory] Error: GetFileAttributesW failed or directory does not exist.");
        return false; 
    }

    bool result = (attribs & FILE_ATTRIBUTE_DIRECTORY) != 0;
    DIR_LOG("[dir_is_directory] Directory check result for path %s: %s", dirPath, result ? "true" : "false");
    
    return result;

#else 

    struct stat path_stat;
    if (stat(dirPath, &path_stat) != 0) {
        DIR_LOG("[dir_is_directory] Error: stat failed or directory does not exist for path %s.", dirPath);
        return false;
    }

    bool result = S_ISDIR(path_stat.st_mode);
    DIR_LOG("[dir_is_directory] Directory check result for path %s: %s", dirPath, result ? "true" : "false");
    return result;
#endif 
}


/**
 * @brief Moves a file from a source path to a destination path.
 *
 * @param srcPath The source file path.
 * @param destPath The destination file path.
 * @return `true` if the file was successfully moved, `false` otherwise.
 */
bool dir_move_file(const char* srcPath, const char* destPath) {
    if (!srcPath) {
        DIR_LOG("[dir_move_file] Error: Source path is NULL.");
        return false;
    }
    if (!destPath) {
        DIR_LOG("[dir_move_file] Error: Destination path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);
    if (!source || !destination) {
        DIR_LOG("[dir_move_file] Error: Conversion to wide-char failed.");
        free(source);
        free(destination);
        return false;
    }

    DIR_LOG("[dir_move_file] Moving file from %s to %s", srcPath, destPath);

    bool result = MoveFileW(source, destination) != 0;
    if (!result) {
        DIR_LOG("[dir_move_file] Error: MoveFileW failed.");
    } 
    else {
        DIR_LOG("[dir_move_file] File moved successfully.");
    }

    free(source);
    free(destination);
    return result;

#else 
    DIR_LOG("[dir_move_file] Moving file from %s to %s", srcPath, destPath);

    if (rename(srcPath, destPath) != 0) {
        DIR_LOG("[dir_move_file] Error: rename failed.");
        return false;
    }

    DIR_LOG("[dir_move_file] File moved successfully.");
    return true;
#endif
}

/**
 * @brief Moves a directory and its contents to a new location.
 *
 * @param srcPath The source directory path.
 * @param destPath The destination directory path.
 * @return `true` if the directory was successfully moved, `false` otherwise.
 */
bool dir_move_directory(const char* srcPath, const char* destPath) {
    if (!srcPath) {
        DIR_LOG("[dir_move_directory] Error: Source path is NULL.");
        return false;
    }
    if (!destPath) {
        DIR_LOG("[dir_move_directory] Error: Destination path is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* source = encoding_utf8_to_wchar(srcPath);
    wchar_t* destination = encoding_utf8_to_wchar(destPath);

    if (!source || !destination) {
        DIR_LOG("[dir_move_directory] Error: Conversion to wide-char failed.");
        free(source);
        free(destination);
        return false;
    }

    DIR_LOG("[dir_move_directory] Moving directory from %s to %s", srcPath, destPath);

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
        DIR_LOG("[dir_move_directory] Error: SHFileOperationW failed.");
    } 
    else {
        DIR_LOG("[dir_move_directory] Directory moved successfully.");
    }

    return result;

#else 
    DIR_LOG("[dir_move_directory] Moving directory from %s to %s", srcPath, destPath);

    if (rename(srcPath, destPath) != 0) {
        DIR_LOG("[dir_move_directory] Error: rename failed for paths %s to %s.", srcPath, destPath);
        return false;
    }

    DIR_LOG("[dir_move_directory] Directory moved successfully.");
    return true;
#endif
}

/**
 * @brief Gets the modified time of the specified directory.
 *
 * @param dirPath The path of the directory.
 * @return A string representing the modified time, or `NULL` on error.
 */
char* dir_get_modified_time(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_get_modified_time] Error: Directory path is NULL.");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        DIR_LOG("[dir_get_modified_time] Error: Conversion to wide-char failed.");
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        DIR_LOG("[dir_get_modified_time] Error: GetFileAttributesExW failed.");
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
    free(wPath); 

    if (!lastTime) {
        DIR_LOG("[dir_get_modified_time] Error: Conversion from wide-char to UTF-8 failed.");
        return NULL;
    }

    DIR_LOG("[dir_get_modified_time] Directory modified time retrieved: %s", lastTime);
    return lastTime;

#else // POSIX
    struct stat fileInfo;
    if (stat(dirPath, &fileInfo) != 0) {
        DIR_LOG("[dir_get_modified_time] Error: stat failed for directory %s.", dirPath);
        return NULL;
    }

    time_t lastModifiedTime = fileInfo.st_mtime;
    struct tm* timeinfo = localtime(&lastModifiedTime);
    char* timeString = malloc(sizeof(char) * 256);

    if (timeString == NULL) {
        DIR_LOG("[dir_get_modified_time] Error: Memory allocation failed.");
        return NULL;
    }

    strftime(timeString, 256, "%Y-%m-%d %H:%M:%S", timeinfo);
    DIR_LOG("[dir_get_modified_time] Directory modified time retrieved: %s", timeString);
    return timeString;
#endif
}

/**
 * @brief Gets the creation time of the specified directory.
 *
 * @param dirPath The path of the directory.
 * @return A string representing the creation time, or `NULL` on error.
 */
char* dir_get_creation_time(const char* dirPath) {
    if (!dirPath) {
        DIR_LOG("[dir_get_creation_time] Error: Directory path is NULL.");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(dirPath);
    if (!wPath) {
        DIR_LOG("[dir_get_creation_time] Error: Conversion to wide-char failed.");
        return NULL;
    }

    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    if (!GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo)) {
        DIR_LOG("[dir_get_creation_time] Error: GetFileAttributesExW failed.");
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
        DIR_LOG("[dir_get_creation_time] Error: Conversion from wide-char to UTF-8 failed.");
        return NULL;
    }

    DIR_LOG("[dir_get_creation_time] Directory creation time retrieved: %s", lastTime);
    return lastTime;

#else 
    DIR_LOG("[dir_get_creation_time] Error: POSIX systems typically don't have a creation time. Use dir_get_modified_time instead.");
    return NULL;
#endif 
}

/**
 * @brief Gets the user's home directory path.
 *
 * @return A string representing the home directory path, or `NULL` on error.
 */
char* dir_get_home_directory() {
    char* homeDir = NULL;

#if defined(_WIN32) || defined(_WIN64)
    wchar_t wPath[MAX_PATH];
    if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, wPath) == S_OK) {
        homeDir = encoding_wchar_to_utf8(wPath); // Convert from wchar_t to UTF-8
        if (homeDir) {
            DIR_LOG("[dir_get_home_directory] Home directory retrieved: %s", homeDir);
        } 
        else {
            DIR_LOG("[dir_get_home_directory] Error: Conversion from wide-char to UTF-8 failed.");
        }
    } 
    else {
        DIR_LOG("[dir_get_home_directory] Error: SHGetFolderPathW failed.");
    }

#else
    const char* homeEnv = getenv("HOME");
    if (homeEnv != NULL) {
        homeDir = string_strdup(homeEnv);
    } 
    else {
        struct passwd *pw = getpwuid(getuid());
        if (!pw) {
            DIR_LOG("[dir_get_home_directory] Error: Unable to retrieve home directory path on POSIX.");
            return NULL;
        }
        homeDir = string_strdup(pw->pw_dir);
    }
    if (!homeDir) {
        DIR_LOG("[dir_get_home_directory] Error: Failed to duplicate home directory string.");
        return NULL;
    }

    DIR_LOG("[dir_get_home_directory] Home directory retrieved: %s", homeDir);
#endif

    return homeDir; 
}

/**
 * @brief Determines the type of file located at the specified path.
 *
 * @param filePath The path of the file to check.
 * @return The type of the file, specified by the `DirFileType` enum.
 */
DirFileType dir_get_file_type(const char* filePath) {
    if (!filePath) {
        DIR_LOG("[dir_get_file_type] Error: File path is NULL.");
        return DIR_FILE_TYPE_UNKNOWN;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(filePath);
    if (!wPath) {
        DIR_LOG("[dir_get_file_type] Error: Failed to convert file path to wide-char.");
        return DIR_FILE_TYPE_UNKNOWN;
    }

    DWORD attribs = GetFileAttributesW(wPath);
    free(wPath);

    if (attribs == INVALID_FILE_ATTRIBUTES) {
        DIR_LOG("[dir_get_file_type] Error: GetFileAttributesW failed or file does not exist.");
        return DIR_FILE_TYPE_UNKNOWN;
    }
    if (attribs & FILE_ATTRIBUTE_DIRECTORY) {
        DIR_LOG("[dir_get_file_type] File type is directory: %s", filePath);
        return DIR_FILE_TYPE_DIRECTORY;
    } 
    else if (attribs & FILE_ATTRIBUTE_REPARSE_POINT) {
        DIR_LOG("[dir_get_file_type] File type is symbolic link: %s", filePath);
        return DIR_FILE_TYPE_SYMLINK;
    } 
    else {
        DIR_LOG("[dir_get_file_type] File type is regular: %s", filePath);
        return DIR_FILE_TYPE_REGULAR;
    }

#else
    struct stat path_stat;
    if (lstat(filePath, &path_stat) != 0) {
        DIR_LOG("[dir_get_file_type] Error: lstat failed for file %s.", filePath);
        return DIR_FILE_TYPE_UNKNOWN;
    }
    if (S_ISREG(path_stat.st_mode)) {
        DIR_LOG("[dir_get_file_type] File type is regular: %s", filePath);
        return DIR_FILE_TYPE_REGULAR;
    }
    else if (S_ISDIR(path_stat.st_mode)) {
        DIR_LOG("[dir_get_file_type] File type is directory: %s", filePath);
        return DIR_FILE_TYPE_DIRECTORY;
    } 
    else if (S_ISLNK(path_stat.st_mode)) {
        DIR_LOG("[dir_get_file_type] File type is symbolic link: %s", filePath);
        return DIR_FILE_TYPE_SYMLINK;
    } 
    else {
        DIR_LOG("[dir_get_file_type] File type is unknown: %s", filePath);
        return DIR_FILE_TYPE_UNKNOWN;
    }
#endif
}

/**
 * @brief Encrypts a file using the specified password and initialization vector.
 *
 * @param filePath The path of the file to encrypt.
 * @param password The password used for encryption.
 * @param iv The initialization vector used for encryption.
 * @return `true` if the file was successfully encrypted, `false` otherwise.
 */
bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv) {
    if (!filePath || !password || !iv) {
        DIR_LOG("[dir_encrypt_file] Error: Invalid arguments. filePath, password, or iv is NULL.");
        return false;
    }

    DIR_LOG("[dir_encrypt_file] Starting encryption for file: %s", filePath);

    FileReader* reader = file_reader_open(filePath, READ_BINARY);
    if (!reader) {
        DIR_LOG("[dir_encrypt_file] Error: Failed to open file for reading: %s", filePath);
        return false;
    }

    size_t fileLen = file_reader_get_size(reader);
    uint8_t* fileData = (uint8_t*)malloc(fileLen);
    if (!fileData) {
        DIR_LOG("[dir_encrypt_file] Error: Memory allocation failed.");
        file_reader_close(reader);
        return false;
    }

    if (file_reader_read(fileData, sizeof(uint8_t), fileLen, reader) != fileLen) {
        DIR_LOG("[dir_encrypt_file] Error: Failed to read file data.");
        free(fileData);
        file_reader_close(reader);
        return false;
    }
    file_reader_close(reader);

    if (strlen(password) > 8) {
        DIR_LOG("[dir_encrypt_file] Error: Password length exceeds 8 bytes.");
        free(fileData);
        return false;
    }

    uint8_t key[8]; // DES key size
    derive_key_from_password(password, key);
    DIR_LOG("[dir_encrypt_file] Key derived from password.");

    size_t outLen;
    uint8_t* encryptedData = (uint8_t*)crypto_des_encrypt(fileData, fileLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    free(fileData);

    if (!encryptedData) {
        DIR_LOG("[dir_encrypt_file] Error: Encryption failed.");
        return false;
    }

    FileWriter* writer = file_writer_open(filePath, WRITE_BINARY);
    if (!writer) {
        DIR_LOG("[dir_encrypt_file] Error: Failed to open file for writing: %s", filePath);
        free(encryptedData);
        return false;
    }

    if (file_writer_write(encryptedData, sizeof(uint8_t), outLen, writer) != outLen) {
        DIR_LOG("[dir_encrypt_file] Error: Failed to write encrypted data.");
        free(encryptedData);
        file_writer_close(writer);
        return false;
    }

    free(encryptedData);
    file_writer_close(writer);

    DIR_LOG("[dir_encrypt_file] File encryption completed successfully: %s", filePath);

    return true;
}

/**
 * @brief Decrypts a file using the specified password and initialization vector.
 *
 * @param filePath The path of the file to decrypt.
 * @param password The password used for decryption.
 * @param iv The initialization vector used for decryption.
 * @return `true` if the file was successfully decrypted, `false` otherwise.
 */
bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv) {
    if (!filePath || !password || !iv) {
        DIR_LOG("[dir_decrypt_file] Error: Invalid arguments. filePath, password, or iv is NULL.");
        return false;
    }

    DIR_LOG("[dir_decrypt_file] Starting decryption for file: %s", filePath);

    FileReader* reader = file_reader_open(filePath, READ_BINARY);
    if (!reader) {
        DIR_LOG("[dir_decrypt_file] Error: Failed to open file for reading: %s", filePath);
        return false;
    }

    size_t fileLen = file_reader_get_size(reader);
    uint8_t* fileData = (uint8_t*)malloc(fileLen);
    if (!fileData) {
        DIR_LOG("[dir_decrypt_file] Error: Memory allocation failed.");
        file_reader_close(reader);
        return false;
    }

    if (file_reader_read(fileData, sizeof(uint8_t), fileLen, reader) != fileLen) {
        DIR_LOG("[dir_decrypt_file] Error: Failed to read file data.");
        free(fileData);
        file_reader_close(reader);
        return false;
    }
    file_reader_close(reader);

    if (strlen(password) > 8) {
        DIR_LOG("[dir_decrypt_file] Error: Password length exceeds 8 bytes.");
        free(fileData);
        return false;
    }

    uint8_t key[8]; // DES key size
    derive_key_from_password(password, key);
    DIR_LOG("[dir_decrypt_file] Key derived from password.");

    size_t outLen;
    uint8_t* decryptedData = (uint8_t*)crypto_des_decrypt(fileData, fileLen, key, iv, CRYPTO_MODE_CBC, &outLen);
    free(fileData);

    if (!decryptedData) {
        DIR_LOG("[dir_decrypt_file] Error: Decryption failed.");
        return false;
    }

    FileWriter* writer = file_writer_open(filePath, WRITE_BINARY);
    if (!writer) {
        DIR_LOG("[dir_decrypt_file] Error: Failed to open file for writing: %s", filePath);
        free(decryptedData);
        return false;
    }

    if (file_writer_write(decryptedData, sizeof(uint8_t), outLen, writer) != outLen) {
        DIR_LOG("[dir_decrypt_file] Error: Failed to write decrypted data.");
        free(decryptedData);
        file_writer_close(writer);
        return false;
    }

    free(decryptedData);
    file_writer_close(writer);

    DIR_LOG("[dir_decrypt_file] File decryption completed successfully: %s", filePath);

    return true;
}

/**
 * @brief Retrieves the owner of the specified file.
 *
 * @param filePath The path of the file.
 * @param ownerBuffer The buffer to store the owner's name.
 * @param bufferSize The size of the buffer.
 * 
 * @return `true` if the owner was successfully retrieved, `false` otherwise.
 */
bool dir_get_file_owner(const char* filePath, char* ownerBuffer, size_t bufferSize) {
    if (!filePath || !ownerBuffer || bufferSize == 0) {
        DIR_LOG("[dir_get_file_owner] Error: Invalid arguments. filePath, ownerBuffer is NULL, or bufferSize is 0.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wFilePath = encoding_utf8_to_wchar(filePath);
    if (wFilePath == NULL) {
        DIR_LOG("[dir_get_file_owner] Error: Conversion to wide-char failed.");
        return false;
    }

    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION;
    PSECURITY_DESCRIPTOR pSD = NULL;
    PSID ownerSid = NULL;

    HANDLE hFile = CreateFileW(wFilePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_get_file_owner] Error: Unable to open file handle for file %s.", filePath);
        free(wFilePath);
        return false;
    }

    if (GetSecurityInfo(hFile, SE_FILE_OBJECT, si, &ownerSid, NULL, NULL, NULL, &pSD) != ERROR_SUCCESS) {
        DIR_LOG("[dir_get_file_owner] Error: GetSecurityInfo failed.");
        CloseHandle(hFile);
        free(wFilePath);
        return false;
    }

    CloseHandle(hFile);

    wchar_t name[512], domain[512];
    DWORD nameSize = 512, domainSize = 512;
    SID_NAME_USE sidUse;

    if (!LookupAccountSidW(NULL, ownerSid, name, &nameSize, domain, &domainSize, &sidUse)) {
        DIR_LOG("[dir_get_file_owner] Error: LookupAccountSidW failed.");
        free(wFilePath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    char* utf8OwnerName = encoding_wchar_to_utf8(name);
    if (utf8OwnerName == NULL) {
        DIR_LOG("[dir_get_file_owner] Error: Conversion from wide-char to UTF-8 failed.");
        free(wFilePath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    strncpy(ownerBuffer, utf8OwnerName, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    DIR_LOG("[dir_get_file_owner] File owner retrieved: %s", utf8OwnerName);

    free(wFilePath);
    free(utf8OwnerName);
    if (pSD) {
        LocalFree(pSD);
    }
    return true;

#else
    struct stat fileInfo;
    if (stat(filePath, &fileInfo) != 0) {
        DIR_LOG("[dir_get_file_owner] Error: stat failed for file %s.", filePath);
        return false;
    }

    struct passwd *pwd = getpwuid(fileInfo.st_uid);
    if (pwd == NULL) {
        DIR_LOG("[dir_get_file_owner] Error: getpwuid failed for UID %d.", fileInfo.st_uid);
        return false;
    }

    strncpy(ownerBuffer, pwd->pw_name, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    DIR_LOG("[dir_get_file_owner] File owner retrieved: %s", pwd->pw_name);
    return true;
#endif
}

/**
 * @brief Retrieves the owner of the specified directory.
 *
 * @param dirPath The path of the directory.
 * @param ownerBuffer The buffer to store the owner's name.
 * @param bufferSize The size of the buffer.
 * 
 * @return `true` if the owner was successfully retrieved, `false` otherwise.
 */
bool dir_get_directory_owner(const char* dirPath, char* ownerBuffer, size_t bufferSize) {
    if (!dirPath || !ownerBuffer || bufferSize == 0) {
        DIR_LOG("[dir_get_directory_owner] Error: Invalid arguments. dirPath, ownerBuffer is NULL or bufferSize is 0.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirPath = encoding_utf8_to_wchar(dirPath);
    if (wDirPath == NULL) {
        DIR_LOG("[dir_get_directory_owner] Error: Conversion to wide-char failed.");
        return false;
    }

    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION;
    PSECURITY_DESCRIPTOR pSD = NULL;
    PSID ownerSid = NULL;

    HANDLE hDir = CreateFileW(wDirPath, READ_CONTROL, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_get_directory_owner] Error: Unable to open directory handle for path: %s.", dirPath);
        free(wDirPath);
        return false;
    }

    if (GetSecurityInfo(hDir, SE_FILE_OBJECT, si, &ownerSid, NULL, NULL, NULL, &pSD) != ERROR_SUCCESS) {
        DIR_LOG("[dir_get_directory_owner] Error: GetSecurityInfo failed.");
        CloseHandle(hDir);
        free(wDirPath);
        return false;
    }

    CloseHandle(hDir);

    wchar_t name[512], domain[512];
    DWORD nameSize = 512, domainSize = 512;
    SID_NAME_USE sidUse;

    if (!LookupAccountSidW(NULL, ownerSid, name, &nameSize, domain, &domainSize, &sidUse)) {
        DIR_LOG("[dir_get_directory_owner] Error: LookupAccountSidW failed.");
        free(wDirPath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    char* utf8OwnerName = encoding_wchar_to_utf8(name);
    if (utf8OwnerName == NULL) {
        DIR_LOG("[dir_get_directory_owner] Error: Conversion from wide-char to UTF-8 failed.");
        free(wDirPath);
        if (pSD) {
            LocalFree(pSD);
        }
        return false;
    }

    strncpy(ownerBuffer, utf8OwnerName, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    DIR_LOG("[dir_get_directory_owner] Owner retrieved: %s", utf8OwnerName);

    free(wDirPath);
    free(utf8OwnerName);
    if (pSD) {
        LocalFree(pSD);
    }
    return true;

#else
    struct stat dirInfo;
    if (stat(dirPath, &dirInfo) != 0) {
        DIR_LOG("[dir_get_directory_owner] Error: stat failed for path %s.", dirPath);
        return false;
    }

    struct passwd *pwd = getpwuid(dirInfo.st_uid);
    if (pwd == NULL) {
        DIR_LOG("[dir_get_directory_owner] Error: getpwuid failed for UID %d.", dirInfo.st_uid);
        return false;
    }

    strncpy(ownerBuffer, pwd->pw_name, bufferSize - 1);
    ownerBuffer[bufferSize - 1] = '\0';

    DIR_LOG("[dir_get_directory_owner] Owner retrieved: %s", pwd->pw_name);
    return true;
#endif
}


/**
 * @brief Searches a directory for files matching a specified pattern.
 *
 * @param dirPath The path of the directory to search.
 * @param pattern The pattern to match files against.
 * @param callback The callback function to invoke for each matched file.
 * @param userData User-defined data to pass to the callback function.
 * @return `true` if the search was successful, `false` otherwise.
 */
bool dir_search(const char* dirPath, const char* pattern, DirCompareFunc callback, void* userData) {
    if (!dirPath || !pattern || !callback) {
        DIR_LOG("[dir_search] Error: Invalid arguments in dir_search. dirPath, pattern, or callback is NULL.");
        return false;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wDirPath = encoding_utf8_to_wchar(dirPath);
    wchar_t* wPattern = encoding_utf8_to_wchar(pattern);
    if (wDirPath == NULL || wPattern == NULL) {
        DIR_LOG("[dir_search] Error: Failed to convert UTF-8 strings to wide-char in dir_search.");
        free(wDirPath);
        free(wPattern);
        return false;
    }

    // Allocate memory for the combined path and pattern
    size_t combinedSize = wcslen(wDirPath) + wcslen(wPattern) + 1; // +1 for null terminator
    wchar_t* wSearchPath = (wchar_t*)malloc(combinedSize * sizeof(wchar_t));
    if (wSearchPath == NULL) {
        DIR_LOG("[dir_search] Error: Failed to allocate memory for search path in dir_search.");
        free(wDirPath);
        free(wPattern);
        return false;
    }

    // Combine dirPath and pattern
    wcscpy_s(wSearchPath, combinedSize, wDirPath);
    wcscat_s(wSearchPath, combinedSize, wPattern);

    DIR_LOG("[dir_search] Searching in directory: %s with pattern: %s", dirPath, pattern);

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSearchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_search] Error: FindFirstFileW failed in dir_search.");
        free(wDirPath);
        free(wPattern);
        free(wSearchPath);
        return false;
    }

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue; // Skip directories if you only want files
        }

        char* utf8FileName = encoding_wchar_to_utf8(findFileData.cFileName);
        if (!callback(utf8FileName, userData) || !utf8FileName) {
            free(utf8FileName);
            break;
        }
        DIR_LOG("[dir_search] Found file: %s", utf8FileName);
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
        DIR_LOG("[dir_search] Error: Failed to open directory '%s' in dir_search.", dirPath);
        return false;
    }

    DIR_LOG("[dir_search] Searching in directory: %s with pattern: %s", dirPath, pattern);

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name, 0) == 0) {
            if (!callback(entry->d_name, userData)) {
                break;
            }
            DIR_LOG("[dir_search] Found file: %s", entry->d_name);
        }
    }

    closedir(dir);
    return true;
#endif
}