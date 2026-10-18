/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Dir
*/

/* POSIX / GNU declarations (strdup, clock_gettime, localtime_r, ...) must be
   requested before the first #include, so that strict -std=c17 builds
   (CMAKE_C_EXTENSIONS=OFF) see them as well. */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
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
#include <io.h>        /* _waccess — dir_is_readable / dir_is_writable */
#include "accctrl.h"
#include "aclapi.h"

#define getcwd _getcwd
#define change_directory _wchdir
#define remove_directory _wrmdir



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

/* Allocates "<dir>\<name>", or the search pattern "<dir>\*" when name is NULL.
   The fixed wchar_t[MAX_PATH] buffers filled by wsprintfW overflowed the stack
   for paths longer than about 260 characters. Caller frees. */
static wchar_t* dir_wjoin(const wchar_t* dir, const wchar_t* name) {
    size_t a = wcslen(dir);
    size_t b = name ? wcslen(name) : 1;
    wchar_t* out = (wchar_t*)malloc((a + 1 + b + 1) * sizeof(wchar_t));
    if (!out) {
        DIR_LOG("[dir_wjoin] Error: Memory allocation failed.");
        return NULL;
    }
    memcpy(out, dir, a * sizeof(wchar_t));
    out[a] = L'\\';
    if (name) {
        memcpy(out + a + 1, name, b * sizeof(wchar_t));
    }
    else {
        out[a + 1] = L'*';
    }
    out[a + 1 + b] = L'\0';
    return out;
}

static int remove_directory_recursive_win(const wchar_t* dir) {
    if (!dir || !*dir) {
        DIR_LOG("[remove_directory_recursive_win] Error: Directory is NULL or empty.");
        return -1;
    }

    /* SHFileOperation treats the path as a pattern: "C:\x\targ*" deleted every
       matching entry (and plain files too), permanently. Only accept a real,
       existing directory with no wildcard characters. */
    if (wcspbrk(dir, L"*?") != NULL) {
        DIR_LOG("[remove_directory_recursive_win] Error: Wildcards are not allowed in %ls.", dir);
        return -1;
    }
    DWORD attrs = GetFileAttributesW(dir);
    if (attrs == INVALID_FILE_ATTRIBUTES || !(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
        DIR_LOG("[remove_directory_recursive_win] Error: %ls is not an existing directory.", dir);
        return -1;
    }

    /* SHFileOperation requires a fully qualified path (relative paths resolve
       against an unspecified directory) and a double-NUL terminated list. */
    DWORD fullLen = GetFullPathNameW(dir, 0, NULL, NULL);
    if (fullLen == 0) {
        DIR_LOG("[remove_directory_recursive_win] Error: Cannot resolve full path for %ls.", dir);
        return -1;
    }
    wchar_t* tempDir = (wchar_t*) calloc((size_t)fullLen + 2, sizeof(wchar_t));
    if (!tempDir) {
        DIR_LOG("[remove_directory_recursive_win] Error: Memory allocation failed.");
        return -1;
    }
    if (GetFullPathNameW(dir, fullLen + 1, tempDir, NULL) == 0) {
        DIR_LOG("[remove_directory_recursive_win] Error: Cannot resolve full path for %ls.", dir);
        free(tempDir);
        return -1;
    }

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

    // Prepare the search path
    wchar_t* searchPath = dir_wjoin(dirPath, NULL);
    if (!searchPath) {
        return -1;
    }
    hFind = FindFirstFileW(searchPath, &findFileData);
    free(searchPath);

    if (hFind == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_get_size_win] Error: Failed to open directory %ls.", dirPath);
        return -1;
    }

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (wcscmp(findFileData.cFileName, L".") != 0 && wcscmp(findFileData.cFileName, L"..") != 0) {
                if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    /* Do not descend into junctions/symlinked directories: they can
                       loop back into the tree and count data twice. */
                    if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                        continue;
                    }
                    wchar_t* fullPath = dir_wjoin(dirPath, findFileData.cFileName);
                    if (!fullPath) {
                        continue;
                    }
                    // Recursively calculate the size of directories; an
                    // unreadable subdirectory (-1) must not be added to the total.
                    long long sub = dir_get_size_win(fullPath);
                    free(fullPath);
                    if (sub > 0) {
                        size += sub;
                    }
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

    // Create a search pattern
    wchar_t* searchPath = dir_wjoin(dirPath, NULL);
    if (!searchPath) {
        return;
    }
    hFind = FindFirstFileW(searchPath, &findFileData);
    free(searchPath);

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
#include <time.h>   /* localtime() — used by the POSIX directory-time helpers */

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

/* Allocates "<dir>/<name>". Fixed 1024/PATH_MAX buffers silently truncated
   long paths, so the walkers operated on the wrong (truncated) path. */
static char* dir_join_alloc(const char* dir, const char* name) {
    size_t a = strlen(dir), b = strlen(name);
    char* out = (char*)malloc(a + 1 + b + 1);
    if (!out) {
        DIR_LOG("[dir_join_alloc] Error: Memory allocation failed.");
        return NULL;
    }
    memcpy(out, dir, a);
    out[a] = '/';
    memcpy(out + a + 1, name, b + 1);
    return out;
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
            char* fullPath = dir_join_alloc(dirPath, entry->d_name);
            if (!fullPath) {
                ret = -1;
                break;
            }

            /* d_type may be DT_UNKNOWN on some filesystems, so ask lstat. lstat
               (not stat) keeps a symlink to a directory from being followed:
               the link itself is removed, never the tree it points to. */
            struct stat st;
            bool isRealDir = (lstat(fullPath, &st) == 0) && S_ISDIR(st.st_mode);

            if (isRealDir) {
                DIR_LOG("[remove_directory_recursive_posix] Recursively removing directory: %s", fullPath);
                ret = remove_directory_recursive_posix(fullPath);
            }
            else {
                DIR_LOG("[remove_directory_recursive_posix] Removing file: %s", fullPath);
                ret = remove(fullPath);
            }
            free(fullPath);

            if (ret != 0) {
                DIR_LOG("[remove_directory_recursive_posix] Error: Failed to remove an entry of %s.", dirPath);
                break;
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
            char* fullPath = dir_join_alloc(dirPath, entry->d_name);
            if (!fullPath) {
                continue;
            }

            /* lstat: a symlink is not followed (a link back to an ancestor made
               the walk loop and count the same data many times). */
            if (lstat(fullPath, &statbuf) == -1) {
                DIR_LOG("[dir_get_size_posix] Error: Failed to get stats for file %s.", fullPath);
                free(fullPath);
                continue;
            }

            if (S_ISDIR(statbuf.st_mode)) {
                // Recursively calculate the size of directories; an unreadable
                // subdirectory (-1) must not be added to the total.
                long long sub = dir_get_size_posix(fullPath);
                if (sub > 0) {
                    size += sub;
                }
                DIR_LOG("[dir_get_size_posix] Accumulated size for directory %s: %lld bytes.", fullPath, size);
            }
            else {
                // Accumulate the size of each file
                size += statbuf.st_size;
                DIR_LOG("[dir_get_size_posix] File %s size: %lld bytes.", fullPath, (long long)statbuf.st_size);
            }
            free(fullPath);
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

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char* fullPath = dir_join_alloc(dirPath, entry->d_name);
        if (!fullPath) {
            continue;
        }
        int statRc = stat(fullPath, &statbuf);
        if (statRc == -1) {
            DIR_LOG("[dir_list_contents_posix] Error: Failed to get stats for file %s.", fullPath);
        }
        free(fullPath);
        if (statRc == -1) {
            continue;
        }

        bool isDir = S_ISDIR(statbuf.st_mode);
        if ((option == DIR_LIST_FILES && !isDir) || (option == DIR_LIST_DIRECTORIES && isDir) || (option == DIR_LIST_ALL)) {
            char* entryNameCopy = string_strdup(entry->d_name);

            if (entryNameCopy) {
                vector_push_back(resultVector, &entryNameCopy);
                DIR_LOG("[dir_list_contents_posix] Added entry: %s.", entryNameCopy);
            }
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

    /* The name of the directory a path refers to: its last component, with
       trailing separators ignored ("a/b/" -> "b"); "." is the current
       directory's own name. Scanning bytes is safe for UTF-8 because the
       separators are ASCII. On Windows both '\\' and '/' separate (the old
       code searched '\\' first, so "a\\b/c" gave "b/c", and "/foo" gave ""). */
    char* cwd = NULL;
    const char* path = dirpath;
    if (strcmp(dirpath, ".") == 0) {
        cwd = dir_current_path();
        if (!cwd) {
            DIR_LOG("[dir_dir_name] Error: Failed to retrieve current directory name.");
            return string_strdup("");
        }
        path = cwd;
    }

#if defined(_WIN32) || defined(_WIN64)
    #define DIR_IS_SEP(c) ((c) == '/' || (c) == '\\')
#else
    #define DIR_IS_SEP(c) ((c) == '/')
#endif
    size_t end = strlen(path);
    while (end > 0 && DIR_IS_SEP(path[end - 1])) {
        end--;
    }
    size_t start = end;
    while (start > 0 && !DIR_IS_SEP(path[start - 1])) {
        start--;
    }
#undef DIR_IS_SEP

    char* name = (char*)malloc(end - start + 1);
    if (name) {
        memcpy(name, path + start, end - start);
        name[end - start] = '\0';
        DIR_LOG("[dir_dir_name] Directory name for %s: %s", dirpath, name);
    }
    free(cwd);
    return name;
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

    wchar_t* wDirPath = encoding_utf8_to_wchar(dirpath);
    if (!wDirPath) {
        DIR_LOG("[dir_count] Error: utf8->wchar conversion failed for %s.", dirpath);
        return -1;
    }
    size_t needed = wcslen(wDirPath) + 3; /* "\\*" + NUL */
    wchar_t* wSearchPath = (wchar_t*)malloc(needed * sizeof(wchar_t));
    if (!wSearchPath) {
        DIR_LOG("[dir_count] Error: alloc failed.");
        free(wDirPath);
        return -1;
    }
    wcscpy(wSearchPath, wDirPath);
    wcscat(wSearchPath, L"\\*");
    free(wDirPath);

    hFind = FindFirstFileW(wSearchPath, &findFileData);
    free(wSearchPath);

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

    /* Wide API: the narrow ...A call read the UTF-8 bytes in the ANSI code
       page, so non-ASCII names failed unless that code page was UTF-8. */
    wchar_t* wDirName = encoding_utf8_to_wchar(dirName);
    if (!wDirName) {
        DIR_LOG("[dir_is_empty] Error: Conversion to wide-char failed.");
        return false;
    }
    BOOL empty = PathIsDirectoryEmptyW(wDirName);
    free(wDirName);
    if (empty != TRUE) {
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
 * An existing file at @p newName is replaced on every platform (POSIX
 * rename() always did this; Windows used to fail instead). An existing
 * directory is not replaced on Windows; POSIX rename() replaces it only
 * when it is empty. Renaming across volumes fails on every platform.
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

    if (!MoveFileExW(oldName_wchar, newName_wchar, MOVEFILE_REPLACE_EXISTING)) {
        DIR_LOG("[dir_rename] Error: MoveFileExW failed to rename from %s to %s.", oldName, newName);
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
 * A directory is not a file: the result is `false` for it (it used to be
 * `true` for any existing path). Symbolic links are followed.
 *
 * @param filePath The path of the file to check.
 * @return `true` if the path exists and is not a directory, `false` otherwise.
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

    bool result = (attribs != INVALID_FILE_ATTRIBUTES) && !(attribs & FILE_ATTRIBUTE_DIRECTORY);
    DIR_LOG("[dir_is_file_exists] File exists check for %s: %s", filePath, result ? "true" : "false");

    return result;

#else
    struct stat statbuf;
    bool result = (stat(filePath, &statbuf) == 0) && !S_ISDIR(statbuf.st_mode);
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
    /* Opening the destination for writing truncates it; when it is the source
       itself (same path, a hard link or another spelling) the data was lost
       and true was returned. Refuse, like `cp` ("are the same file"). */
    struct stat srcSt, destSt;
    if (stat(srcPath, &srcSt) == 0 && stat(destPath, &destSt) == 0 &&
        srcSt.st_dev == destSt.st_dev && srcSt.st_ino == destSt.st_ino) {
        DIR_LOG("[dir_copy_file] Error: %s and %s are the same file.", srcPath, destPath);
        return false;
    }

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
#if defined(_WIN32) || defined(_WIN64)
/* True when `inner` is `outer` itself or lies inside it (both absolute). */
static bool dir_wpath_within(const wchar_t* inner, const wchar_t* outer) {
    size_t n = wcslen(outer);
    while (n > 0 && (outer[n - 1] == L'\\' || outer[n - 1] == L'/')) {
        n--;
    }
    if (_wcsnicmp(inner, outer, n) != 0) {
        return false;
    }
    return inner[n] == L'\0' || inner[n] == L'\\' || inner[n] == L'/';
}

/* Absolute form of a wide path (caller frees), or NULL. */
static wchar_t* dir_wfull_path(const wchar_t* path) {
    DWORD len = GetFullPathNameW(path, 0, NULL, NULL);
    if (len == 0) {
        return NULL;
    }
    wchar_t* out = (wchar_t*)malloc(((size_t)len + 1) * sizeof(wchar_t));
    if (out && GetFullPathNameW(path, len + 1, out, NULL) == 0) {
        free(out);
        out = NULL;
    }
    return out;
}

static bool dir_copy_directory_win(const wchar_t* wSrcDir, const wchar_t* wDestDir) {
    wchar_t* wSrcDirWildcard = dir_wjoin(wSrcDir, NULL);
    if (!wSrcDirWildcard) {
        return false;
    }

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSrcDirWildcard, &findFileData);
    free(wSrcDirWildcard);

    if (hFind == INVALID_HANDLE_VALUE) {
        DIR_LOG("[dir_copy_directory] Error: Failed to find files in %ls.", wSrcDir);
        return false;
    }

    // Create destination directory
    if (!CreateDirectoryW(wDestDir, NULL) && GetLastError() != ERROR_ALREADY_EXISTS) {
        DIR_LOG("[dir_copy_directory] Error: Failed to create destination directory %ls.", wDestDir);
        FindClose(hFind);
        return false;
    }

    bool success = true;

    do {
        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) {
            continue;
        }

        wchar_t* srcPath = dir_wjoin(wSrcDir, findFileData.cFileName);
        wchar_t* destPath = dir_wjoin(wDestDir, findFileData.cFileName);
        if (!srcPath || !destPath) {
            free(srcPath);
            free(destPath);
            success = false;
            break;
        }

        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                /* A junction/symlinked directory can point back into the tree;
                   following it recursed without bound. It is not copied. */
                DIR_LOG("[dir_copy_directory] Skipping reparse-point directory %ls.", srcPath);
            }
            else {
                // Recursive call to copy subdirectory
                success = dir_copy_directory_win(srcPath, destPath);
            }
        }
        else {
            // Copy file
            char* srcPathUtf8 = encoding_wchar_to_utf8(srcPath);
            char* destPathUtf8 = encoding_wchar_to_utf8(destPath);
            success = srcPathUtf8 && destPathUtf8 && dir_copy_file(srcPathUtf8, destPathUtf8);

            free(srcPathUtf8);
            free(destPathUtf8);
        }
        free(srcPath);
        free(destPath);
    } while (success && FindNextFileW(hFind, &findFileData));

    FindClose(hFind);
    return success;
}

#else

/* True when `inner` is `outer` itself or lies inside it (both canonical). */
static bool dir_path_within(const char* inner, const char* outer) {
    size_t n = strlen(outer);
    while (n > 1 && outer[n - 1] == '/') {
        n--;
    }
    if (strncmp(inner, outer, n) != 0) {
        return false;
    }
    return inner[n] == '\0' || inner[n] == '/' || (n == 1 && outer[0] == '/');
}

static bool dir_copy_directory_posix(const char* srcDir, const char* destDir) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(srcDir))) {
        DIR_LOG("[dir_copy_directory] Error: Failed to open source directory %s.", srcDir);
        return false;
    }

    /* Suppress EEXIST so copying into a pre-existing destination succeeds,
       matching the Windows branch (which ignores ERROR_ALREADY_EXISTS). */
    if (mkdir(destDir, 0755) != 0 && errno != EEXIST) {
        DIR_LOG("[dir_copy_directory] Error: Failed to create destination directory %s.", destDir);
        closedir(dir);
        return false;
    }

    bool success = true;
    while (success && (entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char* srcPath = dir_join_alloc(srcDir, entry->d_name);
        char* destPath = dir_join_alloc(destDir, entry->d_name);
        if (!srcPath || !destPath) {
            free(srcPath);
            free(destPath);
            success = false;
            break;
        }

        struct stat statbuf;
        if (lstat(srcPath, &statbuf) == -1) {
            DIR_LOG("[dir_copy_directory] Error: Failed to get status of source path %s.", srcPath);
            success = false;
        }
        else if (S_ISLNK(statbuf.st_mode)) {
            char target[4096];
            ssize_t tl = readlink(srcPath, target, sizeof(target) - 1);
            if (tl < 0 || (size_t)tl >= sizeof(target) - 1) {
                DIR_LOG("[dir_copy_directory] Error: Failed to read symlink %s.", srcPath);
                success = false;
            }
            else {
                target[tl] = '\0';
                if (symlink(target, destPath) != 0 && errno != EEXIST) {
                    DIR_LOG("[dir_copy_directory] Error: Failed to recreate symlink %s.", destPath);
                    success = false;
                }
            }
        }
        else if (S_ISDIR(statbuf.st_mode)) {
            if (!dir_copy_directory_posix(srcPath, destPath)) {
                DIR_LOG("[dir_copy_directory] Error: Failed to copy subdirectory %s.", srcPath);
                success = false;
            }
        }
        else {
            if (!dir_copy_file(srcPath, destPath)) {
                DIR_LOG("[dir_copy_directory] Error: Failed to copy file %s.", srcPath);
                success = false;
            }
        }
        free(srcPath);
        free(destPath);
    }

    closedir(dir);
    return success;
}
#endif

bool dir_copy_directory(const char* srcDir, const char* destDir) {
    if (!srcDir || !destDir) {
        DIR_LOG("[dir_copy_directory] Error: Source or destination directory is NULL.");
        return false;
    }

    DIR_LOG("[dir_copy_directory] Copying directory from %s to %s", srcDir, destDir);

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wSrcDir = encoding_utf8_to_wchar(srcDir);
    wchar_t* wDestDir = encoding_utf8_to_wchar(destDir);
    wchar_t* fullSrc = wSrcDir ? dir_wfull_path(wSrcDir) : NULL;
    wchar_t* fullDest = wDestDir ? dir_wfull_path(wDestDir) : NULL;
    bool success = false;

    if (!wSrcDir || !wDestDir || !fullSrc || !fullDest) {
        DIR_LOG("[dir_copy_directory] Error: Path conversion failed.");
    }
    else if (dir_wpath_within(fullDest, fullSrc)) {
        /* Copying a tree into itself would copy the copy, forever. */
        DIR_LOG("[dir_copy_directory] Error: Destination %s is inside the source %s.", destDir, srcDir);
    }
    else {
        success = dir_copy_directory_win(fullSrc, fullDest);
    }
    free(wSrcDir);
    free(wDestDir);
    free(fullSrc);
    free(fullDest);
#else
    char* fullSrc = realpath(srcDir, NULL);
    bool success = false;
    if (!fullSrc) {
        DIR_LOG("[dir_copy_directory] Error: Failed to resolve source directory %s.", srcDir);
        return false;
    }

    char* destCopy = string_strdup(destDir);
    if (destCopy) {
        size_t dl = strlen(destCopy);
        while (dl > 1 && destCopy[dl - 1] == '/') {
            destCopy[--dl] = '\0';
        }
    }
    char* destName = destCopy ? strrchr(destCopy, '/') : NULL;
    char* destParent = NULL;
    if (destCopy) {
        if (destName) {
            *destName = '\0';
            destParent = realpath(*destCopy ? destCopy : "/", NULL);
        }
        else {
            destParent = realpath(".", NULL);
        }
    }
    if (!destParent) {
        DIR_LOG("[dir_copy_directory] Error: Failed to resolve destination parent of %s.", destDir);
    }
    else if (dir_path_within(destParent, fullSrc)) {
        /* Copying a tree into itself would copy the copy, forever. */
        DIR_LOG("[dir_copy_directory] Error: Destination %s is inside the source %s.", destDir, srcDir);
    }
    else {
        char* fullDest = realpath(destDir, NULL);
        if (fullDest && strcmp(fullDest, fullSrc) == 0) {
            DIR_LOG("[dir_copy_directory] Error: Source and destination are the same directory.");
        }
        else {
            success = dir_copy_directory_posix(srcDir, destDir);
        }
        free(fullDest);
    }
    free(destParent);
    free(destCopy);
    free(fullSrc);
#endif

    if (success) {
        DIR_LOG("[dir_copy_directory] Directory copied successfully from %s to %s", srcDir, destDir);
    }
    else {
        DIR_LOG("[dir_copy_directory] Error: Failed to copy directory from %s to %s", srcDir, destDir);
    }
    return success;
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
 * An existing file at @p destPath is replaced on every platform (POSIX
 * rename() always did this; Windows used to fail instead). A move across
 * volumes / file systems fails on every platform; copy and delete instead.
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

    bool result = MoveFileExW(source, destination, MOVEFILE_REPLACE_EXISTING) != 0;
    if (!result) {
        DIR_LOG("[dir_move_file] Error: MoveFileExW failed.");
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
    wsprintfW(timeString, L"%04d-%02d-%02d %02d:%02d:%02d", sysTime.wYear, sysTime.wMonth, sysTime.wDay, sysTime.wHour, sysTime.wMinute, sysTime.wSecond);

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
    /* localtime() shares a static struct tm and is a data race under concurrent
       use; localtime_r writes into a caller-owned buffer. */
    struct tm tmbuf;
    struct tm* timeinfo = localtime_r(&lastModifiedTime, &tmbuf);
    if (!timeinfo) {
        DIR_LOG("[dir_get_modified_time] Error: localtime_r() returned NULL.");
        return NULL;
    }

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
    /* POSIX exposes no portable file birth/creation time through stat()
       (st_ctime is the inode "change time", and a true btime needs statx() on a
       supporting filesystem). Fall back to st_ctime — for a freshly created
       file this equals its creation moment — so the API stays usable and
       cross-platform instead of always returning NULL. */
    struct stat fileInfo;
    if (stat(dirPath, &fileInfo) != 0) {
        DIR_LOG("[dir_get_creation_time] Error: stat failed for %s.", dirPath);
        return NULL;
    }

    time_t creationTime = fileInfo.st_ctime;
    /* Reentrant localtime — see the note in dir_get_modified_time. */
    struct tm tmbuf;
    struct tm* timeinfo = localtime_r(&creationTime, &tmbuf);
    if (!timeinfo) {
        DIR_LOG("[dir_get_creation_time] Error: localtime_r() returned NULL.");
        return NULL;
    }

    char* timeString = malloc(sizeof(char) * 256);
    if (timeString == NULL) {
        DIR_LOG("[dir_get_creation_time] Error: Memory allocation failed.");
        return NULL;
    }

    strftime(timeString, 256, "%Y-%m-%d %H:%M:%S", timeinfo);
    DIR_LOG("[dir_get_creation_time] Creation time (st_ctime fallback) retrieved: %s", timeString);
    return timeString;
#endif
}


/**
 * @brief Modification time of a file or directory as Unix epoch seconds.
 *
 * Unlike `dir_get_modified_time`, which returns a localized
 * "YYYY-MM-DD HH:MM:SS" string, this returns a machine-comparable integer
 * (seconds since 1970-01-01T00:00:00 UTC). Use it to sort by age, detect
 * changes, or invalidate caches. Works on both files and directories.
 *
 * @param path Filesystem path. Must not be NULL.
 * @return Unix time in seconds, or -1 on a NULL path or if `path` can't be
 *         stat'd (e.g. it does not exist).
 */
int64_t dir_get_modified_time_unix(const char* path) {
    if (!path) {
        DIR_LOG("[dir_get_modified_time_unix] Error: path is NULL.");
        return -1;
    }

#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(path);
    if (!wPath) {
        DIR_LOG("[dir_get_modified_time_unix] Error: UTF-8 to wide-char conversion failed.");
        return -1;
    }
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;
    BOOL ok = GetFileAttributesExW(wPath, GetFileExInfoStandard, &fileInfo);
    free(wPath);
    if (!ok) {
        DIR_LOG("[dir_get_modified_time_unix] Error: GetFileAttributesExW failed.");
        return -1;
    }
    /* FILETIME counts 100-ns ticks since 1601-01-01 UTC; shift to the Unix epoch. */
    uint64_t ticks = ((uint64_t)fileInfo.ftLastWriteTime.dwHighDateTime << 32)
                   | (uint64_t)fileInfo.ftLastWriteTime.dwLowDateTime;
    return (int64_t)(ticks / 10000000ULL) - 11644473600LL;
#else
    struct stat fileInfo;
    if (stat(path, &fileInfo) != 0) {
        DIR_LOG("[dir_get_modified_time_unix] Error: stat failed for %s.", path);
        return -1;
    }
    return (int64_t)fileInfo.st_mtime;
#endif
}


/**
 * @brief Whether the current process can read `path`.
 *
 * Probes read access with `access(R_OK)` on POSIX and `_waccess` on Windows; a
 * missing path returns false. On Windows the check reflects the file's
 * attributes rather than full ACLs (the platform's documented best effort).
 *
 * @param path Filesystem path. Must not be NULL.
 * @return true if `path` exists and is readable, false otherwise.
 */
bool dir_is_readable(const char* path) {
    if (!path) {
        DIR_LOG("[dir_is_readable] Error: path is NULL.");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(path);
    if (!wPath) {
        return false;
    }
    int rc = _waccess(wPath, 4);   /* 4 = read */
    free(wPath);
    return rc == 0;
#else
    return access(path, R_OK) == 0;
#endif
}


/**
 * @brief Whether the current process can write to `path`.
 *
 * Probes write access with `access(W_OK)` on POSIX and `_waccess` on Windows; a
 * missing path returns false. On Windows the check reflects the read-only
 * attribute rather than full ACLs.
 *
 * @param path Filesystem path. Must not be NULL.
 * @return true if `path` exists and is writable, false otherwise.
 */
bool dir_is_writable(const char* path) {
    if (!path) {
        DIR_LOG("[dir_is_writable] Error: path is NULL.");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(path);
    if (!wPath) {
        return false;
    }
    int rc = _waccess(wPath, 2);   /* 2 = write */
    free(wPath);
    return rc == 0;
#else
    return access(path, W_OK) == 0;
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
/*
 * Encrypted file layout written by dir_encrypt_file:
 *
 *   "CSTDAEG1"                                   8 bytes, plaintext marker
 *   crypto_password_encrypt blob                 PBKDF2-HMAC-SHA256 + AES-256-GCM
 *                                                (iterations, salt, nonce, tag, data)
 *
 * Earlier versions wrote single DES ("CSTDDES2" below, or no marker at all)
 * with the password as the raw 8-byte key; dir_decrypt_file still reads them:
 *
 *   "CSTDDES2"                                   8 bytes, plaintext marker
 *   DES-CBC( "CSTDCHK1" | length (8 bytes, LE) | data | zero padding )
 *
 * The encrypted check block lets dir_decrypt_file detect a wrong password or
 * IV instead of overwriting the file with garbage, and the stored length
 * restores the exact original size (zero padding alone cannot tell real
 * trailing NUL bytes from padding). Both functions replace the file atomically
 * (temporary file + rename), so a failure never destroys the original.
 */
#define DIR_ENC_MAGIC     "CSTDDES2"       /* legacy DES files (decrypt only) */
#define DIR_ENC_MAGIC_AEAD "CSTDAEG1"      /* current: marker + crypto_password_encrypt blob */
#define DIR_ENC_CHECK     "CSTDCHK1"
#define DIR_ENC_BLOCK     8
#define DIR_ENC_HEADER    (2 * DIR_ENC_BLOCK)   /* check block + length block */

/* Legacy DES files were zero-padded, but crypto_des_decrypt now removes (and
   checks) PKCS#7 padding. Append one ciphertext block that CBC-decrypts to a
   full padding block, C' = E_K(0x08.. XOR C_last), so the stored bytes come
   back unchanged (padding included, as before). */
static uint8_t* dir_des_cbc_decrypt_legacy(const uint8_t* cipher, size_t len, const uint8_t key[8], const uint8_t* iv, size_t* outLen) {
    uint8_t x[8];
    for (int i = 0; i < 8; ++i) {
        x[i] = (uint8_t)(cipher[len - 8 + i] ^ 0x08);
    }

    size_t encLen = 0;
    uint8_t* e = (uint8_t*)crypto_des_encrypt(x, sizeof x, key, NULL, CRYPTO_MODE_ECB, &encLen);
    if (!e || encLen < 8) {
        free(e);
        return NULL;
    }

    uint8_t* ext = (uint8_t*)malloc(len + 8);
    if (!ext) {
        free(e);
        return NULL;
    }

    memcpy(ext, cipher, len);
    memcpy(ext + len, e, 8);
    memset(e, 0, encLen);
    free(e);
    uint8_t* pt = (uint8_t*)crypto_des_decrypt(ext, len + 8, key, iv, CRYPTO_MODE_CBC, outLen);
    free(ext);
    return pt;
}

/* Reads the whole file into a malloc'd buffer (binary-safe). */
static bool dir_read_file_bytes(const char* filePath, uint8_t** data, size_t* len) {
    FileReader* reader = file_reader_open(filePath, READ_BINARY);
    if (!reader) {
        DIR_LOG("[dir_read_file_bytes] Error: Failed to open file for reading: %s", filePath);
        return false;
    }
    char* buf = NULL;
    size_t size = 0;
    bool ok = file_reader_read_all(reader, &buf, &size);
    file_reader_close(reader);
    if (!ok) {
        DIR_LOG("[dir_read_file_bytes] Error: Failed to read file data: %s", filePath);
        free(buf);
        return false;
    }
    *data = (uint8_t*)buf;
    *len = size;
    return true;
}

bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv) {

    (void)iv;
    if (!filePath || !password || !*password) {
        DIR_LOG("[dir_encrypt_file] Error: Invalid arguments.");
        return false;
    }

    uint8_t* fileData = NULL;
    size_t fileLen = 0;
    if (!dir_read_file_bytes(filePath, &fileData, &fileLen)) {
        DIR_LOG("[dir_encrypt_file] Error: Could not read %s.", filePath);
        return false;
    }

    /* PBKDF2-HMAC-SHA256 (random salt) -> AES-256-GCM. The blob carries the
       iteration count, salt, nonce and authentication tag, so any password
       length works and tampering or a wrong password is detected. (It used
       to be single DES with the password bytes as the raw 8-byte key.) */
    size_t blobLen = 0;
    uint8_t* blob = crypto_password_encrypt(fileData, fileLen, password, 0, &blobLen);
    crypto_secure_zero(fileData, fileLen);
    free(fileData);
    if (!blob || blobLen > SIZE_MAX - DIR_ENC_BLOCK) {
        free(blob);
        DIR_LOG("[dir_encrypt_file] Error: Encryption failed.");
        return false;
    }

    uint8_t* out = (uint8_t*)malloc(DIR_ENC_BLOCK + blobLen);
    if (!out) {
        free(blob);
        return false;
    }
    memcpy(out, DIR_ENC_MAGIC_AEAD, DIR_ENC_BLOCK);
    memcpy(out + DIR_ENC_BLOCK, blob, blobLen);
    free(blob);

    bool ok = file_writer_write_all_atomic(filePath, out, DIR_ENC_BLOCK + blobLen);
    free(out);
    if (!ok) {
        DIR_LOG("[dir_encrypt_file] Error: Could not replace %s.", filePath);
        return false;
    }

    DIR_LOG("[dir_encrypt_file] File encrypted successfully: %s", filePath);
    return true;
}

/**
 * @brief Decrypts a file using the specified password and initialization vector.
 *
 * @param filePath The path of the file to decrypt.
 * @param password The password used for decryption.
 * @param iv The initialization vector used for decryption.
 * @return `true` if the file was successfully decrypted, `false` otherwise
 *         (wrong password/IV, not an encrypted file, or I/O error - the file is
 *         left unchanged in every failure case).
 */
bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv) {
    if (!filePath || !password) {
        return false;
    }

    uint8_t* fileData = NULL;
    size_t fileLen = 0;
    if (!dir_read_file_bytes(filePath, &fileData, &fileLen)) {
        return false;
    }

    /* Current format: marker + crypto_password_encrypt blob. */
    if (fileLen >= DIR_ENC_BLOCK && memcmp(fileData, DIR_ENC_MAGIC_AEAD, DIR_ENC_BLOCK) == 0) {
        size_t plainLen = 0;
        uint8_t* plain = crypto_password_decrypt(fileData + DIR_ENC_BLOCK, fileLen - DIR_ENC_BLOCK, password, &plainLen);
        free(fileData);
        if (!plain) {
            DIR_LOG("[dir_decrypt_file] Error: Wrong password or damaged file: %s", filePath);
            return false;   /* the file is left unchanged */
        }
        bool ok = file_writer_write_all_atomic(filePath, plain, plainLen);
        crypto_secure_zero(plain, plainLen);
        free(plain);
        return ok;
    }

    /* Older DES files ("CSTDDES2" or untagged) can still be decrypted; they
       need the 8-byte IV and a password of at most 8 bytes. */
    if (!iv || strlen(password) > 8) {
        free(fileData);
        return false;
    }

    bool tagged = fileLen >= DIR_ENC_BLOCK && memcmp(fileData, DIR_ENC_MAGIC, DIR_ENC_BLOCK) == 0;
    const uint8_t* cipher = tagged ? fileData + DIR_ENC_BLOCK : fileData;
    size_t cipherLen = tagged ? fileLen - DIR_ENC_BLOCK : fileLen;

    if (cipherLen == 0 || cipherLen % DIR_ENC_BLOCK != 0 || (tagged && cipherLen < DIR_ENC_HEADER)) {
        DIR_LOG("[dir_decrypt_file] Error: %s is not a DES-CBC encrypted file.", filePath);
        free(fileData);
        return false;
    }

    uint8_t key[8]; // DES key size
    derive_key_from_password(password, key);
    DIR_LOG("[dir_decrypt_file] Key derived from password.");

    size_t outLen = 0;
    uint8_t* decryptedData = dir_des_cbc_decrypt_legacy(cipher, cipherLen, key, iv, &outLen);
    free(fileData);
    memset(key, 0, sizeof(key));

    if (!decryptedData) {
        DIR_LOG("[dir_decrypt_file] Error: Decryption failed.");
        return false;
    }

    const uint8_t* result = decryptedData;
    size_t resultLen = outLen;
    if (tagged) {
        uint64_t len64 = 0;
        for (int i = 0; i < 8; ++i) {
            len64 |= (uint64_t)decryptedData[DIR_ENC_BLOCK + i] << (8 * i);
        }
        if (memcmp(decryptedData, DIR_ENC_CHECK, DIR_ENC_BLOCK) != 0 ||
            len64 > (uint64_t)(outLen - DIR_ENC_HEADER)) {
            DIR_LOG("[dir_decrypt_file] Error: Wrong password or IV for %s (file left unchanged).", filePath);
            free(decryptedData);
            return false;
        }
        result = decryptedData + DIR_ENC_HEADER;
        resultLen = (size_t)len64;
    }
    /* Untagged input is a file written by older versions: the original length
       was not stored, so the zero padding cannot be removed reliably. */

    bool ok = file_writer_write_all_atomic(filePath, result, resultLen);
    free(decryptedData);
    if (!ok) {
        DIR_LOG("[dir_decrypt_file] Error: Failed to write decrypted data (encrypted file left intact).");
        return false;
    }

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
 * Only entries that are not directories are reported, on every platform
 * (POSIX used to report ".", ".." and subdirectories as well). The callback
 * receives the bare entry name. A search with no match succeeds.
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

    /* Combine dirPath + "\\" + pattern so callers can pass "*.txt" and
       have it resolve correctly inside the directory. +2 for the
       separator and the null terminator. */
    size_t dirLen = wcslen(wDirPath);
    bool   needSep = (dirLen > 0 && wDirPath[dirLen - 1] != L'\\' && wDirPath[dirLen - 1] != L'/');
    size_t combinedSize = dirLen + (needSep ? 1 : 0) + wcslen(wPattern) + 1;
    wchar_t* wSearchPath = (wchar_t*)malloc(combinedSize * sizeof(wchar_t));
    if (wSearchPath == NULL) {
        DIR_LOG("[dir_search] Error: Failed to allocate memory for search path in dir_search.");
        free(wDirPath);
        free(wPattern);
        return false;
    }

    wcscpy_s(wSearchPath, combinedSize, wDirPath);
    if (needSep) wcscat_s(wSearchPath, combinedSize, L"\\");
    wcscat_s(wSearchPath, combinedSize, wPattern);

    DIR_LOG("[dir_search] Searching in directory: %s with pattern: %s", dirPath, pattern);

    WIN32_FIND_DATAW findFileData;
    HANDLE hFind = FindFirstFileW(wSearchPath, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        /* ERROR_FILE_NOT_FOUND: the directory exists but nothing matches,
           which POSIX reports as a successful empty search. */
        bool noMatch = (GetLastError() == ERROR_FILE_NOT_FOUND);
        if (!noMatch) {
            DIR_LOG("[dir_search] Error: FindFirstFileW failed in dir_search.");
        }
        free(wDirPath);
        free(wPattern);
        free(wSearchPath);
        return noMatch;
    }

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue; /* skip directories — we only report files */
        }

        char* utf8FileName = encoding_wchar_to_utf8(findFileData.cFileName);
        if (!utf8FileName) {
            DIR_LOG("[dir_search] Skipping entry: utf-8 conversion failed.");
            continue;
        }
        bool keep_going = callback(utf8FileName, userData);
        DIR_LOG("[dir_search] Found file: %s", utf8FileName);
        free(utf8FileName);
        if (!keep_going) break;
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

    size_t dirLen = strlen(dirPath);
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (fnmatch(pattern, entry->d_name, 0) != 0) {
            continue;
        }
        /* skip directories (".", ".." included), as the Windows branch does */
        size_t full = dirLen + 1 + strlen(entry->d_name) + 1;
        char* entryPath = (char*)malloc(full);
        if (!entryPath) {
            DIR_LOG("[dir_search] Error: out of memory.");
            closedir(dir);
            return false;
        }
        snprintf(entryPath, full, "%s/%s", dirPath, entry->d_name);
        struct stat st;
        bool isDir = (stat(entryPath, &st) == 0) && S_ISDIR(st.st_mode);
        free(entryPath);
        if (isDir) {
            continue;
        }
        DIR_LOG("[dir_search] Found file: %s", entry->d_name);
        if (!callback(entry->d_name, userData)) {
            break;
        }
    }

    closedir(dir);
    return true;
#endif
}


/**
 * @brief Returns the native path separator for the current platform.
 *
 * Behavior:
 *   - Windows builds (`_WIN32` / `_WIN64`) return `'\\'`.
 *   - All other platforms (Linux, macOS, BSD, ...) return `'/'`.
 *
 * @return The platform-native path separator character.
 */
char dir_path_separator(void) {
    DIR_LOG("[dir_path_separator]: enter");
#if defined(_WIN32) || defined(_WIN64)
    DIR_LOG("[dir_path_separator]: exit -> '\\\\' (Windows)");
    return '\\';
#else
    DIR_LOG("[dir_path_separator]: exit -> '/' (POSIX)");
    return '/';
#endif
}


/**
 * @brief Reports whether @p path is an absolute filesystem path.
 *
 * Recognized as absolute:
 *   - Windows: a leading backslash or forward slash (UNC-style), OR
 *     a drive prefix matching `[A-Za-z]:[\\/]`.
 *   - POSIX: starts with `/`.
 *
 * @param path Path to inspect. May be NULL or empty.
 * @return true if the path is absolute, false otherwise (incl. NULL /
 *         empty input).
 */
bool dir_is_absolute_path(const char* path) {
    DIR_LOG("[dir_is_absolute_path]: enter path=%s", path ? path : "(null)");
    if (!path || !*path) {
        DIR_LOG("[dir_is_absolute_path]: NULL or empty -> false");
        return false;
    }
#if defined(_WIN32) || defined(_WIN64)
    if (path[0] == '\\' || path[0] == '/') {
        DIR_LOG("[dir_is_absolute_path]: leading slash -> true (Windows)");
        return true;
    }
    if (((path[0] >= 'A' && path[0] <= 'Z') || (path[0] >= 'a' && path[0] <= 'z')) && path[1] == ':' &&
        (path[2] == '\\' || path[2] == '/')) {
        DIR_LOG("[dir_is_absolute_path]: drive-letter prefix -> true");
        return true;
    }
    DIR_LOG("[dir_is_absolute_path]: not absolute -> false");
    return false;
#else
    bool out = (path[0] == '/');
    DIR_LOG("[dir_is_absolute_path]: exit -> %s", out ? "true" : "false");
    return out;
#endif
}


/**
 * @brief Joins two path segments with the platform separator.
 *
 * Behavior:
 *   - `a` is NULL or empty → returns a copy of `b` (or `""` if `b` is
 *     also NULL/empty).
 *   - `b` is NULL or empty → returns a copy of `a`.
 *   - `b` is absolute (see @ref dir_is_absolute_path) → returns a copy
 *     of `b` verbatim, matching std-library convention.
 *   - Otherwise: trims trailing separator(s) off `a`, trims leading
 *     separator(s) off the *trimmed*-then-relative `b`, and joins with
 *     exactly one native separator.
 *
 * @param a Left segment. May be NULL or empty.
 * @param b Right segment. May be NULL or empty.
 *
 * @return Newly-allocated joined path the caller must `free()`. NULL
 *         on heap-allocation failure only.
 */
char* dir_join_path(const char* a, const char* b) {
    DIR_LOG("[dir_join_path]: enter a=%s b=%s",
            a ? a : "(null)", b ? b : "(null)");

    char sep = dir_path_separator();

    if (!a || !*a) {
        if (b) {
            DIR_LOG("[dir_join_path]: NULL/empty a -> copy of b");
            return string_strdup(b);
        }
        DIR_LOG("[dir_join_path]: both NULL/empty -> \"\"");
        return string_strdup("");
    }
    if (!b || !*b) {
        DIR_LOG("[dir_join_path]: NULL/empty b -> copy of a");
        return string_strdup(a);
    }
    if (dir_is_absolute_path(b)) {
        DIR_LOG("[dir_join_path]: b is absolute -> copy of b");
        return string_strdup(b);
    }

    size_t la = strlen(a);
    size_t lb = strlen(b);

    /* Trim trailing separators off `a`. */
    while (la > 0 && (a[la - 1] == '\\' || a[la - 1] == '/')) {
        --la;
    }
    /* Trim leading separators off `b`. */
    size_t b_off = 0;
    while (b[b_off] == '\\' || b[b_off] == '/') {
        ++b_off;
    }
    lb -= b_off;

    char* out = (char*)malloc(la + 1 + lb + 1);
    if (!out) {
        DIR_LOG("[dir_join_path]: malloc failed");
        return NULL;
    }
    memcpy(out, a, la);
    out[la] = sep;
    memcpy(out + la + 1, b + b_off, lb);
    out[la + 1 + lb] = '\0';

    DIR_LOG("[dir_join_path]: exit -> %s", out);
    return out;
}


/**
 * @brief Returns the final component (basename) of a path.
 *
 * Behavior:
 *   - `"C:/foo/bar.txt"` → `"bar.txt"`
 *   - `"/usr/local/"`    → `"local"` (trailing separators trimmed)
 *   - `"plain.txt"`      → `"plain.txt"` (no parent)
 *   - `""`               → `""` (a single empty character is returned)
 *
 * @param path Input path. May be NULL.
 *
 * @return Newly-allocated component the caller must `free()`. NULL on
 *         NULL input or heap-allocation failure.
 */
char* dir_base_name(const char* path) {
    DIR_LOG("[dir_base_name]: enter path=%s", path ? path : "(null)");
    if (!path) {
        DIR_LOG("[dir_base_name]: NULL input -> NULL");
        return NULL;
    }

    size_t len = strlen(path);
    /* Strip trailing separators. */
    while (len > 0 && (path[len - 1] == '\\' || path[len - 1] == '/')) {
        --len;
    }
    if (len == 0) {
        /* All-separators or empty input. Return a 1-char copy of the
           first byte (typically '\0' or '/') so the result still has a
           defined value. */
        char* out = (char*)malloc(2);
        if (!out) {
            DIR_LOG("[dir_base_name]: malloc failed");
            return NULL;
        }
        out[0] = path[0];
        out[1] = '\0';
        DIR_LOG("[dir_base_name]: exit -> \"%s\" (empty/all-sep)", out);
        return out;
    }

    /* Walk back to the last separator (or beginning of string). */
    size_t start = len;
    while (start > 0 && path[start - 1] != '\\' && path[start - 1] != '/') {
        --start;
    }

    size_t outLen = len - start;
    char* out = (char*)malloc(outLen + 1);
    if (!out) {
        DIR_LOG("[dir_base_name]: malloc failed");
        return NULL;
    }

    memcpy(out, path + start, outLen);
    out[outLen] = '\0';

    DIR_LOG("[dir_base_name]: exit -> %s", out);
    return out;
}


/**
 * @brief Returns the extension of a path INCLUDING the leading dot.
 *
 * Behavior:
 *   - `"file.tar.gz"` → `".gz"` (only the last extension).
 *   - `"Makefile"`    → `""`     (no dot in basename).
 *   - `"/etc/.bashrc"` → `""`    (a leading dot in the basename is a
 *                                  hidden-file marker, NOT an extension).
 *
 * @param path Input path. May be NULL.
 *
 * @return Newly-allocated extension (possibly empty); caller must
 *         `free()`. NULL on NULL input or OOM.
 */
char* dir_extension(const char* path) {
    DIR_LOG("[dir_extension]: enter path=%s", path ? path : "(null)");
    if (!path) {
        DIR_LOG("[dir_extension]: NULL input -> NULL");
        return NULL;
    }

    char* base = dir_base_name(path);
    if (!base) {
        DIR_LOG("[dir_extension]: dir_base_name failed -> NULL");
        return NULL;
    }

    char* dot = strrchr(base, '.');
    char* out;
    if (!dot || dot == base) {
        DIR_LOG("[dir_extension]: no extension -> \"\"");
        out = string_strdup("");
    } 
    else {
        DIR_LOG("[dir_extension]: extension = %s", dot);
        out = string_strdup(dot);
    }
    free(base);
    return out;
}


/**
 * @brief Returns a copy of @p path with its extension replaced.
 *
 * @p newExt may be passed with or without a leading dot; the function
 * normalises both forms identically.
 *
 * Behavior:
 *   - Empty `newExt` strips the extension (`"foo.txt"` → `"foo"`).
 *   - If `path` has no extension, `newExt` is appended (with a single
 *     leading dot if `newExt` didn't already have one).
 *   - A leading dot inside the basename (e.g. `".bashrc"`) is preserved
 *     and treated as a hidden-file marker, not an extension boundary.
 *
 * @param path   Input path. Must be non-NULL.
 * @param newExt New extension, with or without a leading dot. Must be
 *               non-NULL (use `""` to strip).
 *
 * @return Newly-allocated path the caller must `free()`. NULL on bad
 *         input or heap-allocation failure.
 */
char* dir_change_extension(const char* path, const char* newExt) {
    DIR_LOG("[dir_change_extension]: enter path=%s newExt=%s",
            path ? path : "(null)", newExt ? newExt : "(null)");
    if (!path || !newExt) {
        DIR_LOG("[dir_change_extension]: NULL input -> NULL");
        return NULL;
    }

    size_t len = strlen(path);
    size_t baseStart = len;

    while (baseStart > 0 && path[baseStart - 1] != '\\' && path[baseStart - 1] != '/') {
        --baseStart;
    }

    size_t dotPos = len;
    for (size_t i = len; i > baseStart; --i) {
        if (path[i - 1] == '.') {
            dotPos = i - 1;
            break;
        }
    }

    if (dotPos == baseStart && baseStart < len && path[baseStart] == '.') {
        dotPos = len;
    }

    bool   extHasDot = (newExt[0] == '.');
    size_t extLen    = strlen(newExt);
    size_t newLen    = dotPos + (extHasDot ? 0 : (extLen > 0 ? 1 : 0)) + extLen;

    char* out = (char*)malloc(newLen + 1);
    if (!out) {
        DIR_LOG("[dir_change_extension]: malloc failed");
        return NULL;
    }
    memcpy(out, path, dotPos);

    size_t p = dotPos;
    if (extLen > 0 && !extHasDot) {
        out[p++] = '.';
    }

    memcpy(out + p, newExt, extLen);
    out[p + extLen] = '\0';

    DIR_LOG("[dir_change_extension]: exit -> %s", out);
    return out;
}


/**
 * @brief Normalises path separators and collapses "." / ".." segments.
 *
 * Output uses the native separator. The function does NOT resolve
 * symlinks; use dir_absolute_file_path for that. A leading absolute
 * marker (drive letter on Windows, leading slash on POSIX) is
 * preserved.
 *
 * @param path Input path. May be NULL.
 * @return Newly-allocated normalised path; caller must free().
 */
char* dir_normalize_path(const char* path) {
    if (!path) {
        return NULL;
    }

    char sep = dir_path_separator();
    size_t len = strlen(path);
    char* working = (char*)malloc(len + 2);
    if (!working) {
        return NULL;
    }
    memcpy(working, path, len + 1);

    char** stack = (char**)calloc(len + 1, sizeof(char*));
    if (!stack) { 
        free(working); 
        return NULL; 
    }

    size_t top = 0;

    bool isAbs = dir_is_absolute_path(path);
    size_t prefixLen = 0;
    char prefix[8] = {0};

#if defined(_WIN32) || defined(_WIN64)
    if (isAbs) {
        if (len >= 2 && path[1] == ':') {
            prefix[0] = path[0];
            prefix[1] = ':';
            prefix[2] = '\\';
            prefix[3] = '\0';
            prefixLen = 3;
        } 
        else {
            prefix[0] = '\\';
            prefix[1] = '\0';
            prefixLen = 1;
        }
    }
#else
    if (isAbs) {
        prefix[0] = '/';
        prefix[1] = '\0';
        prefixLen = 1;
    }
#endif

    char* p = working + prefixLen;
    while (*p) {
        while (*p == '\\' || *p == '/') {
            ++p;
        }
        if (!*p) {
            break;
        }

        char* segStart = p;
        while (*p && *p != '\\' && *p != '/') {
            ++p;
        }

        if (*p) { 
            *p = '\0'; 
            ++p; 
        }

        if (strcmp(segStart, ".") == 0 || *segStart == '\0') {
            continue;
        }
        if (strcmp(segStart, "..") == 0) {
            if (top > 0 && strcmp(stack[top - 1], "..") != 0) {
                --top;
            } 
            else if (!isAbs) {
                stack[top++] = segStart;
            }
            continue;
        }
        stack[top++] = segStart;
    }

    size_t outLen = prefixLen;
    for (size_t i = 0; i < top; ++i) {
        outLen += strlen(stack[i]) + 1;
    }
    if (outLen == 0) {
        outLen = 2;
    }

    char* out = (char*)malloc(outLen + 1);
    if (!out) { 
        free(working); 
        free(stack); 
        return NULL; 
    }

    size_t off = 0;
    memcpy(out + off, prefix, prefixLen);
    off += prefixLen;
    for (size_t i = 0; i < top; ++i) {
        size_t sl = strlen(stack[i]);
        memcpy(out + off, stack[i], sl);
        off += sl;

        if (i + 1 < top) {
            out[off++] = sep;
        }
    }
    if (off == 0) {
        out[off++] = '.';
    }
    out[off] = '\0';

    free(working);
    free(stack);
    return out;
}


/**
 * @brief Returns the path of the system temporary directory.
 *
 * Windows: GetTempPathW result (typically %TEMP%).
 * POSIX: $TMPDIR, or "/tmp" as a fallback.
 *
 * @return Newly-allocated path with no trailing separator. Caller must
 *         free(). NULL on failure.
 */
char* dir_temp_directory(void) {
#if defined(_WIN32) || defined(_WIN64)
    wchar_t wbuf[MAX_PATH + 1];
    DWORD n = GetTempPathW(MAX_PATH + 1, wbuf);

    if (n == 0 || n > MAX_PATH) {
        DIR_LOG("[dir_temp_directory] Error: GetTempPathW failed.");
        return NULL;
    }
    while (n > 0 && (wbuf[n - 1] == L'\\' || wbuf[n - 1] == L'/')) {
        wbuf[--n] = L'\0';
    }
    char* utf8 = encoding_wchar_to_utf8(wbuf);
    if (!utf8) {
        DIR_LOG("[dir_temp_directory] Error: utf-8 conversion failed.");
        return NULL;
    }
    return utf8;
#else
    const char* env = getenv("TMPDIR");
    if (!env || !*env) {
        env = "/tmp";
    }
    size_t len = strlen(env);

    while (len > 1 && env[len - 1] == '/') {
        --len;
    }
    char* out = (char*)malloc(len + 1);
    if (!out) {
        return NULL;
    }
    memcpy(out, env, len);
    out[len] = '\0';

    return out;
#endif
}


/**
 * @brief Recursive mkdir -p — creates every intermediate directory.
 *
 * Walks the path component by component and creates each directory
 * that does not already exist. Succeeds if the leaf directory exists
 * at the end (whether created by this call or pre-existing).
 *
 * @param dirpath Path to create. May contain '/' or '\\' separators.
 * @return true on success, false on failure at any level.
 */
bool dir_make_directories(const char* dirpath) {
    if (!dirpath || !*dirpath) {
        DIR_LOG("[dir_make_directories] Error: path is NULL or empty.");
        return false;
    }

    size_t len = strlen(dirpath);
    char* tmp = (char*)malloc(len + 1);
    if (!tmp) {
        return false;
    }
    memcpy(tmp, dirpath, len + 1);

    for (size_t i = 0; i < len; ++i) {
        if (tmp[i] == '\\') tmp[i] = '/';
    }
    while (len > 0 && tmp[len - 1] == '/') {
        tmp[--len] = '\0';
    }
    if (len == 0) { 
        free(tmp); 
        return true; 
    }

    size_t start = 0;
#if defined(_WIN32) || defined(_WIN64)
    if (len >= 2 && tmp[1] == ':') {
        start = 2;
    }
    if (len > start && tmp[start] == '/') {
        ++start;
    }
#else
    if (tmp[0] == '/') {
        start = 1;
    }
#endif

    for (size_t i = start; i <= len; ++i) {
        if (i == len || tmp[i] == '/') {
            char saved = tmp[i];
            tmp[i] = '\0';

            if (!dir_is_directory_exists(tmp)) {
                if (!dir_make_directory(tmp)) {
                    DIR_LOG("[dir_make_directories] Error: failed to create %s", tmp);
                    free(tmp);
                    return false;
                }
            }
            tmp[i] = saved;
        }
    }

    free(tmp);
    return true;
}


/* Walk into a directory and append every entry matching the option,
 * with paths prefixed by the directory being walked. */
/* True when `path` itself is a symbolic link (or, on Windows, any reparse
   point such as a junction), without following it. */
static bool dir_is_link(const char* path) {
#if defined(_WIN32) || defined(_WIN64)
    wchar_t* wPath = encoding_utf8_to_wchar(path);
    if (!wPath) {
        return false;
    }
    DWORD attrs = GetFileAttributesW(wPath);
    free(wPath);
    return attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0;
#else
    struct stat st;
    return lstat(path, &st) == 0 && S_ISLNK(st.st_mode);
#endif
}

static void dir_list_recursive_impl(const char* dirPath, DirListOption option, Vector* result) {
    Vector* entries = vector_create(sizeof(char*));
    if (!entries) {
        return;
    }
    dir_list_contents(dirPath, DIR_LIST_ALL, entries);

    size_t n = vector_size(entries);
    for (size_t i = 0; i < n; ++i) {
        char** namePtr = (char**)vector_at(entries, i);
        if (!namePtr || !*namePtr) {
            continue;
        }

        char* full = dir_join_path(dirPath, *namePtr);
        if (!full) { 
            free(*namePtr); 
            continue; 
        }

        bool isDir = dir_is_directory(full);
        if ((option == DIR_LIST_ALL) || (option == DIR_LIST_FILES && !isDir) || (option == DIR_LIST_DIRECTORIES && isDir)) {
            char* push = string_strdup(full);
            if (push) {
                vector_push_back(result, &push);
            }
        }
        /* Descend only into real directories. A symlink (or Windows junction)
           to a directory is listed but not followed: a link back to an
           ancestor made the walk loop until ELOOP (42 phantom entries for 2
           files), and a link to "/" walked the whole disk. */
        if (isDir && !dir_is_link(full)) {
            dir_list_recursive_impl(full, option, result);
        }
        free(full);
        free(*namePtr);
    }
    vector_deallocate(entries);
}


/**
 * @brief Walks a directory tree and collects every file / directory.
 *
 * The collected paths are prefixed with `dirPath` so callers can use
 * them directly with the other dir_* / file_* APIs. Each entry in the
 * result Vector is a heap-allocated char* — the caller is responsible
 * for freeing each one and the vector itself.
 *
 * @param dirPath Root directory to walk. Not added to the result.
 * @param option Filter: files, directories, or both.
 * @param result Output vector of `char*` (must already be created).
 */
void dir_list_recursive(const char* dirPath, DirListOption option, Vector* result) {
    if (!dirPath || !result) {
        DIR_LOG("[dir_list_recursive] Error: NULL argument.");
        return;
    }
    dir_list_recursive_impl(dirPath, option, result);
}
