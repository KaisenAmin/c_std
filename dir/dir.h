#ifndef DIR_H_ 
#define DIR_H_

#include <stdbool.h>
#include <stddef.h>

typedef enum {
    LIST_FILES,
    LIST_DIRECTORIES,
    LIST_ALL
} DirListOption;


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

char* dir_get_creation_time(const char* dirPath);
char* dir_get_modified_time(const char* dirPath);
char* dir_dir_name(const char* dirpath);
char* dir_current_path(void);
char* dir_absolute_file_path(const char* relative_path);

int dir_count(const char* dirpath);
void dir_list_contents(const char* dirPath, DirListOption option);

long long dir_get_directory_size(const char* dirPath);
long long dir_get_file_size(const char* filePath);

#endif 