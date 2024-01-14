





## Example 1 : create directory with `dir_make_directory`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* dir_name_utf8 = "./کایزن";
    bool result = dir_make_directory(dir_name_utf8);

    if (result) {
        printf("Directory '%s' created successfully.\n", dir_name_utf8);
    } 
    else {
        printf("Failed to create directory '%s'.\n", dir_name_utf8);
    }
    return 0;
}
```

## Example 2 : get directory name with `dir_dir_name`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* dir_path = "./کایزن";
    char* dirName = dir_dir_name(dir_path);

    if (dirName) {
        printf("Directory name: '%s'\n", dirName);
        free(dirName);
    } 
    else {
        printf("Failed to get directory name.\n");
    }
    return 0;
}
```

## Example 3 : how to get current_path with `dir_current_path`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* currentPath = dir_current_path();

    if (currentPath) {
        printf("Current directory path: '%s'\n", currentPath);
        free(currentPath);
    } 
    else {
        printf("Failed to get current directory path.\n");
    }

    return 0;
}
```

## Example 4 : count any things in path with `dir_count`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* dir_path = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std"; // Current directory
    int fileCount = dir_count(dir_path);

    if (fileCount >= 0) {
        printf("Total number of directories and files in '%s': %d\n", dir_path, fileCount);
    } 
    else {
        printf("Failed to count files and directories in '%s'.\n", dir_path);
    }

    return 0;
}
```

## Example 5 : get absolute_path of file with `dir_absolute_path_file`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* relativePath = "کایزن/amin.txt"; // .,/ or any path
    char* absolutePath = dir_absolute_file_path(relativePath);

    if (absolutePath) {
        printf("Absolute file path: '%s'\n", absolutePath);
        free(absolutePath);
    } 
    else {
        printf("Failed to get absolute file path.\n");
    }

    return 0;
}
```

## Example 6 : change directory or move up with `dir_cd` and `dir_cd_up`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* cd = "date";

    if (dir_cd(cd)) {
        printf("Changed directory to 'date'\n");
        char* currentPath = dir_current_path();

        if (currentPath) {
            printf("Current directory path: '%s'\n", currentPath);
            free(currentPath);
        } 
        else {
            printf("Failed to get current directory path.\n");
        }
    } 
    else {
        printf("Failed to change directory.\n");
    }

    if (dir_cd_up()) {
        printf("Moved up one directory.\n");
        char* currentPath = dir_current_path();
        if (currentPath) {
            printf("Current directory path: '%s'\n", currentPath);
            free(currentPath);
        } 
        else {
            printf("Failed to get current directory path.\n");
        }
    } 
    else {
        printf("Failed to move up directory.\n");
    }

    return 0;
}
```

## Example 7 : check if directory is empty delete dir `dir_remove_directory` and `dir_is_empty`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* dirName = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";

    if (dir_is_empty(dirName)) {
        printf("Directory '%s' is empty.\n", dirName);
        
        if (dir_remove_directory(dirName)) {
            printf("Successfully removed directory '%s'.\n", dirName);
        } 
        else {
            printf("Failed to remove directory '%s'.\n", dirName);
        }
    } 
    else {
        printf("Directory '%s' is not empty, cannot remove.\n", dirName);
    }

    return 0;
}
```

## Example 8: remove dir recursivly with `dir_remove_directory_recursive`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    const char* dirName = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";

    if (dir_remove_directory_recursive(dirName)) {
        printf("Successfully removed directory '%s'.\n", dirName);
    } 
    else {
        printf("Failed to remove directory '%s'.\n", dirName);
    }

    return 0;
}
```

## Example 9: Renaming a File with `dir_rename`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* oldFileName = "./tester.txt";
    const char* newFileName = "./amin.txt";

    if (dir_rename(oldFileName, newFileName)) {
        printf("File renamed successfully from '%s' to '%s'.\n", oldFileName, newFileName);
    } 
    else {
        printf("Failed to rename file '%s'.\n", oldFileName);
    }

    return 0;
}

```

## Example 10: Renaming a Directory with `dir_rename`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* oldDirName = "./one";
    const char* newDirName = "./کایزن";

    if (dir_rename(oldDirName, newDirName)) {
        printf("Directory renamed successfully from '%s' to '%s'.\n", oldDirName, newDirName);
    } 
    else {
        printf("Failed to rename directory '%s'.\n", oldDirName);
    }

    return 0;
}

```

## Example 11 : check file exists or not `dir_is_file_exists`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* dirPath = "./dir/dir.h"; // Replace with the directory path you want to check

    if (dir_is_file_exists(dirPath)) {
        printf("File '%s' exists.\n", dirPath);
    } 
    else {
        printf("File '%s' does not exist.\n", dirPath);
    }

    return 0;
}
```

## Example 12 : check directory is exists or not `dir_is_directory_exists`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* dirPath = "./کایزن"; // Replace with the directory path you want to check

    if (dir_is_directory_exists(dirPath)) {
        printf("Directory '%s' exists.\n", dirPath);
    } 
    else {
        printf("Directory '%s' does not exist.\n", dirPath);
    }

    return 0;
}
```

## Example 13 : copy dir and its contents to new location with `dir_copy_directory`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* sourceDir = "./کایزن"; // Replace with your source directory path
    const char* destinationDir = "C:\\Users\\Science\\Desktop\\کایزن"; // Replace with your destination directory path

    if (dir_make_directory("C:\\Users\\Science\\Desktop\\کایزن")) {
        if (dir_copy_directory(sourceDir, destinationDir)) {
            printf("Directory copied successfully from '%s' to '%s'.\n", sourceDir, destinationDir);
        } 
        else {
            printf("Failed to copy directory from '%s' to '%s'.\n", sourceDir, destinationDir);
        }
    } 
    else {
        printf("Can not create folder");
    }

    return 0;
}
```

## Example 14 : copy file into new location `dir_copy_file`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* sourceFile = "./کایزن/amin.txt"; // Replace with your source file path
    const char* destinationFile = "C:\\Users\\Science\\Desktop\\amin.txt"; // Replace with your destination file path

    if (dir_copy_file(sourceFile, destinationFile)) {
        printf("File copied successfully from '%s' to '%s'.\n", sourceFile, destinationFile);
    } 
    else {
        printf("Failed to copy file from '%s' to '%s'.\n", sourceFile, destinationFile);
    }

    return 0;
}
```

## Example 15 :  Calculate the total size of a directory, including all files and subdirectories `dir_get_directory_size`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* dirPath = "./کایزن"; 

    long long size = dir_get_directory_size(dirPath);

    // Check for errors
    if (size == -1) {
        printf("Failed to calculate the size of directory '%s'.\n", dirPath);
    } 
    else {
        printf("Total size of directory '%s' is: %lld bytes.\n", dirPath, size);
    }
    return 0;
}
```

## Example 16 : Calculating the total size of file with `dir_get_file_size`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* filePath = "./کایزن/amin.txt"; 

    long long size = dir_get_file_size(filePath);

    // Check for errors
    if (size == -1) {
        printf("Failed to calculate the file size '%s'.\n", filePath);
    } 
    else {
        printf("Total size of file '%s' is: %lld bytes.\n", filePath, size);
    }
    return 0;
}
```

## Example 17 : get list of director or files or both with `dir_list_contents`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* dirPath = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std"; 

    printf("Listing all files and directories:\n");
    dir_list_contents(dirPath, DIR_LIST_ALL);

    printf("\nListing only files:\n");
    dir_list_contents(dirPath, DIR_LIST_FILES);

    printf("\nListing only directories:\n");
    dir_list_contents(dirPath, DIR_LIST_DIRECTORIES);

    return 0;
}
```

## Example 18 : check path is dir or file with `dir_is_file` or `dir_is_directory`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* dirPath = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\main.c"; 

    // also you can set another path for checking dir
    if (dir_is_file(dirPath)) {
        printf("Yes \"%s\" is path", dirPath);
    }
    else {
        printf("No it is\n");
    }
    return 0;
}
```

## Example 19 : move file and directoy with these `dir_move_directory` or `dir_move_file`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\projects\\C\\c_std\\کایزن";
    const char* destPath = "C:\\Users\\Science\\Desktop\\one";

    // also for moving files you can use "dir_move_file" 
    if (dir_move_directory(sourcePath, destPath)) {
        printf("Move to new location");
    }
    else {
        printf("not Move\n");
    }

    return 0;
}

```

## Example 20 : get last modified file and dir with `dir_get_modified_time`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\new_one.txt";

    printf("Last Modified Time is %s\n", dir_get_modified_time(sourcePath));

    return 0;
}
```

## Example 21 : get last creation file and dir with `dir_get_creation_time`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    const char* sourcePath = "C:\\Users\\Science\\Desktop\\new_one.txt";

    printf("Last Modified Time is %s\n", dir_get_creation_time(sourcePath));

    return 0;
}

```

## Example 22 : get home dir with `dir_get_home_directory`

```c
#include "dir/dir.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    char* homeDirectory = dir_get_home_directory();

    if (homeDirectory != NULL) {
        printf("Home Directory: %s\n", homeDirectory);
        free(homeDirectory);
    } 
    else {
        printf("Failed to get home directory.\n");
    }

    return 0;
}
```

## Example 22 : get file type with `dir_get_file_type`

```c
#include "dir/dir.h"
#include <stdio.h>

int main() {
    DirFileType type = dir_get_file_type("./dir/dir.h");
    switch (type) {
        case DIR_FILE_TYPE_REGULAR:
            printf("Regular File\n");
            break;
        case DIR_FILE_TYPE_DIRECTORY:
            printf("Directory\n");
            break;
        case DIR_FILE_TYPE_SYMLINK:
            printf("Symbolic Link\n");
            break;
        default:
            printf("Unknown Type\n");
    }
    return 0;
}
```
## Example 23 : encrypt and decrypt file with des mode cbc `dir_encrypt_file` and `dir_decrypt_file`

```c
#include "dir/dir.h"
#include "crypto/crypto.h"
#include <stdio.h>
#include <stdint.h>

int main() {
    const char* filePath = "C:\\Users\\Science\\Desktop\\new_one.txt";
    const char* password = "aminamin";
    uint8_t iv[DES_BLOCK_SIZE]; // IV for CBC mode
    
    crypto_generate_random_iv(iv, DES_BLOCK_SIZE); 

    if (dir_encrypt_file(filePath, password, iv)) {
        printf("File encrypted successfully.\n");
    } 
    else {
        printf("Failed to encrypt the file.\n");
    }

    if (dir_decrypt_file(filePath, password, iv)) {
        printf("File decrypted successfully.\n");
    } 
    else {
        printf("Failed to decrypt the file.\n");
    }

    return 0;
}
```