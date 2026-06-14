# Dir Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview
This library is dedicated to directory and file operations, offering comprehensive functionality to manage and manipulate files and directories on the filesystem. It is designed to provide a straightforward and efficient way to interact with the filesystem in C programming.

## Key Features
- **Directory Operations**: Create, delete, rename, and navigate through directories.
- **File Operations**: Handle file creation, deletion, renaming, encryption, decryption, and more.
- **Directory Listing**: Retrieve lists of files and directories, including recursive searches.
- **File Information**: Obtain details about files and directories, such as size, type, owner, and timestamps.
- **Path Manipulation**: Work with absolute and relative paths, and convert between them.
- **File and Directory Search**: Search for files and directories based on patterns and conditions.
- **Error Handling**: Includes robust error handling for reliable file system operations.
- **Cross Platform**: Work in Windows and linux.
- **Support Unicode**: work with differnt kind of name file.

## Function Explanations

### `bool dir_make_directory(const char* dirpath)`
**Purpose**: Creates a single directory at the specified path. Handles directory creation differently based on the operating system (Windows or Unix-like systems).
**Parameters**:
- `dirpath`: A pointer to a string representing the path of the directory to be created.
**Return Value**: `true` if the directory is successfully created; `false` if creation fails (e.g., invalid permissions, null path, or the directory already exists).
**Usage Case**: Use when you need to create one new directory at a known path and the parent directory is guaranteed to already exist.

---

### `bool dir_make_directories(const char* dirpath)`
**Purpose**: Creates a full directory tree (equivalent to `mkdir -p`). Every intermediate directory in `dirpath` that does not already exist is created. Tolerates being called on a path whose tree already exists.
**Parameters**:
- `dirpath`: A pointer to a string representing the full path to create.
**Return Value**: `true` if the leaf directory exists at the end of the call; `false` on error.
**Usage Case**: Use when you need to atomically create a deep path such as `a/b/c/d` in one call, regardless of how many intermediate directories are missing.

---

### `bool dir_cd(const char* dirName)`
**Purpose**: Changes the current working directory to the specified directory.
**Parameters**:
- `dirName`: A string representing the name or path of the directory to change to.
**Return Value**: `true` if the directory was successfully changed; `false` on error (e.g., directory does not exist or is null).
**Usage Case**: Use to navigate into a subdirectory at runtime, similar to the `cd` shell command.

---

### `bool dir_cd_up(void)`
**Purpose**: Changes the current working directory to the parent directory (equivalent to `dir_cd("..")`).
**Parameters**: None.
**Return Value**: `true` if the directory change was successful; `false` otherwise.
**Usage Case**: Use to move up one level in the directory hierarchy without having to specify a full path.

---

### `bool dir_is_directory_exists(const char* dirPath)`
**Purpose**: Checks whether a directory exists at the specified path.
**Parameters**:
- `dirPath`: A string representing the path of the directory to check.
**Return Value**: `true` if the path exists and is a directory; `false` if it does not exist, is not a directory, or if an error occurs.
**Usage Case**: Use before attempting to open or iterate a directory to avoid errors from non-existent paths.

---

### `bool dir_is_file_exists(const char* filePath)`
**Purpose**: Checks whether a file exists at the specified path.
**Parameters**:
- `filePath`: A string representing the path of the file to check.
**Return Value**: `true` if the path exists and is a file; `false` if it does not exist or an error occurs.
**Usage Case**: Use before reading or processing a file to guard against missing-file errors.

---

### `bool dir_is_directory(const char* filePath)`
**Purpose**: Checks whether the specified path is a directory.
**Parameters**:
- `filePath`: A pointer to a string representing the path to check.
**Return Value**: `true` if the path is a directory; `false` if it is not a directory, does not exist, or an error occurs.
**Usage Case**: Use to distinguish directories from regular files when iterating directory contents.

---

### `bool dir_is_file(const char* filePath)`
**Purpose**: Checks whether the specified path points to a regular file.
**Parameters**:
- `filePath`: A pointer to a string specifying the path to check.
**Return Value**: `true` if the path points to a regular file; `false` if the path does not exist, points to something other than a regular file, or an error occurs.
**Usage Case**: Use to confirm that a path refers to a regular file (not a directory, symlink, etc.) before performing file I/O.

---

### `bool dir_is_empty(const char* dirName)`
**Purpose**: Checks whether a directory exists and contains no entries.
**Parameters**:
- `dirName`: A string representing the name or path of the directory to check.
**Return Value**: `true` if the directory is empty; `false` if it is not empty or an error occurs.
**Usage Case**: Use before calling `dir_remove_directory` to verify the directory is safe to delete.

---

### `bool dir_is_absolute_path(const char* path)`
**Purpose**: Determines whether a path is absolute. On Windows, recognises drive-letter prefixes (`C:\...`), UNC paths (`\\server\share`), and leading `/` or `\\`. On POSIX, recognises a leading `/`.
**Parameters**:
- `path`: A pointer to the path string to test.
**Return Value**: `true` if the path is absolute; `false` if it is relative or null.
**Usage Case**: Use to decide whether to prepend the current working directory when resolving a user-supplied path.

---

### `bool dir_is_readable(const char* path)`
**Purpose**: Reports whether the current process can read `path`. Uses `access(R_OK)` on POSIX and `_waccess` on Windows; a missing path returns `false`. (On Windows the result reflects the file's attributes rather than full ACLs — the platform's documented best-effort behavior.)
**Parameters**:
- `path`: A pointer to the path string to test.
**Return Value**: `true` if `path` exists and is readable; `false` otherwise (including a `NULL` path).
**Usage Case**: Pre-flight check before opening a config/data file, so you can fail early with a clear message instead of a cryptic open error.

---

### `bool dir_is_writable(const char* path)`
**Purpose**: Reports whether the current process can write to `path`. Uses `access(W_OK)` on POSIX and `_waccess` on Windows; a missing path returns `false`. (On Windows the result reflects the read-only attribute rather than full ACLs.)
**Parameters**:
- `path`: A pointer to the path string to test.
**Return Value**: `true` if `path` exists and is writable; `false` otherwise (including a `NULL` path).
**Usage Case**: Verify a target directory is writable before attempting to create files in it (e.g. choosing an output/cache location at startup).

---

### `DirFileType dir_get_file_type(const char* filePath)`
**Purpose**: Determines the type of the file system entry at the specified path.
**Parameters**:
- `filePath`: A pointer to a string representing the path to the file or directory.
**Return Value**: One of `DIR_FILE_TYPE_REGULAR`, `DIR_FILE_TYPE_DIRECTORY`, `DIR_FILE_TYPE_SYMLINK`, or `DIR_FILE_TYPE_UNKNOWN` if the type cannot be determined or the path is null.
**Usage Case**: Use when you need a single call to distinguish files, directories, and symbolic links without calling multiple predicate functions.

---

### `bool dir_remove_directory(const char* dirName)`
**Purpose**: Removes an empty directory. Fails if the directory is not empty.
**Parameters**:
- `dirName`: A string representing the name or path of the directory to remove.
**Return Value**: `true` if the directory was successfully removed; `false` if the directory is not empty or an error occurred.
**Usage Case**: Use for safe deletion of directories when you have already verified (or ensured) that the directory is empty.

---

### `bool dir_remove_directory_recursive(const char* dirPath)`
**Purpose**: Recursively removes a directory and all of its contents (files and subdirectories).
**Parameters**:
- `dirPath`: A string representing the path of the directory to be removed.
**Return Value**: `true` if the directory and its contents were successfully removed; `false` if an error occurred.
**Usage Case**: Use when you need to completely delete a directory tree, similar to `rm -rf`.

---

### `bool dir_rename(const char* oldName, const char* newName)`
**Purpose**: Renames a file or directory from `oldName` to `newName`.
**Parameters**:
- `oldName`: A string representing the current name of the file or directory.
- `newName`: A string representing the new name for the file or directory.
**Return Value**: `true` if the renaming was successful; `false` if it failed (e.g., either argument is null, or the underlying rename operation fails).
**Usage Case**: Use to rename files or directories in-place without copying and deleting.

---

### `bool dir_copy_file(const char* srcPath, const char* destPath)`
**Purpose**: Copies a file from `srcPath` to `destPath`.
**Parameters**:
- `srcPath`: The path of the source file to be copied.
- `destPath`: The destination path where the file should be copied to.
**Return Value**: `true` if the file was successfully copied; `false` if an error occurred.
**Usage Case**: Use to duplicate a single file to another location, preserving the original.

---

### `bool dir_copy_directory(const char* srcDir, const char* destDir)`
**Purpose**: Copies an entire directory tree, including all files and subdirectories, from `srcDir` to `destDir`.
**Parameters**:
- `srcDir`: The path of the source directory to be copied.
- `destDir`: The path of the destination directory.
**Return Value**: `true` if the directory and its contents were successfully copied; `false` if an error occurred.
**Usage Case**: Use to back up or duplicate a directory tree without removing the original.

---

### `bool dir_move_file(const char* srcPath, const char* destPath)`
**Purpose**: Moves a file from `srcPath` to `destPath`.
**Parameters**:
- `srcPath`: A pointer to a string representing the source file path.
- `destPath`: A pointer to a string representing the destination file path.
**Return Value**: `true` if the file was successfully moved; `false` if an error occurs.
**Usage Case**: Use to relocate a file to a different path, removing it from the original location.

---

### `bool dir_move_directory(const char* srcPath, const char* destPath)`
**Purpose**: Moves a directory and its contents from `srcPath` to `destPath`.
**Parameters**:
- `srcPath`: A pointer to a string representing the source directory path.
- `destPath`: A pointer to a string representing the destination directory path.
**Return Value**: `true` if the directory was successfully moved; `false` if an error occurs.
**Usage Case**: Use to relocate an entire directory tree to a new location in one operation.

---

### `void dir_list_contents(const char* dirPath, DirListOption option, Vector* result)`
**Purpose**: Lists the contents of a directory based on the specified filter option and stores the results in a `Vector` of heap-allocated `char*` strings.
**Parameters**:
- `dirPath`: The path of the directory to list.
- `option`: Filter specifying what to list: `DIR_LIST_FILES`, `DIR_LIST_DIRECTORIES`, or `DIR_LIST_ALL`.
- `result`: A `Vector*` where the results will be stored. Each element is a heap-allocated `char*` that the caller must `free`.
**Return Value**: `void`. Errors are handled internally via logging.
**Usage Case**: Use to enumerate the immediate children of a directory when you need only one level of contents.

---

### `void dir_list_recursive(const char* dirPath, DirListOption option, Vector* result)`
**Purpose**: Walks an entire directory tree and pushes every matching entry into `result` as a heap-allocated `char*`. Paths are prefixed with `dirPath`.
**Parameters**:
- `dirPath`: The root path to walk.
- `option`: Filter specifying what to collect: `DIR_LIST_FILES`, `DIR_LIST_DIRECTORIES`, or `DIR_LIST_ALL`.
- `result`: A `Vector*` where results are stored. Each element is a heap-allocated `char*` that the caller must `free`.
**Return Value**: `void`. Errors are handled internally via logging.
**Usage Case**: Use to collect all files (or directories) under a subtree for bulk processing such as archiving or indexing.

---

### `bool dir_search(const char* dirPath, const char* pattern, DirCompareFunc callback, void* userData)`
**Purpose**: Searches a directory for files matching a specified pattern, invoking a user-defined callback for each match.
**Parameters**:
- `dirPath`: The path of the directory to search.
- `pattern`: The glob pattern to match file names against (e.g., `"*.txt"`).
- `callback`: A function pointer `bool (*)(const char* filePath, void* userData)`. Return `true` to continue searching, `false` to stop.
- `userData`: Arbitrary pointer passed through to each callback invocation.
**Return Value**: `true` if the search completed without error; `false` if any argument is null or the directory cannot be opened.
**Usage Case**: Use to find all files of a given type in a directory and process or collect them in a single pass.

---

### `int dir_count(const char* dirpath)`
**Purpose**: Counts the number of entries (files and directories) in the specified directory.
**Parameters**:
- `dirpath`: A string representing the path to the directory.
**Return Value**: The number of entries in the directory, or `-1` if the directory cannot be opened or `dirpath` is null.
**Usage Case**: Use to quickly determine how many items a directory contains, e.g., to decide whether it is empty before deleting.

---

### `char dir_path_separator(void)`
**Purpose**: Returns the native path separator character for the current platform.
**Parameters**: None.
**Return Value**: `'\\'` on Windows; `'/'` on POSIX.
**Usage Case**: Use when building paths manually to avoid hard-coding a platform-specific separator character.

---

### `char* dir_join_path(const char* a, const char* b)`
**Purpose**: Joins two path segments with the native separator, trimming a trailing separator on `a` and a leading separator on `b`. If `b` is an absolute path it is returned verbatim (POSIX convention). The result is heap-allocated.
**Parameters**:
- `a`: The left-hand path segment.
- `b`: The right-hand path segment.
**Return Value**: A newly allocated string containing the joined path. The caller must call `free()` on it.
**Usage Case**: Use to construct file paths from components without manually inserting or deduplicating separators.

---

### `char* dir_base_name(const char* path)`
**Purpose**: Returns the final component of a path. Strips a trailing separator first, so `"foo/bar/"` returns `"bar"`.
**Parameters**:
- `path`: The input path string.
**Return Value**: A newly allocated string containing the final path component. The caller must call `free()` on it.
**Usage Case**: Use to extract the file or directory name from a full path, e.g., to display it without its parent directories.

---

### `char* dir_dir_name(const char* dirpath)`
**Purpose**: Returns the directory portion of a path (everything up to, but not including, the last component). If the input is `"."`, the current working directory name is returned.
**Parameters**:
- `dirpath`: A pointer to a string representing the directory path.
**Return Value**: A dynamically allocated string containing the directory name. The caller must call `free()` on it.
**Usage Case**: Use to strip the file name off a full path when you need only the parent directory.

---

### `char* dir_extension(const char* path)`
**Purpose**: Returns the file extension of the final path component, including the leading dot. A leading dot in the basename (e.g., `.bashrc`) is not treated as an extension.
**Parameters**:
- `path`: The input path string.
**Return Value**: A newly allocated string containing the extension (e.g., `".gz"`) or an empty string if there is none. The caller must call `free()` on it.
**Usage Case**: Use to determine a file's type from its name suffix, e.g., to select a parser or filter files by extension.

---

### `char* dir_change_extension(const char* path, const char* newExt)`
**Purpose**: Returns a copy of `path` with the extension replaced by `newExt`. `newExt` may include or omit the leading dot. Passing `""` strips the extension entirely.
**Parameters**:
- `path`: The original path string.
- `newExt`: The new extension to apply (e.g., `".bak"` or `"bak"`).
**Return Value**: A newly allocated string containing the modified path. The caller must call `free()` on it.
**Usage Case**: Use to generate a backup or converted-file name from an original file path.

---

### `char* dir_normalize_path(const char* path)`
**Purpose**: Cleans up a path by normalizing separators and collapsing `.` and `..` segments. Does not touch the filesystem (no symlink resolution). Output uses the native separator.
**Parameters**:
- `path`: The path string to normalize.
**Return Value**: A newly allocated string containing the normalized path. The caller must call `free()` on it.
**Usage Case**: Use to canonicalize user-supplied or concatenated paths before passing them to filesystem operations.

---

### `char* dir_absolute_file_path(const char* relative_path)`
**Purpose**: Resolves a relative path to its absolute form using the current working directory.
**Parameters**:
- `relative_path`: A string representing the relative file path.
**Return Value**: A dynamically allocated string containing the absolute file path, or `NULL` if an error occurs. The caller must call `free()` on it.
**Usage Case**: Use to turn a relative path (e.g., `"./config.ini"`) into an absolute path for storage or display.

---

### `char* dir_current_path(void)`
**Purpose**: Returns the current working directory's path.
**Parameters**: None.
**Return Value**: A dynamically allocated string containing the current working directory, or `NULL` on error. The caller must call `free()` on it.
**Usage Case**: Use to record or display where the program is currently running from.

---

### `char* dir_get_home_directory(void)`
**Purpose**: Returns the current user's home directory path. Supports both Windows and POSIX systems.
**Parameters**: None.
**Return Value**: A dynamically allocated string representing the home directory path, or `NULL` on error. The caller must call `free()` on it.
**Usage Case**: Use to locate user-specific configuration or data files stored under the home directory.

---

### `char* dir_temp_directory(void)`
**Purpose**: Returns the system temporary directory path with no trailing separator. On Windows uses `GetTempPathW`; on POSIX checks `$TMPDIR`, falling back to `/tmp`.
**Parameters**: None.
**Return Value**: A newly allocated string containing the temp directory path. The caller must call `free()` on it.
**Usage Case**: Use to obtain a safe location for writing temporary files that will be cleaned up later.

---

### `char* dir_get_creation_time(const char* dirPath)`
**Purpose**: Returns the creation time of a file or directory as a formatted string. Creation time retrieval is only supported on Windows; POSIX returns `NULL`.
**Parameters**:
- `dirPath`: A pointer to a string representing the path of the file or directory.
**Return Value**: A dynamically allocated string in `"YYYY-MM-DD HH:MM:SS"` format, or `NULL` on error or unsupported platform. The caller must call `free()` on it.
**Usage Case**: Use on Windows to record or display when a file or directory was first created.

---

### `char* dir_get_modified_time(const char* dirPath)`
**Purpose**: Returns the last-modified time of a file or directory as a formatted string. Supports both Windows and POSIX.
**Parameters**:
- `dirPath`: A pointer to a string representing the path of the file or directory.
**Return Value**: A dynamically allocated string in `"YYYY-MM-DD HH:MM:SS"` format, or `NULL` on error. The caller must call `free()` on it.
**Usage Case**: Use to detect whether a file has changed since it was last processed, or to display modification timestamps to users.

---

### `int64_t dir_get_modified_time_unix(const char* path)`
**Purpose**: Returns the modification time of a file or directory as **Unix epoch seconds** (seconds since 1970-01-01T00:00:00 UTC) — the machine-comparable counterpart to `dir_get_modified_time`, which returns a localized string. No allocation; works on both files and directories, on Windows and POSIX.
**Parameters**:
- `path`: A pointer to a string representing the path of the file or directory.
**Return Value**: The modification time in Unix seconds, or `-1` on a `NULL` path or if `path` cannot be `stat`'d (e.g. it does not exist).
**Usage Case**: Sort files by age, detect changes for cache invalidation/incremental builds, or compare two paths' ages with simple integer arithmetic.

---

### `long long dir_get_directory_size(const char* dirPath)`
**Purpose**: Calculates the total size of a directory and all its contents (including subdirectories) in bytes.
**Parameters**:
- `dirPath`: A pointer to a string representing the path of the directory.
**Return Value**: The total size in bytes, or `-1` if an error occurs (e.g., null path or inaccessible directory).
**Usage Case**: Use to report disk usage for a directory tree, or to enforce a storage quota.

---

### `long long dir_get_file_size(const char* filePath)`
**Purpose**: Returns the size of a specific file in bytes.
**Parameters**:
- `filePath`: A pointer to a string representing the path of the file.
**Return Value**: The size of the file in bytes, or `-1` if an error occurs (e.g., null path or the file does not exist).
**Usage Case**: Use to validate file sizes before reading, or to display file size information.

---

### `bool dir_get_file_owner(const char* filePath, char* ownerBuffer, size_t bufferSize)`
**Purpose**: Retrieves the name of the owner of the specified file and writes it into `ownerBuffer`.
**Parameters**:
- `filePath`: A pointer to a string representing the path of the file.
- `ownerBuffer`: A buffer where the owner's name will be stored.
- `bufferSize`: The size of `ownerBuffer` in bytes.
**Return Value**: `true` if the owner was successfully retrieved; `false` on error (e.g., null arguments, zero buffer size, or retrieval failure).
**Usage Case**: Use for access-control checks or audit logging that requires knowing who owns a given file.

---

### `bool dir_get_directory_owner(const char* dirPath, char* ownerBuffer, size_t bufferSize)`
**Purpose**: Retrieves the name of the owner of the specified directory and writes it into `ownerBuffer`.
**Parameters**:
- `dirPath`: A pointer to a string representing the path of the directory.
- `ownerBuffer`: A buffer where the owner's name will be stored.
- `bufferSize`: The size of `ownerBuffer` in bytes.
**Return Value**: `true` if the owner was successfully retrieved; `false` on error.
**Usage Case**: Use for permission audits or to display ownership metadata for directories.

---

### `bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv)`
**Purpose**: Encrypts the contents of a file in-place using DES-CBC mode. The encrypted data overwrites the original file.
**Parameters**:
- `filePath`: A pointer to a string representing the path to the file to be encrypted.
- `password`: The encryption password (must be 8 bytes or fewer).
- `iv`: A pointer to an 8-byte initialization vector for CBC mode.
**Return Value**: `true` if the file was successfully encrypted; `false` if any argument is null, the file cannot be opened, or encryption fails.
**Usage Case**: Use to protect sensitive files on disk with a password before storing or transmitting them.

---

### `bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv)`
**Purpose**: Decrypts the contents of a file in-place using DES-CBC mode. The decrypted data overwrites the encrypted file.
**Parameters**:
- `filePath`: A pointer to a string representing the path to the file to be decrypted.
- `password`: The decryption password (must be 8 bytes or fewer; must match the password used for encryption).
- `iv`: A pointer to the same 8-byte initialization vector used during encryption.
**Return Value**: `true` if the file was successfully decrypted; `false` if any argument is null, the file cannot be opened, or decryption fails.
**Usage Case**: Use to restore a previously encrypted file to its original readable form.

---

## Compilation
To compile a program using the Dir library, include all necessary source files in your compilation command
This command compiles with GCC using the C17 standard and enables various optimizations and warnings for better performance and code quality.

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./dir/dir.c
```

---
### Examples 

## Example 1 : create directory with `dir_make_directory`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dir_name_utf8 = "./کایزن";
    bool result = dir_make_directory(dir_name_utf8);

    if (result) {
        fmt_printf("Directory %s created successfully.\n", "کایزن");
    } 
    else {
        fmt_printf("Failed to create directory '%s'.\n", dir_name_utf8);
    }
    return 0;
}
```
**Result**
```
Directory کایزن created successfully.
```

---

## Example 2 : get directory name with `dir_dir_name`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* dir_path = "./کایزن";
    char* dirName = dir_dir_name(dir_path);

    if (dirName) {
        fmt_printf("Directory name: '%s'\n", dirName);
        free(dirName);
    } 
    else {
        fmt_printf("Failed to get directory name.\n");
    }
    return 0;
}
```
**Result**
```
Directory name: 'کایزن'
```

---

## Example 3 : how to get current_path with `dir_current_path`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* currentPath = dir_current_path();

    if (currentPath) {
        fmt_printf("Current directory path: '%s'\n", currentPath);
        free(currentPath);
    } 
    else {
        fmt_printf("Failed to get current directory path.\n");
    }
    return 0;
}
```
**Result**
```
Current directory path: 'C:\Users\amint\Desktop\c_std'
```
> Note: Output is machine-specific; the exact path reflects the project directory on the current machine.

---

## Example 4 : count any things in path with `dir_count`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* dir_path = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std"; // Current directory
    int fileCount = dir_count(dir_path);

    if (fileCount >= 0) {
        fmt_printf("Total number of directories and files in '%s': %d\n", dir_path, fileCount);
    } 
    else {
        fmt_printf("Failed to count files and directories in '%s'.\n", dir_path);
    }
    return 0;
}
```
**Result**
```
Total number of directories and files in 'C:\Users\Science\Desktop\projects\C\c_std': <N>
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 5 : get absolute_path of file with `dir_absolute_path_file`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* relativePath = "./کایزن/amin.txt"; // .,/ or any path
    char* absolutePath = dir_absolute_file_path(relativePath);

    if (absolutePath) {
        fmt_printf("Absolute file path: '%s'\n", absolutePath);
        free(absolutePath);
    } 
    else {
        fmt_printf("Failed to get absolute file path.\n");
    }
    return 0;
}
```
**Result**
```
Absolute file path: 'C:\Users\amint\Desktop\c_std\کایزن\amin.txt'
```
> Note: Output is machine-specific; the prefix path reflects the project directory on the current machine.

---

## Example 6 : change directory or move up with `dir_cd` and `dir_cd_up`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* cd = "date";

    if (dir_cd(cd)) {
        fmt_printf("Changed directory to 'date'\n");
        char* currentPath = dir_current_path();

        if (currentPath) {
            fmt_printf("Current directory path: '%s'\n", currentPath);
            free(currentPath);
        } 
        else {
            fmt_printf("Failed to get current directory path.\n");
        }
    } 
    else {
        fmt_printf("Failed to change directory.\n");
    }

    if (dir_cd_up()) {
        fmt_printf("Moved up one directory.\n");
        char* currentPath = dir_current_path();
        if (currentPath) {
            fmt_printf("Current directory path: '%s'\n", currentPath);
            free(currentPath);
        } 
        else {
            fmt_printf("Failed to get current directory path.\n");
        }
    } 
    else {
        fmt_printf("Failed to move up directory.\n");
    }
    return 0;
}
```
**Result**
```
Changed directory to 'date'
Current directory path: 'C:\Users\amint\Desktop\c_std\date'
Moved up one directory.
Current directory path: 'C:\Users\amint\Desktop\c_std'
```
> Note: Output is machine-specific; the `date` subdirectory must exist for `dir_cd` to succeed, and all paths reflect the current machine.

---

## Example 7 : check if directory is empty delete dir `dir_remove_directory` and `dir_is_empty`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirName = "C:\\Users\\asus\\OneDrive\\Desktop\\stack\\کایزن";

    if (dir_is_empty(dirName)) {
        fmt_printf("Directory '%s' is empty.\n", dirName);
        
        if (dir_remove_directory(dirName)) {
            fmt_printf("Successfully removed directory '%s'.\n", dirName);
        } 
        else {
            fmt_printf("Failed to remove directory '%s'.\n", dirName);
        }
    } 
    else {
        fmt_printf("Directory '%s' is not empty, cannot remove.\n", dirName);
    }
    return 0;
}
```
**Result**
```
Successfully removed directory 'C:\Users\asus\OneDrive\Desktop\stack\کایزن'.
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 8: remove dir recursivly with `dir_remove_directory_recursive`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirName = "C:\\Users\\asus\\OneDrive\\Desktop\\stack\\کایزن";

    if (dir_remove_directory_recursive(dirName)) {
        fmt_printf("Successfully removed directory '%s'.\n", dirName);
    } 
    else {
        fmt_printf("Failed to remove directory '%s'.\n", dirName);
    }
    return 0;
}
```
**Result**
```
Successfully removed directory 'C:\Users\asus\OneDrive\Desktop\stack\کایزن'.
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 9: Renaming a File with `dir_rename`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* oldFileName = "./tester.txt";
    const char* newFileName = "./amin.txt";

    if (dir_rename(oldFileName, newFileName)) {
        fmt_printf("File renamed successfully from '%s' to '%s'.\n", oldFileName, newFileName);
    } 
    else {
        fmt_printf("Failed to rename file '%s'.\n", oldFileName);
    }
    return 0;
}
```
**Result**
```
File renamed successfully from './tester.txt' to './amin.txt'.
```
> Note: Output is machine-specific; the source file `./tester.txt` must exist on the local filesystem.

---

## Example 10: Renaming a Directory with `dir_rename`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* oldDirName = "./one";
    const char* newDirName = "./کایزن";

    if (dir_rename(oldDirName, newDirName)) {
        fmt_printf("Directory renamed successfully from '%s' to '%s'.\n", oldDirName, newDirName);
    } 
    else {
        fmt_printf("Failed to rename directory '%s'.\n", oldDirName);
    }
    return 0;
}
```
**Result**
```
Directory renamed successfully from './one' to './کایزن'.
```
> Note: Output is machine-specific; the source directory `./one` must exist on the local filesystem.

---

## Example 11 : check file exists or not `dir_is_file_exists`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "./کایزن/amin.txt"; // Replace with the directory path you want to check

    if (dir_is_file_exists(dirPath)) {
        fmt_printf("File '%s' exists.\n", dirPath);
    } 
    else {
        fmt_printf("File '%s' does not exist.\n", dirPath);
    }
    return 0;
}
```
**Result**
```
File './کایزن/amin.txt' does not exist.
```

---

## Example 12 : check directory is exists or not `dir_is_directory_exists`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "./کایزن"; // Replace with the directory path you want to check

    if (dir_is_directory_exists(dirPath)) {
        fmt_printf("Directory '%s' exists.\n", dirPath);
    } 
    else {
        fmt_printf("Directory '%s' does not exist.\n", dirPath);
    }
    return 0;
}
```
**Result**
```
Directory './کایزن' does not exist.
```

---

## Example 13 : copy dir and its contents to new location with `dir_copy_directory`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourceDir = "./کایزن"; // Replace with your source directory path
    const char* destinationDir = "C:\\Users\\asus\\OneDrive\\Desktop"; // Replace with your destination directory path

    if (dir_make_directory("C:\\Users\\asus\\OneDrive\\Desktop\\کایزن")) {
        if (dir_copy_directory(sourceDir, destinationDir)) {
            fmt_printf("Directory copied successfully from '%s' to '%s'.\n", sourceDir, destinationDir);
        } 
        else {
            fmt_printf("Failed to copy directory from '%s' to '%s'.\n", sourceDir, destinationDir);
        }
    } 
    else {
        fmt_printf("Can not create folder");
    }
    return 0;
}
```
**Result**
```
Directory copied successfully from './کایزن' to 'C:\Users\asus\OneDrive\Desktop'.
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 14 : copy file into new location `dir_copy_file`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourceFile = "./کایزن/amin.txt"; // Replace with your source file path
    const char* destinationFile = "C:\\Users\\Science\\Desktop\\amin.txt"; // Replace with your destination file path

    if (dir_copy_file(sourceFile, destinationFile)) {
        fmt_printf("File copied successfully from '%s' to '%s'.\n", sourceFile, destinationFile);
    } 
    else {
        fmt_printf("Failed to copy file from '%s' to '%s'.\n", sourceFile, destinationFile);
    }
    return 0;
}
```
**Result**
```
File copied successfully from './کایزن/amin.txt' to 'C:\Users\asus\OneDrive\Desktop\amin.txt'.
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 15 :  Calculate the total size of a directory, including all files and subdirectories `dir_get_directory_size`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "./کایزن"; 
    long long size = dir_get_directory_size(dirPath);

    // Check for errors
    if (size == -1) {
        fmt_printf("Failed to calculate the size of directory '%s'.\n", dirPath);
    } 
    else {
        fmt_printf("Total size of directory '%s' is: %lld bytes.\n", dirPath, size);
    }
    return 0;
}
```
**Result**
```
Total size of directory './کایزن' is: 10 bytes.
```
> Note: Output is machine-specific; the size depends on the actual files inside the directory.

---

## Example 16 : Calculating the total size of file with `dir_get_file_size`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* filePath = "./کایزن/amin.txt"; 
    long long size = dir_get_file_size(filePath);

    // Check for errors
    if (size == -1) {
        fmt_printf("Failed to calculate the file size '%s'.\n", filePath);
    } 
    else {
        fmt_printf("Total size of file '%s' is: %lld bytes.\n", filePath, size);
    }
    return 0;
}
```
**Result**
```
Total size of file './کایزن/amin.txt' is: 10 bytes.
```
> Note: Output is machine-specific; the size depends on the actual content of the file.

---

## Example 17 : get list of director or files or both with `dir_list_contents`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(){
    const char *dirPath = "C:\\Users\\Science\\Desktop\\";
    Vector *foundItems = vector_create(sizeof(char *));

    dir_list_contents(dirPath, DIR_LIST_ALL, foundItems);

    for (size_t i = 0; i < vector_size(foundItems); ++i){
        char **item = (char **)vector_at(foundItems, i);
        fmt_printf("%s\n", *item);
        free(*item); // Remember to free the duplicated string 
    }

    vector_deallocate(foundItems);
    return 0;
}
```
**Result**
```
Dark Souls II Scholar of the First Sin.lnk
DARK SOULS REMASTERED.lnk
desktop.ini
Docker Desktop.lnk
ELDEN RING Shadow of the Erdtree.lnk
Lively Wallpaper.lnk
MayaVPN.lnk
Mirage.lnk
project
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 18 : check path is dir or file with `dir_is_file` or `dir_is_directory`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\main.c"; 

    // also you can set another path for checking dir
    if (dir_is_file(dirPath)) {
        fmt_printf("Yes \"%s\" is path", dirPath);
    }
    else {
        fmt_printf("No it is\n");
    }
    return 0;
}
```
**Result**
```
Yes "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\main.c" is path
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 19 : move file and directoy with these `dir_move_directory` or `dir_move_file`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";
    const char* destPath = "C:\\Users\\Science\\Desktop\\one";

    // also for moving files you can use "dir_move_file" 
    if (dir_move_directory(sourcePath, destPath)) {
        fmt_printf("Move to new location");
    }
    else {
        fmt_printf("not Move\n");
    }
    return 0;
}
```
**Result**
```
Move to new location
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 20 : get last modified file and dir with `dir_get_modified_time`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\new_one.txt";

    fmt_printf("Last Modified Time is %s\n", dir_get_modified_time(sourcePath));

    return 0;
}
```
**Result**
```
Last Modified Time is 2024-11-28 21:49:45
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 21 : get last creation file and dir with `dir_get_creation_time`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\new_one.txt";

    fmt_printf("Creation Time is %s\n", dir_get_creation_time(sourcePath));

    return 0;
}
```
**Result**
```
Creation Time is 2024-11-26 13:46:42
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 22 : get home dir with `dir_get_home_directory`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    char* homeDirectory = dir_get_home_directory();

    if (homeDirectory != NULL) {
        fmt_printf("Home Directory: %s\n", homeDirectory);
        free(homeDirectory);
    } 
    else {
        fmt_printf("Failed to get home directory.\n");
    }
    return 0;
}
```
**Result**
```
Home Directory: C:\Users\amint
```

---

## Example 23 : get file type with `dir_get_file_type`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    DirFileType type = dir_get_file_type("./dir/dir.h");
    switch (type) {
        case DIR_FILE_TYPE_REGULAR:
            fmt_printf("Regular File\n");
            break;
        case DIR_FILE_TYPE_DIRECTORY:
            fmt_printf("Directory\n");
            break;
        case DIR_FILE_TYPE_SYMLINK:
            fmt_printf("Symbolic Link\n");
            break;
        default:
            fmt_printf("Unknown Type\n");
    }
    return 0;
}
```
**Result**
```
Regular File
```

---

## Example 24 : encrypt and decrypt file with des mode cbc `dir_encrypt_file` and `dir_decrypt_file`

```c
#include "dir/dir.h"
#include "crypto/crypto.h"
#include "fmt/fmt.h"

int main() {
    const char* filePath = "C:\\Users\\Science\\Desktop\\new_one.txt";
    const char* password = "aminamin";
    uint8_t iv[DES_BLOCK_SIZE]; // IV for CBC mode

    crypto_generate_random_iv(iv, DES_BLOCK_SIZE); 

    if (dir_encrypt_file(filePath, password, iv)) {
        fmt_printf("File encrypted successfully.\n");
    } 
    else {
        fmt_printf("Failed to encrypt the file.\n");
    }

    if (dir_decrypt_file(filePath, password, iv)) {
        fmt_printf("File decrypted successfully.\n");
    } 
    else {
        fmt_printf("Failed to decrypt the file.\n");
    }
    return 0;
}
```
**Result**
```
File encrypted successfully.
File decrypted successfully.
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 25 : get file owner with `dir_get_file_owner`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* filePath = "C:\\Users\\Science\\Desktop\\new_one.txt";
    char ownerBuffer[256];  // Buffer to store the owner's name

    if (dir_get_file_owner(filePath, ownerBuffer, sizeof(ownerBuffer))) {
        fmt_printf("The owner of '%s' is: %s\n", filePath, ownerBuffer);
    } 
    else {
        fmt_printf("Failed to retrieve the file owner for '%s'\n", filePath);
    }
    return 0;
}
```
**Result**
```
The owner of 'C:\\Users\\Science\\Desktop\\new_one.txt' is: asus
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 26 : get dir owner with `dir_get_directory_owner`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "C:\\Users\\Science\\Desktop\\one";
    char ownerBuffer[256];  // Buffer to store the owner's name

    if (dir_get_directory_owner(dirPath, ownerBuffer, sizeof(ownerBuffer))) {
        fmt_printf("The owner of '%s' is: %s\n", dirPath, ownerBuffer);
    } 
    else {
        fmt_printf("Failed to retrieve the file owner for '%s'\n", dirPath);
    }
    return 0;
}
```
**Result**
```
The owner of 'C:\\Users\\Science\\Desktop\\one' is: asus
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 27 : search file in path `dir_search`

```c
#include "dir/dir.h"
#include "string/std_string.h"
#include "fmt/fmt.h"
#include <stdlib.h>

bool append_file_name_to_vector(const char* filePath, void* userData) {
    Vector* vector = (Vector*)userData;
    char* filePathCopy = string_strdup(filePath); // Duplicate the file path

    if (!filePathCopy) {
        fmt_fprintf(stderr, "Failed to duplicate file path");
        return false;
    }
    vector_push_back(vector, &filePathCopy);
    return true; // Continue the search
}

int main() {
    const char* dirPath = "C:\\Users\\Science\\Desktop\\"; // Replace with your directory path
    const char* pattern = "*.txt"; // Search pattern, e.g., all .txt files 
    Vector* foundFiles = vector_create(sizeof(char*)); 

    if (!dir_search(dirPath, pattern, append_file_name_to_vector, foundFiles)) {
        fmt_fprintf(stderr, "An error occurred during the search.\n");
    } 
    else {
        for (size_t i = 0; i < vector_size(foundFiles); ++i) {
            char** filePath = (char**)vector_at(foundFiles, i);
            fmt_printf("Found file: %s\n", *filePath);
            free(*filePath); // Free the duplicated file path
        }
    }

    vector_deallocate(foundFiles);
    return 0;
}
```
**Result**
```
Found file: CMakeLists.txt
Found file: destination_file.txt
Found file: locked_file.txt
Found file: log.txt
Found file: new_log.txt
Found file: new_log_file.txt
Found file: old_log.txt
Found file: output.txt
Found file: test_uni.txt
Found file: text_uni.txt
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 28 : get list of contents `dir_list_contents`

```c
#include "dir/dir.h"
#include "algorithm/algorithm.h"
#include "fmt/fmt.h"

void print_char(void *element) {
    char** strPtr = (char**)element;
    fmt_printf("%s\n", *strPtr);
}

int main(){
    const char *dirPath = "C:\\Users\\Science\\Desktop\\";
    Vector *foundItems = vector_create(sizeof(char *));

    dir_list_contents(dirPath, DIR_LIST_ALL, foundItems);
    algorithm_for_each(vector_begin(foundItems), vector_size(foundItems), sizeof(char*), print_char);
    
    vector_deallocate(foundItems);
    return 0;
}
```
**Result**
```
amin.txt
CapCut.lnk
Dark Souls II Scholar of the First Sin.lnk
DARK SOULS REMASTERED.lnk
desktop.ini
Docker Desktop.lnk
ELDEN RING Shadow of the Erdtree.lnk
Lively Wallpaper.lnk
Mirage.lnk
one
project
Project1
Silent Hill 2 Remake.lnk
```
> Note: Output is machine-specific; exact path/content depends on the local filesystem.

---

## Example 29 — `dir_path_separator`, `dir_is_absolute_path`, `dir_join_path`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    fmt_printf("native separator = '%c'\n", dir_path_separator());

    fmt_printf("is_absolute(\"foo\")        = %d\n", dir_is_absolute_path("foo"));
    fmt_printf("is_absolute(\"/etc\")       = %d\n", dir_is_absolute_path("/etc"));
    fmt_printf("is_absolute(\"C:\\\\Users\") = %d\n", dir_is_absolute_path("C:\\Users"));

    char* p1 = dir_join_path("logs",       "2026/05.txt");
    char* p2 = dir_join_path("trailing/",  "no-leading");

    /* Right-hand path is absolute: returned verbatim. */
    char* p3 = dir_join_path("ignored", "C:\\absolute");

    fmt_printf("join: %s\n", p1);
    fmt_printf("join: %s\n", p2);
    fmt_printf("join: %s\n", p3);

    free(p1);
    free(p2);
    free(p3);
    return 0;
}
```

**Result**

```
native separator = '\'
is_absolute("foo")        = 0
is_absolute("/etc")       = 1
is_absolute("C:\\Users") = 1
join: logs\2026/05.txt
join: trailing\no-leading
join: C:\absolute
```

---

## Example 30 — `dir_base_name`, `dir_extension`, `dir_change_extension`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

static void show(const char* path) {
    char* base = dir_base_name(path);
    char* ext  = dir_extension(path);
    char* swap = dir_change_extension(path, ".bak");
    fmt_printf("%-30s base=%-15s ext=%-8s swap=%s\n", path, base, ext, swap);
    free(base); free(ext); free(swap);
}

int main(void) {
    show("/usr/local/bin/gcc");
    show("C:\\Users\\amin\\notes.tar.gz");
    show("Makefile");
    show("/etc/.bashrc");
    show("plain.txt");
    return 0;
}
```

**Result**

```
/usr/local/bin/gcc             base=gcc             ext=         swap=/usr/local/bin/gcc.bak
C:\Users\amin\notes.tar.gz     base=notes.tar.gz    ext=.gz      swap=C:\Users\amin\notes.tar.bak
Makefile                       base=Makefile        ext=         swap=Makefile.bak
/etc/.bashrc                   base=.bashrc         ext=         swap=/etc/.bashrc.bak
plain.txt                      base=plain.txt       ext=.txt     swap=plain.bak
```

---

## Example 31 — `dir_normalize_path`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    const char* samples[] = {
        "a//b///c",
        "a/b/../c",
        "./logs/././today.txt",
        "C:\\Users\\..\\Program Files",
    };
    for (size_t i = 0; i < sizeof(samples)/sizeof(samples[0]); ++i) {
        char* n = dir_normalize_path(samples[i]);
        fmt_printf("%-32s -> %s\n", samples[i], n);
        free(n);
    }
    return 0;
}
```

**Result**

```
a//b///c                         -> a\b\c
a/b/../c                         -> a\c
./logs/././today.txt             -> logs\today.txt
C:\Users\..\Program Files        -> C:\Program Files
```

---

## Example 32 — `dir_temp_directory` + `dir_make_directories`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    char* tmp = dir_temp_directory();
    fmt_printf("system temp dir: %s\n", tmp);

    /* Build a deep tree atomically — every intermediate dir is created. */
    char* deep = dir_join_path(tmp, "demo/2026/05/24/snapshots");
    fmt_printf("creating: %s\n", deep);

    if (dir_make_directories(deep)) {
        fmt_printf("created OK, leaf exists = %d\n", dir_is_directory_exists(deep));
    } else {
        fmt_printf("creation failed\n");
    }

    /* Calling it again on an existing tree is a no-op success. */
    fmt_printf("idempotent call: %d\n", dir_make_directories(deep));

    /* Clean up the whole tree. */
    char* root = dir_join_path(tmp, "demo");
    dir_remove_directory_recursive(root);

    free(deep); 
    free(root); 
    free(tmp);
    return 0;
}
```

**Result**

```
system temp dir: C:\Users\amint\AppData\Local\Temp
creating: C:\Users\amint\AppData\Local\Temp\demo/2026/05/24/snapshots
created OK, leaf exists = 1
idempotent call: 1
```
> Note: The `creating:` line shows the raw joined path; forward slashes within the second argument to `dir_join_path` are preserved as-is (only the junction separator is normalized). The temp directory prefix is machine-specific.

---

## Example 33 — `dir_list_recursive` (walk a whole tree)

```c
#include "dir/dir.h"
#include "vector/vector.h"
#include "fmt/fmt.h"
#include "file_io/file_writer.h"
#include <stdlib.h>
#include <string.h>

static void touch(const char* p, const char* s) {
    FileWriter* w = file_writer_open(p, WRITE_BINARY);

    if (w) { 
      file_writer_write((char*)s, 1, strlen(s), w); file_writer_close(w); 
    }
}

int main(void) {
    /* Build a small tree under temp. */
    char* tmp     = dir_temp_directory();
    char* root    = dir_join_path(tmp, "walk_demo");
    char* sub_a   = dir_join_path(root, "a");
    char* sub_a_b = dir_join_path(root, "a/b");

    dir_make_directories(sub_a_b);

    char* f1 = dir_join_path(root,    "top.txt");
    char* f2 = dir_join_path(sub_a,   "mid.txt");
    char* f3 = dir_join_path(sub_a_b, "deep.txt");

    touch(f1, "1"); 
    touch(f2, "2"); 
    touch(f3, "3");

    Vector* files = vector_create(sizeof(char*));
    dir_list_recursive(root, DIR_LIST_FILES, files);

    fmt_printf("files (%zu):\n", vector_size(files));
    for (size_t i = 0; i < vector_size(files); ++i) {
        char** p = (char**)vector_at(files, i);
        fmt_printf("  %s\n", *p);
        free(*p);
    }

    vector_deallocate(files);
    dir_remove_directory_recursive(root);
    free(f1); 
    free(f2); 
    free(f3);
    free(sub_a); 
    free(sub_a_b);
    free(root); 
    free(tmp);

    return 0;
}
```

**Result**

```
files (3):
  C:\Users\amint\AppData\Local\Temp\walk_demo\top.txt
  C:\Users\amint\AppData\Local\Temp\walk_demo\a\mid.txt
  C:\Users\amint\AppData\Local\Temp\walk_demo\a\b\deep.txt
```
> Note: Output is machine-specific; the temp directory prefix reflects the current machine.

---

## Example 34 — Production pre-flight: `dir_is_writable`, `dir_is_readable`, `dir_get_modified_time_unix`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char* tmp = dir_temp_directory();

    /* Pre-flight: only proceed if the target directory is writable. */
    if (!dir_is_writable(tmp)) {
        fmt_printf("temp dir not writable, aborting\n");
        free(tmp);
        return 1;
    }
    fmt_printf("temp dir writable: yes\n");

    /* Create a file, then inspect its access flags and modification time. */
    char* path = dir_join_path(tmp, "dir_prod_demo.txt");
    FILE* f = fopen(path, "wb");
    if (f) { fputs("data", f); fclose(f); }

    fmt_printf("file readable: %s\n", dir_is_readable(path) ? "yes" : "no");
    fmt_printf("file writable: %s\n", dir_is_writable(path) ? "yes" : "no");

    int64_t mt = dir_get_modified_time_unix(path);
    fmt_printf("mtime is a positive unix timestamp: %s\n", mt > 0 ? "yes" : "no");

    /* A missing file reports -1 / not readable. */
    char* gone = dir_join_path(tmp, "no_such_file.txt");
    fmt_printf("missing file mtime: %lld\n", (long long)dir_get_modified_time_unix(gone));
    fmt_printf("missing file readable: %s\n", dir_is_readable(gone) ? "yes" : "no");

    remove(path);
    free(path);
    free(gone);
    free(tmp);
    return 0;
}
```
**Result**
```
temp dir writable: yes
file readable: yes
file writable: yes
mtime is a positive unix timestamp: yes
missing file mtime: -1
missing file readable: no
```
> **Why it matters:** `dir_is_writable` lets a service fail early with a clear message instead of a cryptic open error, and `dir_get_modified_time_unix` returns a machine-comparable integer (unlike the localized string from `dir_get_modified_time`) for change-detection, cache invalidation, and sort-by-age.

---

## License

This project is open-source and available under [ISC License].
