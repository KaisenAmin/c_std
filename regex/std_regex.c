/* Regex backend selection. Prefer PCRE when it is available -- always on
 * Windows (which ships <pcre.h> here) and on Linux/Unix whenever libpcre's
 * <pcre.h> is installed -- so BOTH platforms get the full PCRE feature set
 * (lookaround, \d, lazy quantifiers, '.' excluding newline, ...). When
 * <pcre.h> is absent, fall back to POSIX <regex.h> (in libc), which handles
 * basic ERE patterns but not PCRE-only constructs. */
#if CSTD_REGEX_USE_PCRE || __has_include(<pcre.h>)
  #define CSTD_REGEX_USE_PCRE 1
  #include <pcre.h>
#else
  #define CSTD_REGEX_USE_PCRE 0
  #include <regex.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "std_regex.h"



// Internal struct to hold platform-specific regex data
struct Regex {
#if CSTD_REGEX_USE_PCRE
    pcre* compiled_pattern;
    const char* error;
    int erroffset;
#else
    regex_t compiled;   /* POSIX compiled pattern */
    int     valid;      /* 1 once regcomp succeeded, so deallocate is safe */
#endif
};


#if !CSTD_REGEX_USE_PCRE
/* Translate the PCRE escapes this project uses (\d \w \s and their negations)
 * into POSIX ERE bracket expressions, so patterns authored for the Windows
 * PCRE backend keep working against <regex.h>. Any other "\X" passes through
 * unchanged (POSIX ERE treats a backslash before a special char as a literal,
 * matching PCRE for \. \/ etc.). Character-class context is tracked so a \d
 * inside [...] expands to "0-9" rather than a nested class.
 * Returns a malloc'd string the caller must free, or NULL on OOM. */
static char* regex_pcre_to_posix(const char* p) {
    size_t cap = strlen(p) * 8 + 16, len = 0;
    char* out = (char*)malloc(cap);
    if (!out) {
        return NULL;
    }
    int in_class = 0;
    for (size_t i = 0; p[i]; i++) {
        char c = p[i];
        if (c == '\\' && p[i + 1]) {
            char n = p[i + 1];
            const char* rep = NULL;
            if (!in_class) {
                if      (n == 'd') rep = "[0-9]";
                else if (n == 'D') rep = "[^0-9]";
                else if (n == 'w') rep = "[A-Za-z0-9_]";
                else if (n == 'W') rep = "[^A-Za-z0-9_]";
                else if (n == 's') rep = "[ \t\n\r\f\v]";
                else if (n == 'S') rep = "[^ \t\n\r\f\v]";
            } else {
                if      (n == 'd') rep = "0-9";
                else if (n == 'w') rep = "A-Za-z0-9_";
                else if (n == 's') rep = " \t\n\r\f\v";
            }
            if (rep) {
                size_t rl = strlen(rep);
                while (len + rl + 1 > cap) { cap *= 2; char* nb = (char*)realloc(out, cap); if (!nb) { free(out); return NULL; } out = nb; }
                memcpy(out + len, rep, rl);
                len += rl;
                i++;            /* consume the escape letter */
                continue;
            }
            /* keep "\X" verbatim */
            while (len + 3 > cap) { cap *= 2; char* nb = (char*)realloc(out, cap); if (!nb) { free(out); return NULL; } out = nb; }
            out[len++] = '\\';
            out[len++] = n;
            i++;
            continue;
        }
        if (c == '[') in_class = 1;
        else if (c == ']') in_class = 0;
        while (len + 2 > cap) { cap *= 2; char* nb = (char*)realloc(out, cap); if (!nb) { free(out); return NULL; } out = nb; }
        out[len++] = c;
    }
    out[len] = '\0';
    return out;
}
#endif


/**
 * @brief Compiles the given regular expression pattern into a Regex object.
 * 
 * @param pattern The regex pattern to compile.
 * @param flags Optional flags for regex compilation (e.g., REGEX_CASE_INSENSITIVE).
 * @return A pointer to a Regex object, or NULL if compilation failed.
 */
Regex* regex_compile(const char* pattern, RegexFlags flags) {
    /* NULL-check up front: pcre_compile(NULL, ...) is undefined behaviour
       and segfaults under glibc / MSYS pcre. Document and guard it here
       so callers can treat regex_compile(NULL, ...) as a normal error. */
    if (!pattern) {
        REGEX_LOG("[regex_compile] NULL pattern.");
        return NULL;
    }
    REGEX_LOG("[regex_compile] Compiling pattern: %s", pattern);

    Regex* regex = (Regex*)malloc(sizeof(Regex));
    if (!regex) {
        REGEX_LOG("[regex_compile] Memory allocation failed.");
        return NULL;
    }

#if CSTD_REGEX_USE_PCRE
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
#else
    /* POSIX backend: translate PCRE-isms, then regcomp with ERE semantics. */
    char* posix_pat = regex_pcre_to_posix(pattern);
    if (!posix_pat) {
        REGEX_LOG("[regex_compile] Pattern translation failed (OOM).");
        free(regex);
        return NULL;
    }
    int cflags = REG_EXTENDED;
    if (flags & REGEX_CASE_INSENSITIVE) {
        cflags |= REG_ICASE;
    }
    if (flags & REGEX_MULTILINE) {
        cflags |= REG_NEWLINE;
    }
    /* REGEX_DOTALL: with REG_NEWLINE off (the default here) POSIX '.' already
       matches newline, so no extra flag is required. */
    int reg_rc = regcomp(&regex->compiled, posix_pat, cflags);
    free(posix_pat);
    if (reg_rc != 0) {
        REGEX_LOG("[regex_compile] POSIX regcomp failed (code %d).", reg_rc);
        free(regex);
        return NULL;
    }
    regex->valid = 1;
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
    if (!regex || !string) {
        REGEX_LOG("[regex_match] NULL regex or input string.");
        return REGEX_MATCH_ERROR;
    }
    REGEX_LOG("[regex_match] Matching string: %s", string);

    /* Make the match always safe to pass to regex_match_free(), even on the
       no-match / error paths below where the group buffers are never set. */
    if (match) {
        match->group_starts  = NULL;
        match->group_lengths = NULL;
        match->group_count   = 0;
    }

#if CSTD_REGEX_USE_PCRE
    if (!regex->compiled_pattern) {
        return REGEX_MATCH_ERROR;
    }
    int ovector[30];  // Array for storing the start and end positions of the matches
    int rc = pcre_exec(regex->compiled_pattern, NULL, string, (int)strlen(string), 0, 0, ovector, 30);

    if (rc < 0) {
        REGEX_LOG("[regex_match] Match failed with code %d", rc);
        return (rc == PCRE_ERROR_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }

    if (match) {
        match->start = string + ovector[0];
        match->end = string + ovector[1];
        match->length = (size_t)(ovector[1] - ovector[0]);
        match->group_count = rc - 1;  // Subtract 1 because rc includes the whole match as group 0


        if (match->group_count > 0) {
            match->group_starts = (const char**) malloc((size_t)match->group_count * sizeof(char*));
            match->group_lengths = (size_t*) malloc((size_t)match->group_count * sizeof(size_t));
            if (!match->group_starts || !match->group_lengths) {
                REGEX_LOG("[regex_match] OOM allocating group buffers.");
                free((void*)match->group_starts);
                free(match->group_lengths);

                match->group_starts = NULL;
                match->group_lengths = NULL;
                match->group_count = 0;

                return REGEX_MATCH_ERROR;
            }
            for (int i = 1; i <= match->group_count; i++) {
                match->group_starts[i-1] = string + ovector[2 * i];
                match->group_lengths[i-1] = (size_t)(ovector[2 * i + 1] - ovector[2 * i]);
            }
        }
        else {
            match->group_starts = NULL;
            match->group_lengths = NULL;
        }

        REGEX_LOG("[regex_match] Match found: %.*s", (int)(match->length), match->start);
    }
#else
    size_t ngroups = regex->compiled.re_nsub;
    size_t nmatch  = ngroups + 1;
    regmatch_t* pm = (regmatch_t*) malloc(nmatch * sizeof(regmatch_t));
    if (!pm) {
        REGEX_LOG("[regex_match] OOM allocating match buffer.");
        return REGEX_MATCH_ERROR;
    }
    int rc = regexec(&regex->compiled, string, nmatch, pm, 0);
    if (rc != 0) {
        REGEX_LOG("[regex_match] Match failed with code %d", rc);
        free(pm);
        return (rc == REG_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }

    if (match) {
        match->start  = string + pm[0].rm_so;
        match->end    = string + pm[0].rm_eo;
        match->length = (size_t)(pm[0].rm_eo - pm[0].rm_so);
        match->group_count = (int)ngroups;

        if (ngroups > 0) {
            match->group_starts  = (const char**) malloc(ngroups * sizeof(char*));
            match->group_lengths = (size_t*)      malloc(ngroups * sizeof(size_t));
            if (!match->group_starts || !match->group_lengths) {
                REGEX_LOG("[regex_match] OOM allocating group buffers.");
                free((void*)match->group_starts);
                free(match->group_lengths);
                match->group_starts = NULL;
                match->group_lengths = NULL;
                match->group_count = 0;
                free(pm);
                return REGEX_MATCH_ERROR;
            }
            for (size_t i = 1; i <= ngroups; i++) {
                if (pm[i].rm_so >= 0) {
                    match->group_starts[i-1]  = string + pm[i].rm_so;
                    match->group_lengths[i-1] = (size_t)(pm[i].rm_eo - pm[i].rm_so);
                }
                else {   /* group did not participate in the match */
                    match->group_starts[i-1]  = string;
                    match->group_lengths[i-1] = 0;
                }
            }
        }
        else {
            match->group_starts = NULL;
            match->group_lengths = NULL;
        }

        REGEX_LOG("[regex_match] Match found: %.*s", (int)(match->length), match->start);
    }
    free(pm);
#endif

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
    if (!regex || !string) {
        REGEX_LOG("[regex_search] NULL regex or input string.");
        return REGEX_MATCH_ERROR;
    }
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
    if (!regex || !string || !matches || max_matches <= 0) {
        REGEX_LOG("[regex_find_all] Invalid arguments.");
        return 0;
    }
    REGEX_LOG("[regex_find_all] Finding all matches in string: %s", string);

    int count = 0;
    const char* current = string;
    RegexMatch temp_match;

    while (count < max_matches && regex_match(regex, current, &temp_match) == REGEX_SUCCESS) {
        matches[count] = temp_match;
        REGEX_LOG("[regex_find_all] Match %d found: %.*s", count + 1, (int)temp_match.length, temp_match.start);
        count++;

        // Guard against zero-length match (e.g. pattern "a*" matching ""):
        // without this, current never advances and we loop forever.
        if (temp_match.end == temp_match.start) {
            if (*temp_match.end == '\0') {
                break;  // end of string anyway
            }
            current = temp_match.end + 1;        // skip a char to make progress
        }
        else {
            current = temp_match.end;
        }
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
    REGEX_LOG("[regex_deallocate]: enter (regex=%p)", (void*)regex);
    if (regex) {
#if CSTD_REGEX_USE_PCRE
        pcre_free(regex->compiled_pattern);
#else
        if (regex->valid) {
            regfree(&regex->compiled);
        }
#endif
        free(regex);
        REGEX_LOG("[regex_deallocate]: freed compiled pattern and Regex");
    }
    else {
        REGEX_LOG("[regex_deallocate]: NULL receiver, no-op");
    }
}


/* Frees the per-match group buffers allocated by regex_match / regex_search /
 * regex_find_all. Safe to call with a zeroed or already-freed RegexMatch.
 * For arrays of matches (e.g. from regex_find_all), call once per entry. */
void regex_match_free(RegexMatch* match) {
    REGEX_LOG("[regex_match_free]: enter (match=%p)", (void*)match);
    if (!match) {
        REGEX_LOG("[regex_match_free]: NULL match, no-op");
        return;
    }

    free((void*)match->group_starts);
    free(match->group_lengths);

    match->group_starts = NULL;
    match->group_lengths = NULL;
    match->group_count = 0;
    REGEX_LOG("[regex_match_free]: group buffers released");
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


/**
 * @brief Returns 1 if @p string contains at least one match for @p pattern,
 *        0 if it doesn't, and -1 on compile / NULL-arg error.
 *
 * Wraps compile -> search -> deallocate so callers don't have to manage
 * a Regex lifetime for one-shot membership tests. Match groups are not
 * captured (the function does not allocate a RegexMatch).
 *
 * @code
 * if (regex_test("[0-9]+", "alpha42beta", REGEX_DEFAULT) == 1) {
 *     // string contains a digit run
 * }
 * @endcode
 */
int regex_test(const char* pattern, const char* string, RegexFlags flags) {
    REGEX_LOG("[regex_test]: enter (pattern='%s', flags=0x%X)",
              pattern ? pattern : "(null)", (unsigned)flags);
    if (!pattern || !string) {
        REGEX_LOG("[regex_test]: NULL input -> -1");
        return -1;
    }

    Regex* r = regex_compile(pattern, flags);
    if (!r) {
        REGEX_LOG("[regex_test]: compile failed -> -1");
        return -1;
    }

    RegexResult rr = regex_search(r, string, NULL);
    regex_deallocate(r);
    int out = (rr == REGEX_SUCCESS) ? 1 : (rr == REGEX_NO_MATCH ? 0 : -1);

    REGEX_LOG("[regex_test]: exit -> %d", out);
    return out;
}

/**
 * @brief Counts every non-overlapping occurrence of @p pattern in @p string.
 *
 * Zero-length matches (e.g. `\b`, `^$`) are handled by advancing one byte
 * per zero-width hit, so the walk always terminates.
 *
 * @code
 * int n = regex_count_matches("[aeiou]", "production-ready", REGEX_DEFAULT);
 * // n == 6
 * @endcode
 *
 * @return Number of matches, or -1 on NULL inputs / compile error.
 */
int regex_count_matches(const char* pattern, const char* string, RegexFlags flags) {
    REGEX_LOG("[regex_count_matches]: enter (pattern='%s', flags=0x%X)",
              pattern ? pattern : "(null)", (unsigned)flags);
    if (!pattern || !string) {
        REGEX_LOG("[regex_count_matches]: NULL input -> -1");
        return -1;
    }

    Regex* r = regex_compile(pattern, flags);
    if (!r) {
        REGEX_LOG("[regex_count_matches]: compile failed -> -1");
        return -1;
    }

    int count = 0;
    const char* cursor = string;
    RegexMatch m;

    while (regex_match(r, cursor, &m) == REGEX_SUCCESS) {
        count++;
        /* Snapshot the post-match position BEFORE freeing the per-match
           buffers. Advancing by `m.length` alone is wrong because the
           match may start at an offset within `cursor` — we have to jump
           past the match, not by its length. */
        const char* match_end = m.end;
        size_t consumed = m.length;
        regex_match_free(&m);

        if (consumed == 0) {
            if (*cursor == '\0') {
                break;
            }
            cursor++;
        }
        else {
            cursor = match_end;
        }
    }
    regex_deallocate(r);
    REGEX_LOG("[regex_count_matches]: exit -> %d", count);
    return count;
}

/**
 * @brief Internal: append @p n bytes of @p src to a growing buffer.
 *
 * Maintains a doubling capacity scheme so the amortized cost per byte
 * is O(1). Returns true on success, false on OOM (the buffer is left in
 * a valid state with its existing contents and the caller can decide
 * to free it).
 */
static int regex_buf_append(char** buf, size_t* len, size_t* cap, const char* src, size_t n) {
    if (*len + n + 1 > *cap) {
        size_t newcap = *cap ? *cap : 64;
        while (newcap < *len + n + 1) {
            newcap *= 2;
        }

        char* nb = (char*)realloc(*buf, newcap);
        if (!nb) {
            return 0;
        }
        *buf = nb;
        *cap = newcap;
    }
    if (n) {
        memcpy(*buf + *len, src, n);
    }

    *len += n;
    (*buf)[*len] = '\0';
    return 1;
}


/**
 * @brief Internal: shared engine for regex_replace and regex_replace_first.
 *
 * @param max_replacements -1 means "replace all". Any other non-negative
 *                         value caps the number of substitutions.
 */
static char* regex_replace_impl(const char* pattern, const char* string, const char* replacement, RegexFlags flags, int max_replacements) {
    if (!pattern || !string || !replacement) {
        return NULL;
    }

    Regex* r = regex_compile(pattern, flags);
    if (!r) {
        return NULL;
    }

    char* out   = NULL;
    size_t outl = 0;
    size_t outc = 0;

    const char* cursor    = string;
    size_t replacements   = 0;
    size_t replacement_n  = strlen(replacement);

    RegexMatch m;
    while ((max_replacements < 0 || (int)replacements < max_replacements) &&
           regex_match(r, cursor, &m) == REGEX_SUCCESS) {
        /* Bytes from cursor up to the match start. */
        size_t prefix = (size_t)(m.start - cursor);
        if (!regex_buf_append(&out, &outl, &outc, cursor, prefix)) {
            regex_match_free(&m); 
            regex_deallocate(r); 
            free(out); 
            return NULL;
        }
        /* The replacement. */
        if (!regex_buf_append(&out, &outl, &outc, replacement, replacement_n)) {
            regex_match_free(&m); 
            regex_deallocate(r); 
            free(out); 
            return NULL;
        }
        replacements++;
        cursor = m.end;
        size_t consumed = m.length;
        regex_match_free(&m);
        /* Zero-length match: avoid infinite loop. */
        if (consumed == 0) {
            if (*cursor == '\0') {
                break;
            }
            /* Copy the un-replaced byte and advance. */
            if (!regex_buf_append(&out, &outl, &outc, cursor, 1)) {
                regex_deallocate(r); 
                free(out); 
                return NULL;
            }
            cursor++;
        }
    }
    /* Append the rest of the string. */
    if (!regex_buf_append(&out, &outl, &outc, cursor, strlen(cursor))) {
        regex_deallocate(r); 
        free(out); 
        return NULL;
    }
    if (!out) {

        out = (char*)malloc(1);
        if (out) {
            out[0] = '\0';
        }
    }
    regex_deallocate(r);
    return out;
}


/**
 * @brief Returns a freshly-allocated copy of @p string with every match
 *        of @p pattern replaced by the literal text @p replacement.
 *
 * Backreferences (`$1`, `\1`) are NOT expanded — replacement is treated
 * as literal text. Use @ref regex_replace_first for single-occurrence
 * replacement.
 *
 * @code
 * char* out = regex_replace("[aeiou]", "production", "*", REGEX_DEFAULT);
 * // out -> "pr*d*ct**n"
 * free(out);
 * @endcode
 *
 * @return Newly-allocated result string (caller must `free`), or NULL on
 *         compile failure / OOM / NULL inputs.
 */
char* regex_replace(const char* pattern, const char* string, const char* replacement, RegexFlags flags) {
    REGEX_LOG("[regex_replace]: enter (pattern='%s', flags=0x%X, replace_all=true)",
              pattern ? pattern : "(null)", (unsigned)flags);
    char* out = regex_replace_impl(pattern, string, replacement, flags, -1);
    REGEX_LOG("[regex_replace]: exit -> %p", (void*)out);
    return out;
}


/**
 * @brief Same as @ref regex_replace but replaces only the FIRST match.
 *
 * @code
 * char* out = regex_replace_first("[0-9]+", "abc123def456", "###", REGEX_DEFAULT);
 * // out -> "abc###def456"
 * free(out);
 * @endcode
 */
char* regex_replace_first(const char* pattern, const char* string, const char* replacement, RegexFlags flags) {
    REGEX_LOG("[regex_replace_first]: enter (pattern='%s', flags=0x%X, replace_all=false)",
              pattern ? pattern : "(null)", (unsigned)flags);
    char* out = regex_replace_impl(pattern, string, replacement, flags, 1);
    REGEX_LOG("[regex_replace_first]: exit -> %p", (void*)out);
    return out;
}


/**
 * @brief Splits @p string at every non-empty match of @p pattern.
 *
 * The output is a NULL-terminated array of malloc'd C strings (one per
 * token, in order). @p out_count is filled with the number of tokens
 * (excluding the trailing NULL). Empty tokens are emitted at adjacent
 * separator positions — matching the behaviour of Python's `re.split`.
 *
 * @code
 * int n = 0;
 * char** parts = regex_split(",\\s*", "alpha, beta ,  gamma", REGEX_DEFAULT, &n);
 * // n == 3; parts[0]="alpha", parts[1]="beta", parts[2]="gamma", parts[3]=NULL
 * regex_split_free(parts, n);
 * @endcode
 *
 * @return Token array (caller frees via @ref regex_split_free), or NULL
 *         on compile failure / NULL inputs.
 */
char** regex_split(const char* pattern, const char* string, RegexFlags flags, int* out_count) {
    REGEX_LOG("[regex_split]: enter (pattern='%s', flags=0x%X)",
              pattern ? pattern : "(null)", (unsigned)flags);
    if (out_count) {
        *out_count = 0;
    }
    if (!pattern || !string) {
        REGEX_LOG("[regex_split]: NULL input -> NULL");
        return NULL;
    }

    Regex* r = regex_compile(pattern, flags);
    if (!r) {
        REGEX_LOG("[regex_split]: compile failed -> NULL");
        return NULL;
    }

    char** tokens = NULL;
    int     ntokens = 0;
    int     cap = 0;

    const char* cursor = string;
    RegexMatch m;
    while (regex_match(r, cursor, &m) == REGEX_SUCCESS) {
        size_t consumed = m.length;
        /* Zero-length match would never advance — skip these for split. */
        if (consumed == 0) {
            regex_match_free(&m);
            if (*cursor == '\0') {
                break;
            }
            cursor++;
            continue;
        }

        /* Token = bytes from cursor up to m.start. */
        size_t tlen = (size_t)(m.start - cursor);
        char* tok = (char*)malloc(tlen + 1);
        if (!tok) {
            regex_match_free(&m);
            for (int i = 0; i < ntokens; i++) {
                free(tokens[i]);
            }

            free(tokens);
            regex_deallocate(r);
            return NULL;
        }
        memcpy(tok, cursor, tlen);
        tok[tlen] = '\0';

        if (ntokens + 1 >= cap) {
            int newcap = cap ? cap * 2 : 8;
            char** nt = (char**)realloc(tokens, (size_t)newcap * sizeof(char*));
            if (!nt) {
                free(tok);
                regex_match_free(&m);
                for (int i = 0; i < ntokens; i++) {
                    free(tokens[i]);
                }

                free(tokens);
                regex_deallocate(r);
                return NULL;
            }
            tokens = nt;
            cap = newcap;
        }
        tokens[ntokens++] = tok;
        cursor = m.end;
        regex_match_free(&m);
    }

    /* Last token = remainder of string. */
    size_t tail_len = strlen(cursor);
    char* tail = (char*)malloc(tail_len + 1);
    if (!tail) {
        for (int i = 0; i < ntokens; i++) {
            free(tokens[i]);
        }
        free(tokens);
        regex_deallocate(r);
        return NULL;
    }
    memcpy(tail, cursor, tail_len + 1);

    if (ntokens + 2 >= cap) {
        int newcap = cap ? cap + 2 : 8;
        char** nt = (char**)realloc(tokens, (size_t)newcap * sizeof(char*));
        
        if (!nt) {
            free(tail);
            for (int i = 0; i < ntokens; i++) {
                free(tokens[i]);
            }

            free(tokens);
            regex_deallocate(r);
            return NULL;
        }
        tokens = nt;
    }
    tokens[ntokens++] = tail;
    tokens[ntokens]   = NULL;

    regex_deallocate(r);
    if (out_count) {
        *out_count = ntokens;
    }
    REGEX_LOG("[regex_split]: exit -> %d token(s)", ntokens);
    return tokens;
}


/**
 * @brief Frees the token array returned by @ref regex_split. NULL-safe.
 *
 * @param tokens Token array from @ref regex_split.
 * @param count  Number of tokens (the second out-arg from
 *               @ref regex_split). Passing -1 will scan for the NULL
 *               sentinel instead.
 */
void regex_split_free(char** tokens, int count) {
    REGEX_LOG("[regex_split_free]: enter (tokens=%p, count=%d)", (void*)tokens, count);
    if (!tokens) {
        REGEX_LOG("[regex_split_free]: NULL tokens, no-op");
        return;
    }
    if (count < 0) {
        for (int i = 0; tokens[i] != NULL; i++) {
            free(tokens[i]);
        }
    }
    else {
        for (int i = 0; i < count; i++) {
            free(tokens[i]);
        }
    }

    free(tokens);
    REGEX_LOG("[regex_split_free]: exit");
}


/**
 * @brief Copies the @p group_index group of @p match into a freshly-
 *        allocated, NUL-terminated C string.
 *
 * `group_index == 0` returns the whole match; indices `1..group_count`
 * return the corresponding capturing group. Out-of-range indices return
 * NULL without crashing.
 *
 * @code
 * Regex* r = regex_compile("(\\d{4})-(\\d{2})", REGEX_DEFAULT);
 * RegexMatch m;
 * regex_search(r, "Build 2026-05 finished", &m);
 * char* g0 = regex_match_group(&m, 0);  // "2026-05"
 * char* g1 = regex_match_group(&m, 1);  // "2026"
 * char* g2 = regex_match_group(&m, 2);  // "05"
 * free(g0); free(g1); free(g2);
 * regex_match_free(&m);
 * regex_deallocate(r);
 * @endcode
 *
 * @return Newly-allocated copy of the group text, or NULL on bad input.
 *         Caller must `free()`.
 */
char* regex_match_group(const RegexMatch* match, int group_index) {
    REGEX_LOG("[regex_match_group]: enter (match=%p, group_index=%d)",
              (const void*)match, group_index);
    if (!match) {
        REGEX_LOG("[regex_match_group]: NULL match -> NULL");
        return NULL;
    }

    const char* start;
    size_t len;

    if (group_index == 0) {
        start = match->start;
        len   = match->length;
    }
    else if (group_index > 0 && group_index <= match->group_count && match->group_starts && match->group_lengths) {
        start = match->group_starts[group_index - 1];
        len   = match->group_lengths[group_index - 1];
    }
    else {
        REGEX_LOG("[regex_match_group]: group_index %d out of range (group_count=%d) -> NULL",
                  group_index, match->group_count);
        return NULL;
    }

    char* out = (char*)malloc(len + 1);
    if (!out) {
        REGEX_LOG("[regex_match_group]: malloc(%zu) failed -> NULL", len + 1);
        return NULL;
    }
    if (len) {
        memcpy(out, start, len);
    }

    out[len] = '\0';
    REGEX_LOG("[regex_match_group]: exit -> %p (len=%zu)", (void*)out, len);
    return out;
}