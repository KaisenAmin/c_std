/**
 * @author Amin Tahmasebi
 * @class Regex
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in std_regex.c (file-level convention).
 */

#ifndef STD_REGEX_H_
#define STD_REGEX_H_

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif


/* #define REGEX_LOGGING_ENABLE */

#ifdef REGEX_LOGGING_ENABLE
    #define REGEX_LOG(fmt, ...) \
        do { fprintf(stderr, "[REGEX LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define REGEX_LOG(...) do { } while (0)
#endif


/* Opaque compiled-pattern handle. Allocate with `regex_compile`,
 * release with `regex_deallocate`. */
typedef struct Regex Regex;

/* Result codes returned by `regex_match` / `regex_search`. */
typedef enum {
    REGEX_SUCCESS,
    REGEX_COMPILE_ERROR,
    REGEX_MATCH_ERROR,
    REGEX_NO_MATCH
} RegexResult;


/* Compile-time flags. Combine with bitwise OR. */
typedef enum {
    REGEX_DEFAULT          = 0,
    REGEX_CASE_INSENSITIVE = 1,
    REGEX_MULTILINE        = 2,
    REGEX_DOTALL           = 4
} RegexFlags;


/* Match result. After a successful `regex_match` / `regex_search`,
 * `start` / `end` delimit the full match in the original string,
 * `length` == end - start, and each captured subgroup is exposed via
 * `group_starts[i]` / `group_lengths[i]` (0 < i < group_count).
 * Group 0 is the whole match. Release with `regex_match_free`. */
typedef struct {
    const char*  start;
    const char*  end;
    size_t       length;
    int          group_count;
    const char** group_starts;
    size_t*      group_lengths;
} RegexMatch;


/* ------------------------------------------------------------------ */
/* Compile / destroy                                                  */
/* ------------------------------------------------------------------ */

Regex*       regex_compile             (const char* pattern, RegexFlags flags);
void         regex_deallocate          (Regex* regex);


/* ------------------------------------------------------------------ */
/* Matching                                                           */
/* ------------------------------------------------------------------ */

RegexResult  regex_match               (Regex* regex, const char* string, RegexMatch* match);
RegexResult  regex_search              (Regex* regex, const char* string, RegexMatch* match);
int          regex_find_all            (Regex* regex, const char* string, RegexMatch* matches, int max_matches);


/* ------------------------------------------------------------------ */
/* Match-result helpers                                               */
/* ------------------------------------------------------------------ */

void         regex_match_free          (RegexMatch* match);
char*        regex_match_group         (const RegexMatch* match, int group_index);


/* ------------------------------------------------------------------ */
/* Convenience one-shot wrappers (compile + match in one call)        */
/* ------------------------------------------------------------------ */

int          regex_test                (const char* pattern, const char* string, RegexFlags flags);
int          regex_count_matches       (const char* pattern, const char* string, RegexFlags flags);
char*        regex_replace             (const char* pattern, const char* string, const char* replacement, RegexFlags flags);
char*        regex_replace_first       (const char* pattern, const char* string, const char* replacement, RegexFlags flags);
char**       regex_split               (const char* pattern, const char* string, RegexFlags flags, int* out_count);
void         regex_split_free          (char** tokens, int count);


/* ------------------------------------------------------------------ */
/* Diagnostics                                                        */
/* ------------------------------------------------------------------ */

const char*  regex_error_message       (RegexResult result);


#ifdef __cplusplus
}
#endif

#endif 
