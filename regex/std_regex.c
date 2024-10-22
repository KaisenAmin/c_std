#include <pcre.h>
#include <stdlib.h>
#include <string.h>

#include "std_regex.h"

// Internal struct to hold platform-specific regex data
struct Regex {
    pcre* compiled_pattern;
    const char* error;
    int erroffset;
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

    const char *error;
    int erroffset;
    int pcre_flags = 0;

    // Set PCRE flags based on the provided RegexFlags
    if (flags & REGEX_CASE_INSENSITIVE) {
        pcre_flags |= PCRE_CASELESS;
    }
    if (flags & REGEX_MULTILINE) {
        pcre_flags |= PCRE_MULTILINE;
    }
    if (flags & REGEX_DOTALL) {
        pcre_flags |= PCRE_DOTALL;
    }

    regex->compiled_pattern = pcre_compile(pattern, pcre_flags, &error, &erroffset, NULL);

    if (!regex->compiled_pattern) {
        REGEX_LOG("[regex_compile] PCRE compilation failed at offset %d: %s", erroffset, error);
        free(regex);
        return NULL;
    }

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

    int ovector[30];  // Array for storing the start and end positions of the matches
    int rc = pcre_exec(regex->compiled_pattern, NULL, string, (int)strlen(string), 0, 0, ovector, 30);

    if (rc < 0) {
        REGEX_LOG("[regex_match] Match failed with code %d", rc);
        return (rc == PCRE_ERROR_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }

    if (match) {
        match->start = string + ovector[0];
        match->end = string + ovector[1];
        match->length = ovector[1] - ovector[0];
        match->group_count = rc - 1;  // Subtract 1 because rc includes the whole match as group 0

        // Allocate memory for storing group information
        match->group_starts = malloc(match->group_count * sizeof(char*));
        match->group_lengths = malloc(match->group_count * sizeof(size_t));

        for (int i = 1; i <= match->group_count; i++) {
            match->group_starts[i-1] = string + ovector[2 * i];
            match->group_lengths[i-1] = ovector[2 * i + 1] - ovector[2 * i];
        }

        REGEX_LOG("[regex_match] Match found: %.*s", (int)(match->length), match->start);
    }

    return REGEX_SUCCESS;
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
    if (regex) {
        pcre_free(regex->compiled_pattern);
        free(regex);
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