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

- **Purpose**: This function attempts to create a directory at the specified path. It handles directory creation differently based on the operating system (Windows or Unix-like systems).
- **Parameters**:
  - `dirpath`: A pointer to a string representing the path of the directory to be created.
- **Return Value**: 
  - `true`: If the directory is successfully created.
  - `false`: If the directory creation fails for any reason, such as invalid permissions or a null directory path.

- **Input Validation**:
   - The function first checks if the provided `dirpath` is `NULL`. If it is, the function logs an error message and returns `false`.

---

### `char* dir_dir_name(const char* dirpath)`

- **Purpose**: This function retrieves the name of the directory from a given directory path. If the input is the current directory (`"."`), it will return the name of the current working directory.
- **Parameters**:
  - `dirpath`: A pointer to a string representing the directory path.
- **Return Value**: 
  - A dynamically allocated string containing the directory name, or an empty string if an error occurs. The caller is responsible for freeing this memory.
- **Input Validation**:
   - The function first checks if the provided `dirpath` is `NULL`. If so, it logs an error and returns `NULL`.

---

### `char* dir_current_path(void)`

- **Purpose**: This function retrieves the current working directory's path.
- **Parameters**: None.
- **Return Value**: 
  - A dynamically allocated string containing the current working directory, or `NULL` on error. The caller is responsible for freeing this memory.

---

### `int dir_count(const char* dirpath)`

- **Purpose**: This function counts the number of items (files and directories) in a specified directory.
- **Parameters**:
  - `dirpath`: A string representing the path to the directory.
- **Return Value**:
  - Returns the number of items in the directory, or `-1` if an error occurs (e.g., if the directory cannot be opened).

- **Input Validation**:
   - The function first checks if the `dirpath` is `NULL`. If so, it logs an error and returns `-1`.

---

### `char* dir_absolute_file_path(const char* relative_path)`

- **Purpose**: This function converts a relative file path to an absolute file path.
- **Parameters**:
  - `relative_path`: A string representing the relative file path.
- **Return Value**:
  - Returns a dynamically allocated string containing the absolute file path, or `NULL` if an error occurs.
- **Input Validation**:
   - The function checks if the `relative_path` is `NULL`. If so, it logs an error and returns `NULL`.

---

### `bool dir_cd(const char* dirName)`

- **Purpose**: This function changes the current working directory to the specified directory.
- **Parameters**:
  - `dirName`: A string representing the name of the directory to change to.
- **Return Value**:
  - Returns `true` if the directory was successfully changed, or `false` if an error occurs.

- **Input Validation**:
   - The function checks if the `dirName` is `NULL`. If so, it logs an error and returns `false`.

---

### `bool dir_cd_up()`

- **Purpose**: This function changes the current working directory to the parent directory (i.e., the directory one level up from the current directory).
- **Parameters**: None.
- **Return Value**: 
  - Returns `true` if the directory change was successful, or `false` otherwise.

- **Logging**: The function logs that it is changing the directory to the parent (`..`).
- **Calling `dir_cd`**: It calls the `dir_cd` function with `".."` as the argument, which changes the directory to the parent directory.
- **Return**: It returns the result of the `dir_cd` function.

---

### `bool dir_remove_directory(const char* dirName)`

- **Purpose**: This function removes a directory if it is empty.
- **Parameters**:
  - `dirName`: A string representing the name or path of the directory to remove.
- **Return Value**: 
  - Returns `true` if the directory was successfully removed, or `false` if the directory is not empty or an error occurred.

- **Input Validation**: It first checks if the directory name (`dirName`) is `NULL`. If so, it logs an error and returns `false`.
- **Checking Directory Contents**: It calls the `dir_is_empty` function to check if the directory is empty. If the directory is not empty, it logs an error and returns `false`.

---

### `bool dir_is_empty(const char* dirName)`

- **Purpose**: This function checks if a directory is empty.
- **Parameters**:
  - `dirName`: A string representing the name or path of the directory to check.
- **Return Value**: 
  - Returns `true` if the directory is empty, `false` if it is not empty or if an error occurs.
- **Input Validation**: It checks if the directory name is `NULL`. If so, it logs an error and returns `false`.

---

### `bool dir_remove_directory_recursive(const char* dirPath)`

- **Purpose**: This function recursively removes a directory and all its contents.
- **Parameters**:
  - `dirPath`: A string representing the path of the directory to be removed.
- **Return Value**: 
  - Returns `true` if the directory and its contents were successfully removed, or `false` if an error occurred.

- **Input Validation**: It checks if the directory path is `NULL`. If so, it logs an error and returns `false`.

---

### `bool dir_rename(const char* oldName, const char* newName)`

- **Purpose**: This function renames a file or directory from `oldName` to `newName`.
- **Parameters**:
  - `oldName`: A string representing the current name of the file or directory.
  - `newName`: A string representing the new name for the file or directory.
- **Return Value**:
  - Returns `true` if the renaming was successful, or `false` if it failed.

- **Input Validation**: The function first checks if both `oldName` and `newName` are provided. If either is `NULL`, it logs an error and returns `false`.

---

### `bool dir_is_directory_exists(const char* dirPath)`

- **Purpose**: This function checks if a directory exists at the specified path.
- **Parameters**:
  - `dirPath`: A string representing the path of the directory to check.
- **Return Value**:
  - Returns `true` if the directory exists, or `false` if it does not exist or if an error occurs.

- **Input Validation**: It checks if `dirPath` is `NULL`. If so, it logs an error and returns `false`.

---

### `bool dir_is_file_exists(const char* filePath)`

- **Purpose**: This function checks if a file exists at the specified path.
- **Parameters**:
  - `filePath`: A string representing the path of the file to check.
- **Return Value**:
  - Returns `true` if the file exists, or `false` if it does not exist or if an error occurs.
- **Input Validation**: It checks if `filePath` is `NULL`. If so, it logs an error and returns `false`.

---

### `bool dir_copy_file(const char* srcPath, const char* destPath)`

- **Purpose**: This function copies a file from a source path (`srcPath`) to a destination path (`destPath`).
- **Parameters**:
  - `srcPath`: The path of the source file to be copied.
  - `destPath`: The destination file path where the file should be copied to.
- **Return Value**:
  - Returns `true` if the file was successfully copied, or `false` if an error occurred.

- **Input Validation**: The function first checks if both `srcPath` and `destPath` are provided. If either is `NULL`, it logs an error and returns `false`.

---

### `bool dir_copy_directory(const char* srcDir, const char* destDir)`

- **Purpose**: This function copies an entire directory, including its contents (files and subdirectories), from a source path (`srcDir`) to a destination path (`destDir`).
- **Parameters**:
  - `srcDir`: The path of the source directory to be copied.
  - `destDir`: The path of the destination directory where the contents should be copied.
- **Return Value**:
  - Returns `true` if the directory and its contents were successfully copied, or `false` if an error occurred.

- **Input Validation**: The function first checks if both `srcDir` and `destDir` are provided. If either is `NULL`, it logs an error and returns `false`.

---

### `long long dir_get_directory_size(const char* dirPath)`

- **Purpose**:  
  This function calculates the total size of a directory and all its contents, including subdirectories, in bytes. It provides different implementations based on the operating system (Windows or Unix-like systems). If any error occurs, such as a `NULL` directory path or failure to compute the size, the function returns `-1`.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory to be analyzed for its size.
- **Return Value**:  
  - The size of the directory in bytes if successful.
  - `-1`: If an error occurs (e.g., invalid directory path or failure in size calculation).

- **Input Validation**:  
  The function first checks if the provided `dirPath` is `NULL`. If it is, the function logs an error message and returns `-1`.

---

### `long long dir_get_file_size(const char* filePath)`

- **Purpose**:  
  This function calculates the size of a specific file in bytes. It handles file size calculation differently depending on the operating system (Windows or Unix-like systems). If any error occurs, such as a `NULL` file path or failure to calculate the size, the function returns `-1`.
- **Parameters**:  
  - `filePath`: A pointer to a string representing the path of the file whose size is to be determined.
- **Return Value**:  
  - The size of the file in bytes if successful.
  - `-1`: If an error occurs (e.g., invalid file path or failure to calculate the file size).

- **Input Validation**:  
  The function first checks if the provided `filePath` is `NULL`. If it is, the function logs an error message and returns `-1`.

---

### `void dir_list_contents(const char* dirPath, DirListOption option, Vector* resultVector)`  
- **Purpose**:  This function lists the contents of a directory based on the specified option (e.g., files only, directories only, or both). The results are stored in a `Vector`.

- **Parameters:**
  - `dirPath`: The path of the directory to list.
  - `option`: Specifies what to list (e.g., files, directories, or both).
  - `resultVector`: A vector where the results will be stored.
- **Return Value**:
  - void: This function does not return any value. The results (directory contents) are instead stored in the resultVector. Errors are handled internally, typically through logging, rather than returning an error code.

---

### `bool dir_is_file(const char *filePath)`

- **Purpose**: This function checks if the specified path points to a regular file. It returns `true` if the path points to a file and `false` if it does not exist or points to something other than a regular file (e.g., a directory, symbolic link, etc.).
- **Parameters:**
  - `filePath`: A pointer to a string that specifies the path of the file to check. The path must be a valid file system path. If `filePath` is `NULL`, the function logs an error and returns `false`.
- **Return Value**:
  - `true`: If the path points to a regular file.
  - `false`: If the path does not exist, points to something other than a regular file, or if there is an error (such as a `NULL` path or file access issues).
- **Error Handling**: If the provided `filePath` is `NULL` or the file does not exist, the function logs the error and returns `false`.

---

### `bool dir_is_directory(const char* dirPath)`

- **Purpose**:  
  This function checks whether the specified path is a directory.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory to be checked.
- **Return Value**:  
  - `true`: If the path is a directory.
  - `false`: If the path is not a directory, the directory does not exist, or an error occurs.

- **Input Validation**:  
  The function first checks if the provided `dirPath` is `NULL`. If it is, the function logs an error message and returns `false`.

---

### `bool dir_move_file(const char* srcPath, const char* destPath)`

- **Purpose**:  
  This function moves a file from a source path to a destination path.
- **Parameters**:  
  - `srcPath`: A pointer to a string representing the source file path.
  - `destPath`: A pointer to a string representing the destination file path.
- **Return Value**:  
  - `true`: If the file was successfully moved.
  - `false`: If an error occurs (e.g., `NULL` paths, failure to move the file).

- **Input Validation**:  
  The function checks if both `srcPath` and `destPath` are `NULL`. If either is `NULL`, it logs an error message and returns `false`.

---

### `bool dir_move_directory(const char* srcPath, const char* destPath)`

- **Purpose**:  
  This function moves a directory and its contents from a source path to a destination path.
- **Parameters**:  
  - `srcPath`: A pointer to a string representing the source directory path.
  - `destPath`: A pointer to a string representing the destination directory path.
- **Return Value**:  
  - `true`: If the directory was successfully moved.
  - `false`: If an error occurs (e.g., `NULL` paths, failure to move the directory).
- **Input Validation**:  
  The function checks if both `srcPath` and `destPath` are `NULL`. If either is `NULL`, it logs an error message and returns `false`.

---

### `char* dir_get_modified_time(const char* dirPath)`

- **Purpose**:  
  This function retrieves the last modified time of a directory as a string. It supports both Windows and POSIX systems. The modified time is useful for tracking when changes were last made to the directory.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory.
- **Return Value**:  
  - A dynamically allocated string representing the modified time of the directory in the format "YYYY-MM-DD HH:MM:SS".
  - `NULL`: If an error occurs (e.g., `NULL` directory path or failure to retrieve the time).

- **Input Validation**:  
  If the `dirPath` is `NULL`, the function logs an error and returns `NULL`.
- **Error Handling**:  
  If the directory path is invalid, or memory allocation fails, the function logs appropriate error messages and returns `NULL`.

---

### `char* dir_get_creation_time(const char* dirPath)`

- **Purpose**:  
  This function retrieves the creation time of a directory as a string. Creation time retrieval is only supported on Windows systems. POSIX systems do not typically track creation time.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory.
- **Return Value**:  
  - A dynamically allocated string representing the creation time of the directory in the format "YYYY-MM-DD HH:MM:SS".
  - `NULL`: If an error occurs (e.g., `NULL` directory path, failure to retrieve the time, or unsupported platform).

- **Input Validation**:  
  If the `dirPath` is `NULL`, the function logs an error and returns `NULL`.
- **Error Handling**:  
  If the directory path is invalid, or memory allocation fails, the function logs appropriate error messages and returns `NULL`.

---

### `char* dir_get_home_directory()`

- **Purpose**:  
  This function retrieves the current user's home directory path as a string. It supports both Windows and POSIX systems. The home directory is typically where user-specific files and configurations are stored.
- **Parameters**:  
  - None.
- **Return Value**:  
  - A dynamically allocated string representing the home directory path.
  - `NULL`: If an error occurs (e.g., failure to retrieve the home directory path or memory allocation failure).
- **Error Handling**:  
  The function logs appropriate error messages if it fails to retrieve or convert the home directory path and returns `NULL` in such cases.

---

### `DirFileType dir_get_file_type(const char* filePath)`

- **Purpose**:  
  This function determines the type of a file or directory at the specified path and returns a value from the `DirFileType` enum, which can indicate whether the file is a regular file, directory, symbolic link, or an unknown type.
- **Parameters**:  
  - `filePath`: A pointer to a string representing the path to the file or directory.
- **Return Value**:  
  - Returns one of the following enum values:
    - `DIR_FILE_TYPE_REGULAR`: If the file is a regular file.
    - `DIR_FILE_TYPE_DIRECTORY`: If the file is a directory.
    - `DIR_FILE_TYPE_SYMLINK`: If the file is a symbolic link.
    - `DIR_FILE_TYPE_UNKNOWN`: If the file type could not be determined or the file does not exist.

- **Input Validation**:  
  If the `filePath` is `NULL`, the function logs an error and returns `DIR_FILE_TYPE_UNKNOWN`.
- **Error Handling**:  
  If the file path is invalid or the file type cannot be determined, the function logs an error and returns `DIR_FILE_TYPE_UNKNOWN`.

---

### `bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv)`

- **Purpose**:  
  This function encrypts the contents of a file using the DES encryption algorithm in CBC mode, with a specified password and initialization vector (IV). The encrypted data is written back to the original file.
- **Parameters**:  
  - `filePath`: A pointer to a string representing the path to the file to be encrypted.
  - `password`: A pointer to a string representing the encryption password (must be 8 bytes or fewer).
  - `iv`: A pointer to an 8-byte initialization vector (IV) used for encryption.
- **Return Value**:  
  - `true`: If the file was successfully encrypted.
  - `false`: If an error occurs during the encryption process (e.g., file not found, memory allocation failure, or encryption failure).

- **Input Validation**:  
  - The function checks if `filePath`, `password`, or `iv` are `NULL`. If any of these are `NULL`, it logs an error and returns `false`.
- **Error Handling**:  
  - If the file cannot be opened for reading or writing, memory allocation fails, the password is too long, or encryption fails, the function logs appropriate error messages and returns `false`.

---

### `bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv)`

- **Purpose**:  
  This function decrypts the contents of a file using the DES encryption algorithm in CBC mode. It uses the provided password and initialization vector (IV) to decrypt the file, then writes the decrypted data back to the original file.
- **Parameters**:  
  - `filePath`: A pointer to a string representing the path of the file to be decrypted.
  - `password`: A pointer to a string representing the decryption password (must be 8 bytes or fewer).
  - `iv`: A pointer to an 8-byte initialization vector used for decryption.
- **Return Value**:  
  - `true`: If the file was successfully decrypted.
  - `false`: If an error occurs during the decryption process (e.g., invalid input, file not found, or decryption failure).

- **Input Validation**:  
  The function checks if `filePath`, `password`, or `iv` are `NULL`. If any of these are `NULL`, it logs an error and returns `false`.

- **Error Handling**:  
  If the file cannot be opened, memory allocation fails, or decryption fails, the function logs appropriate error messages and returns `false`.

---

### `bool dir_get_file_owner(const char* filePath, char* ownerBuffer, size_t bufferSize)`

- **Purpose**:  
  This function retrieves the owner of the specified file. It fills the `ownerBuffer` with the owner's name and ensures that the string does not exceed the size of the buffer.
- **Parameters**:  
  - `filePath`: A pointer to a string representing the path of the file.
  - `ownerBuffer`: A pointer to a buffer where the owner's name will be stored.
  - `bufferSize`: The size of the `ownerBuffer`.
- **Return Value**:  
  - `true`: If the file owner was successfully retrieved.
  - `false`: If an error occurs (e.g., the file path is invalid, or the retrieval fails).

- **Input Validation**:  
  The function checks if `filePath`, `ownerBuffer`, or `bufferSize` are `NULL` or if `bufferSize` is 0. If any of these conditions are true, it logs an error and returns `false`.
- **Error Handling**:  
  If the file path is invalid, memory allocation fails, or the file owner cannot be retrieved, the function logs appropriate error messages and returns `false`.

---

### `bool dir_get_directory_owner(const char* dirPath, char* ownerBuffer, size_t bufferSize)`

- **Purpose**:  
  This function retrieves the owner of the specified directory and stores the owner's name in the provided buffer.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory.
  - `ownerBuffer`: A pointer to a buffer where the owner's name will be stored.
  - `bufferSize`: The size of the `ownerBuffer`.
- **Return Value**:  
  - `true`: If the directory owner was successfully retrieved.
  - `false`: If an error occurs (e.g., invalid input or failure to retrieve the owner).

- **Input Validation**:  
  The function checks if `dirPath`, `ownerBuffer`, or `bufferSize` are `NULL` or if `bufferSize` is 0. If any of these conditions are true, it logs an error and returns `false`.
- **Error Handling**:  
  If an error occurs, such as failure to open the directory or retrieve the owner, the function logs appropriate error messages and returns `false`.

---

### `bool dir_search(const char* dirPath, const char* pattern, DirCompareFunc callback, void* userData)`

- **Purpose**:  
  This function searches a directory for files matching a specified pattern. It invokes a user-defined callback function for each matched file.
- **Parameters**:  
  - `dirPath`: A pointer to a string representing the path of the directory to search.
  - `pattern`: A pointer to a string representing the pattern to match files against (e.g., "*.txt").
  - `callback`: A function pointer to a user-defined callback function that is invoked for each matched file.
  - `userData`: A pointer to user-defined data that will be passed to the callback function.

- **Return Value**:  
  - `true`: If the search was successful and the callback was invoked for matching files.
  - `false`: If an error occurs (e.g., invalid input or failure to search the directory).

- **Input Validation**:  
  The function checks if `dirPath`, `pattern`, or `callback` are `NULL`. If any of these are `NULL`, it logs an error and returns `false`.
- **Error Handling**:  
  If an error occurs, such as failure to open the directory or search for files, the function logs appropriate error messages and returns `false`.

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
Current directory path: 'C:\Users\asus\Hell\Desktop\stack'
```

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
Failed to count files and directories in 'C:\Users\Science\Desktop\projects\C\c_std'.
```

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
Absolute file path: 'C:\Users\asus\OneDrive\Desktop\stack\کایزن\amin.txt'
```

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
Failed to change directory.
Moved up one directory.
Current directory path: 'C:\Users\asus\OneDrive\Desktop'
```

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
File './کایزن/amin.txt'
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
Directory './کایزن'
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
Total size of directory './کایزن' is: 10 bytes. // data in amin.txt is Hello amin
```

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
Home Directory: C:\Users\amin
```

---

## Example 22 : get file type with `dir_get_file_type`

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

## Example 23 : encrypt and decrypt file with des mode cbc `dir_encrypt_file` and `dir_decrypt_file`

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
File encrypted successfully
File decrypted successfully.
```

---

## Example 24 : get file owner with `dir_get_file_owner`

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

---

## Example 25 : get dir owner with `dir_get_directory_owner`

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

---

## Example 26 : search file in path `dir_search`

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

---

## Example 27 : get list of contents `dir_list_contents`

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

---

## License

This project is open-source and available under [ISC License].