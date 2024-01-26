# Dir Library in C

**Author:** Amin Tahmasebi
**Release Date:** 2024
**License:** ISC License

## Overview
This C library is dedicated to directory and file operations, offering comprehensive functionality to manage and manipulate files and directories on the filesystem. It is designed to provide a straightforward and efficient way to interact with the filesystem in C programming.

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

- `dir_make_directory`: Creates a new directory.
- `dir_dir_name`: Retrieves the name of a directory.
- `dir_current_path`: Gets the current working directory path.
- `dir_count`: Counts the number of items (files and directories) in a given path.
- `dir_absolute_file_path`: Converts a relative file path to an absolute path.
- `dir_cd` and `dir_cd_up`: Changes the current directory and moves up one directory.
- `dir_remove_directory` and `dir_is_empty`: Deletes a directory if it's empty and checks if a directory is empty.
- `dir_remove_directory_recursive`: Removes a directory and its contents recursively.
- `dir_rename`: Renames a file or directory.
- `dir_is_file_exists` and `dir_is_directory_exists`: Checks if a file or directory exists.
- `dir_copy_directory` and `dir_copy_file`: Copies a directory or a file to a new location.
- `dir_get_directory_size` and `dir_get_file_size`: Calculates the size of a directory or a file.
- `dir_list_contents`: Lists the contents of a directory.
- `dir_is_file` and `dir_is_directory`: Checks if a path is a file or a directory.
- `dir_move_directory` and `dir_move_file`: Moves a directory or a file to a new location.
- `dir_get_modified_time` and `dir_get_creation_time`: Gets the last modified or creation time of a file or directory.
- `dir_get_home_directory`: Retrieves the path of the user's home directory.
- `dir_get_file_type`: Determines the type of a file (regular, directory, symbolic link).
- `dir_encrypt_file`

and `dir_decrypt_file`: Encrypts and decrypts a file using DES mode CBC.
- `dir_get_file_owner` and `dir_get_directory_owner`: Retrieves the owner of a file or directory.
- `dir_search`: Searches for files or directories based on a pattern.
- `dir_get_file_type`: Identifies the type of a file (regular, directory, symlink, etc.).
- `dir_list_contents`: Retrieves a list of files and directories in a specified path.
- `dir_is_file` or `dir_is_directory`: Checks if a given path is a file or a directory.
- `dir_move_directory` or `dir_move_file`: Moves a directory or file to a new location.
- `dir_get_modified_time` and `dir_get_creation_time`: Fetches the last modified or creation time of a file or directory.
- `dir_get_home_directory`: Obtains the path to the user's home directory.
- `dir_get_file_type`: Determines the type of a file (e.g., regular file, directory, symbolic link).

## Compilation
To compile a program using the Dir library, include all necessary source files in your compilation command. For example:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./dir/dir.c
```

This command compiles with GCC using the C17 standard and enables various optimizations and warnings for better performance and code quality.


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

## Example 5 : get absolute_path of file with `dir_absolute_path_file`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    const char* relativePath = "کایزن/amin.txt"; // .,/ or any path
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

## Example 7 : check if directory is empty delete dir `dir_remove_directory` and `dir_is_empty`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirName = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";

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

## Example 8: remove dir recursivly with `dir_remove_directory_recursive`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirName = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";

    if (dir_remove_directory_recursive(dirName)) {
        fmt_printf("Successfully removed directory '%s'.\n", dirName);
    } 
    else {
        fmt_printf("Failed to remove directory '%s'.\n", dirName);
    }
    return 0;
}
```

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

## Example 11 : check file exists or not `dir_is_file_exists`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* dirPath = "./dir/dir.h"; // Replace with the directory path you want to check

    if (dir_is_file_exists(dirPath)) {
        fmt_printf("File '%s' exists.\n", dirPath);
    } 
    else {
        fmt_printf("File '%s' does not exist.\n", dirPath);
    }
    return 0;
}
```

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

## Example 13 : copy dir and its contents to new location with `dir_copy_directory`

```c
#include "dir/dir.h"
#include "fmt/fmt.h"

int main() {
    const char* sourceDir = "./کایزن"; // Replace with your source directory path
    const char* destinationDir = "C:\\Users\\Science\\Desktop\\کایزن"; // Replace with your destination directory path

    if (dir_make_directory("C:\\Users\\Science\\Desktop\\کایزن")) {
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

## Example 26 : search file in path `dir_search`

```c
#include "dir/dir.h"
#include "string/string.h"
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