#ifndef STD_REGEX_H_
#define STD_REGEX_H_

#include <stddef.h>
#include <stdio.h>

// #define REGEX_LOGGING_ENABLE

#ifdef REGEX_LOGGING_ENABLE 
    #define REGEX_LOG(fmt, ...) \
        do { fprintf(stderr, "[REGEX LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define REGEX_LOG(fmt, ...) do { } while (0)
#endif

typedef struct Regex Regex;

typedef enum {
    REGEX_SUCCESS,
    REGEX_COMPILE_ERROR,
    REGEX_MATCH_ERROR,
    REGEX_NO_MATCH
} RegexResult;


typedef enum {
    REGEX_DEFAULT = 0,          
    REGEX_CASE_INSENSITIVE = 1, 
    REGEX_MULTILINE = 2,        
    REGEX_DOTALL = 4            
} RegexFlags;


typedef struct {
    const char* start;  
    const char* end;    
    size_t length;      
    int group_count;     
    const char** group_starts;   
    size_t* group_lengths;       
} RegexMatch;


Regex* regex_compile(const char* pattern, RegexFlags flags);

RegexResult regex_match(Regex* regex, const char* string, RegexMatch* match);
RegexResult regex_search(Regex* regex, const char* string, RegexMatch* match);

int regex_find_all(Regex* regex, const char* string, RegexMatch* matches, int max_matches);
void regex_deallocate(Regex* regex);
const char* regex_error_message(RegexResult result);

#endif
