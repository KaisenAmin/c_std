#include "std_regex.h"
#ifdef _WIN32
    #include <pcre.h>
#else
    #include <regex.h>
#endif

#include <stdlib.h>
#include <string.h>

// Internal struct to hold platform-specific regex data
struct Regex {
#ifdef _WIN32
    pcre* compiled_pattern;
    const char* error;
    int erroffset;
#else
    regex_t compiled_pattern;
#endif
};

/**
 * @brief Compiles the given regular expression pattern into a Regex object.
 * 
 * @param pattern The regex pattern to compile.
 * @param flags Optional flags for regex compilation (e.g., REGEX_CASE_INSENSITIVE).
 * @return A pointer to a Regex object, or NULL if compilation failed.
 */
Regex* regex_compile(const char* pattern, RegexFlags flags) {
    REGEX_LOG("[regex_compile] Compiling pattern: %s", pattern);

    Regex* regex = (Regex*)malloc(sizeof(Regex));
    if (!regex) {
        REGEX_LOG("[regex_compile] Memory allocation failed.");
        return NULL;
    }

#ifdef _WIN32
    int pcre_flags = 0;
    if (flags & REGEX_CASE_INSENSITIVE) {
        pcre_flags |= PCRE_CASELESS;
    }
    if (flags & REGEX_MULTILINE) {
        pcre_flags |= PCRE_MULTILINE;
    }
    if (flags & REGEX_DOTALL) {
        pcre_flags |= PCRE_DOTALL;
    }

    regex->compiled_pattern = pcre_compile(pattern, pcre_flags, &regex->error, &regex->erroffset, NULL);
    if (!regex->compiled_pattern) {
        REGEX_LOG("[regex_compile] PCRE compilation failed at offset %d: %s", regex->erroffset, regex->error);
        free(regex);
        return NULL;
    }
#else
    int posix_flags = 0;
    if (flags & REGEX_CASE_INSENSITIVE) {
        posix_flags |= REG_ICASE;
    }
    if (flags & REGEX_MULTILINE) {
        posix_flags |= REG_NEWLINE;
    }

    int compile_result = regcomp(&regex->compiled_pattern, pattern, posix_flags);
    if (compile_result != 0) {
        REGEX_LOG("[regex_compile] POSIX regex compilation failed.");
        free(regex);
        return NULL;
    }
#endif
    REGEX_LOG("[regex_compile] Compilation successful.");
    return regex;
}


/**
 * @brief Matches a string against a compiled regular expression.
 * 
 * @param regex The compiled regex object.
 * @param string The string to match.
 * @param match Optional pointer to store match details (start/end/length) for the first match.
 * @return REGEX_SUCCESS if the string matches, REGEX_NO_MATCH if it doesn't, or REGEX_MATCH_ERROR on error.
 */
RegexResult regex_match(Regex* regex, const char* string, RegexMatch* match) {
    REGEX_LOG("[regex_match] Matching string: %s", string);
    
#ifdef _WIN32
    int ovector[30];
    int rc = pcre_exec(regex->compiled_pattern, NULL, string, (int)strlen(string), 0, 0, ovector, 30);
    
    if (rc < 0) {
        REGEX_LOG("[regex_match] Match failed with code %d", rc);
        return (rc == PCRE_ERROR_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }
    if (match) {
        match->start = string + ovector[0];
        match->end = string + ovector[1];
        match->length = ovector[1] - ovector[0];
        match->group_count = rc - 1;

        // Extract group matches
        if (match->group_count > 0) {
            match->group_starts = malloc(match->group_count * sizeof(char*));
            match->group_lengths = malloc(match->group_count * sizeof(size_t));
            for (int i = 1; i <= match->group_count; ++i) {
                match->group_starts[i-1] = string + ovector[2 * i];
                match->group_lengths[i-1] = ovector[2 * i + 1] - ovector[2 * i];
            }
        }
    }

    REGEX_LOG("[regex_match] Match found: %.*s", (int)(match->length), match->start);
    return REGEX_SUCCESS;
#else
    regmatch_t pmatch[10]; // Allocate for up to 10 groups
    int result = regexec(&regex->compiled_pattern, string, 10, pmatch, 0);

    if (result == 0) {
        if (match) {
            match->start = string + pmatch[0].rm_so;
            match->end = string + pmatch[0].rm_eo;
            match->length = pmatch[0].rm_eo - pmatch[0].rm_so;
            match->group_count = 10; // Assume max groups supported is 10

            match->group_starts = malloc(match->group_count * sizeof(char*));
            match->group_lengths = malloc(match->group_count * sizeof(size_t));

            for (int i = 0; i < 10 && pmatch[i].rm_so != -1; i++) {
                match->group_starts[i] = string + pmatch[i].rm_so;
                match->group_lengths[i] = pmatch[i].rm_eo - pmatch[i].rm_so;
            }
        }
        REGEX_LOG("[regex_match] Match found: %.*s", (int)(match->length), match->start);
        return REGEX_SUCCESS;
    }
    REGEX_LOG("[regex_match] No match found or error during matching.");
    return (result == REG_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
#endif
}

/**
 * @brief Searches for the given regex pattern in the provided string.
 * 
 * @param regex The compiled regex object.
 * @param string The string to search.
 * @param match Optional pointer to store match details (start/end/length) for the first match.
 * @return REGEX_SUCCESS if the pattern is found, REGEX_NO_MATCH if not, or REGEX_MATCH_ERROR on error.
 */
RegexResult regex_search(Regex* regex, const char* string, RegexMatch* match) {
    REGEX_LOG("[regex_search] Searching string: %s", string);
    return regex_match(regex, string, match);
}

/**
 * @brief Finds all occurrences of the regex pattern in the string.
 * 
 * @param regex The compiled regex object.
 * @param string The string to search.
 * @param matches Pointer to an array of RegexMatch objects (caller allocates memory).
 * @param max_matches Maximum number of matches to find.
 * @return The number of matches found, or -1 on error.
 */
int regex_find_all(Regex* regex, const char* string, RegexMatch* matches, int max_matches) {
    REGEX_LOG("[regex_find_all] Finding all matches in string: %s", string);
    
    int count = 0;
    const char* current = string;
    RegexMatch temp_match;

    while (count < max_matches && regex_match(regex, current, &temp_match) == REGEX_SUCCESS) {
        matches[count] = temp_match;
        REGEX_LOG("[regex_find_all] Match %d found: %.*s", count + 1, (int)temp_match.length, temp_match.start);
        count++;
        current = temp_match.end;
    }

    REGEX_LOG("[regex_find_all] Total matches found: %d", count);
    return count;
}

/**
 * @brief Frees resources allocated for the compiled regex object.
 * 
 * @param regex The regex object to free.
 */
void regex_deallocate(Regex* regex) {
    REGEX_LOG("[regex_deallocate] Deallocating regex object.");
    if (regex) {
#ifdef _WIN32
        pcre_free(regex->compiled_pattern);
#else
        regfree(&regex->compiled_pattern);
#endif
        free(regex);
        REGEX_LOG("[regex_deallocate] Regex object successfully deallocated.");
    }
}

/**
 * @brief Converts a regex result code to a human-readable error message.
 * 
 * @param result The result code (e.g., REGEX_COMPILE_ERROR, REGEX_MATCH_ERROR).
 * @return A string representing the error message.
 */
const char* regex_error_message(RegexResult result) {
    REGEX_LOG("[regex_error_message] Converting result code: %d", result);

    switch (result) {
        case REGEX_SUCCESS: 
            return "Success";
        case REGEX_COMPILE_ERROR: 
            return "Compilation Error";
        case REGEX_MATCH_ERROR: 
            return "Match Error";
        case REGEX_NO_MATCH: 
            return "No Match Found";
        default: 
            return "Unknown Error";
    }
}