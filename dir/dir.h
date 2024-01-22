/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Dir
*/

#ifndef DIR_H_ 
#define DIR_H_

#include <stdint.h>
#include "../vector/vector.h"

typedef bool (*DirCompareFunc)(const char* filePath, void* userData);

typedef enum {
    DIR_LIST_FILES,
    DIR_LIST_DIRECTORIES,
    DIR_LIST_ALL
} DirListOption;

typedef enum {
    DIR_FILE_TYPE_UNKNOWN,
    DIR_FILE_TYPE_REGULAR,
    DIR_FILE_TYPE_DIRECTORY,
    DIR_FILE_TYPE_SYMLINK
} DirFileType;

bool dir_make_directory(const char* dirpath);
bool dir_cd(const char* dirName);
bool dir_cd_up();
bool dir_is_empty(const char* dirName);
bool dir_remove_directory(const char* dirName);
bool dir_remove_directory_recursive(const char* dirPath);
bool dir_rename(const char* oldName, const char* newName);
bool dir_is_directory_exists(const char* dirPath);
bool dir_is_file_exists(const char* filePath);
bool dir_copy_file(const char* srcPath, const char* destPath);
bool dir_copy_directory(const char* srcDir, const char* destDir);
bool dir_is_file(const char* filePath);
bool dir_is_directory(const char* filePath);
bool dir_move_file(const char* srcPath, const char* destPath);
bool dir_move_directory(const char* srcPath, const char* destPath);
bool dir_encrypt_file(const char* filePath, const char* password, uint8_t* iv);
bool dir_decrypt_file(const char* filePath, const char* password, uint8_t* iv);
bool dir_get_file_owner(const char* filePath, char* ownerBuffer, size_t bufferSize);
bool dir_get_directory_owner(const char* dirPath, char* ownerBuffer, size_t bufferSize);
bool dir_search(const char* dirPath, const char* pattern, DirCompareFunc callback, void* userData);

char* dir_get_creation_time(const char* dirPath);
char* dir_get_modified_time(const char* dirPath);
char* dir_dir_name(const char* dirpath);
char* dir_current_path(void);
char* dir_absolute_file_path(const char* relative_path);
char* dir_get_home_directory();

int dir_count(const char* dirpath);
void dir_list_contents(const char* dirPath, DirListOption option, Vector* result);
DirFileType dir_get_file_type(const char* filePath);

long long dir_get_directory_size(const char* dirPath);
long long dir_get_file_size(const char* filePath);

#endif 