/* Regex backend selection. Prefer PCRE when it is available -- always on
 * Windows (which ships <pcre.h> here) and on Linux/Unix whenever libpcre's
 * <pcre.h> is installed -- so BOTH platforms get the full PCRE feature set
 * (lookaround, \d, lazy quantifiers, ...). When <pcre.h> is absent, fall
 * back to POSIX <regex.h> (in libc), which handles ERE patterns but not
 * PCRE-only constructs.
 *
 * Define CSTD_REGEX_USE_PCRE to 1 or 0 to force a backend (it used to be
 * impossible to force POSIX while <pcre.h> was installed). Neither header
 * available (e.g. MSVC without PCRE) is a clear build error. */
#ifndef CSTD_REGEX_USE_PCRE
  #if defined(__has_include)
    #if __has_include(<pcre.h>)
      #define CSTD_REGEX_USE_PCRE 1
    #else
      #define CSTD_REGEX_USE_PCRE 0
    #endif
  #else
    #define CSTD_REGEX_USE_PCRE 1
  #endif
#endif

#if CSTD_REGEX_USE_PCRE
  #include <pcre.h>
#else
  #if defined(__has_include)
    #if !__has_include(<regex.h>)
      #error "c_std regex: needs PCRE (<pcre.h>, link -lpcre) or POSIX <regex.h>; neither was found. Install PCRE (see regex/README.md)."
    #endif
  #endif
  #include <regex.h>
  #include <langinfo.h>   /* nl_langinfo(CODESET) for REGEX_UTF8 */
#endif
#include <limits.h>
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
    int     newline;    /* compiled with REG_NEWLINE (REGEX_MULTILINE) */
#endif
    int     utf8;       /* REGEX_UTF8: scans step over whole UTF-8 characters */
};


/* Option for regex_exec_at: only accept a non-empty match that starts exactly
   at the offset. Used right after an empty match, so a scan makes progress
   without skipping a real match there (the standard PCRE scan loop). */
#define REGEX_EXEC_NONEMPTY_AT_START 1


#if !CSTD_REGEX_USE_PCRE
/* Appends n bytes to the growing translation buffer; 0 on OOM. */
static int regex_out_put(char** out, size_t* len, size_t* cap, const char* s, size_t n) {
    while (*len + n + 1 > *cap) {
        size_t nc = *cap * 2;
        char* nb = (char*)realloc(*out, nc);
        if (!nb) {
            return 0;
        }
        *out = nb;
        *cap = nc;
    }
    memcpy(*out + *len, s, n);
    *len += n;
    return 1;
}

/* Translates PCRE-isms (\d \w \s and their negations) into POSIX ERE. With
   `dot_excludes_newline`, an unescaped '.' outside brackets becomes "[^\n]",
   so '.' does not match a newline unless REGEX_DOTALL is given, as in PCRE
   (POSIX '.' matches every character when REG_NEWLINE is off). Bracket
   expressions are copied as one unit, with POSIX rules for where they end:
   a ']' right after '[' or '[^' is literal, and [:class:], [.coll.] and
   [=equiv=] are skipped whole. Any other "\X" passes through unchanged
   (POSIX ERE treats a backslash before a special char as a literal, matching
   PCRE for \. \/ etc.). Returns a malloc'd string, or NULL on OOM. */
static char* regex_pcre_to_posix(const char* p, int dot_excludes_newline) {
    size_t cap = strlen(p) * 8 + 16, len = 0;
    char* out = (char*)malloc(cap);
    if (!out) {
        return NULL;
    }

    for (size_t i = 0; p[i]; i++) {
        char c = p[i];
        const char* rep = NULL;

        if (c == '\\' && p[i + 1]) {
            char n = p[i + 1];
            if      (n == 'd') rep = "[0-9]";
            else if (n == 'D') rep = "[^0-9]";
            else if (n == 'w') rep = "[A-Za-z0-9_]";
            else if (n == 'W') rep = "[^A-Za-z0-9_]";
            else if (n == 's') rep = "[ \t\n\r\f\v]";
            else if (n == 'S') rep = "[^ \t\n\r\f\v]";
            int ok = rep ? regex_out_put(&out, &len, &cap, rep, strlen(rep)) : regex_out_put(&out, &len, &cap, p + i, 2);
            if (!ok) {
                free(out);
                return NULL;
            }
            i++;            /* consume the escaped character */
            continue;
        }

        if (c == '[') {
            /* Copy the bracket expression, expanding \d \w \s inside it. */
            size_t j = i + 1;
            if (p[j] == '^') {
                j++;
            }
            if (p[j] == ']') {
                j++;        /* a leading ']' is a literal member */
            }
            if (!regex_out_put(&out, &len, &cap, p + i, j - i)) {
                free(out);
                return NULL;
            }
            while (p[j] && p[j] != ']') {
                if (p[j] == '[' && (p[j + 1] == ':' || p[j + 1] == '.' || p[j + 1] == '=')) {
                    char kind = p[j + 1];
                    size_t k = j + 2;
                    while (p[k] && !(p[k] == kind && p[k + 1] == ']')) {
                        k++;
                    }
                    if (!p[k]) {
                        break;      /* unterminated: let regcomp report it */
                    }
                    if (!regex_out_put(&out, &len, &cap, p + j, k + 2 - j)) {
                        free(out);
                        return NULL;
                    }
                    j = k + 2;
                    continue;
                }
                if (p[j] == '\\' && (p[j + 1] == 'd' || p[j + 1] == 'w' || p[j + 1] == 's')) {
                    const char* in = p[j + 1] == 'd' ? "0-9" : p[j + 1] == 'w' ? "A-Za-z0-9_" : " \t\n\r\f\v";
                    if (!regex_out_put(&out, &len, &cap, in, strlen(in))) {
                        free(out);
                        return NULL;
                    }
                    j += 2;
                    continue;
                }
                if (!regex_out_put(&out, &len, &cap, p + j, 1)) {
                    free(out);
                    return NULL;
                }
                j++;
            }
            if (!p[j]) {
                i = j - 1;      /* unterminated bracket: copied as is */
                continue;
            }
            if (!regex_out_put(&out, &len, &cap, "]", 1)) {
                free(out);
                return NULL;
            }
            i = j;
            continue;
        }

        if (c == '.' && dot_excludes_newline) {
            rep = "[^\n]";
        }
        if (!(rep ? regex_out_put(&out, &len, &cap, rep, strlen(rep)) : regex_out_put(&out, &len, &cap, &c, 1))) {
            free(out);
            return NULL;
        }
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
    regex->utf8 = (flags & REGEX_UTF8) != 0;

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
    if (flags & REGEX_UTF8) {
        /* Without it PCRE works on bytes: '.' is one byte of a multi-byte
           character and caseless matching folds ASCII only. */
        int utf8_supported = 0;
        if (pcre_config(PCRE_CONFIG_UTF8, &utf8_supported) != 0 || !utf8_supported) {
            REGEX_LOG("[regex_compile] This PCRE build has no UTF-8 support.");
            free(regex);
            return NULL;
        }
        pcre_flags |= PCRE_UTF8;
    }

    regex->compiled_pattern = pcre_compile(pattern, pcre_flags, &error, &erroffset, NULL);

    if (!regex->compiled_pattern) {
        REGEX_LOG("[regex_compile] PCRE compilation failed at offset %d: %s", erroffset, error);
        free(regex);
        return NULL;
    }
#else
    /* POSIX backend: translate PCRE-isms, then regcomp with ERE semantics.
       MULTILINE needs REG_NEWLINE for line anchors, and REG_NEWLINE also
       stops '.' from matching a newline, so MULTILINE | DOTALL cannot be
       expressed: refuse it instead of silently dropping DOTALL. */
    if ((flags & REGEX_MULTILINE) && (flags & REGEX_DOTALL)) {
        REGEX_LOG("[regex_compile] REGEX_MULTILINE | REGEX_DOTALL is not supported by the POSIX backend.");
        free(regex);
        return NULL;
    }
    if (flags & REGEX_UTF8) {
        /* POSIX regex reads characters in the current LC_CTYPE encoding, so
           UTF-8 semantics need a UTF-8 locale (setlocale(LC_CTYPE, "")). */
        const char* codeset = nl_langinfo(CODESET);
        if (!codeset || (strcmp(codeset, "UTF-8") != 0 && strcmp(codeset, "utf8") != 0)) {
            REGEX_LOG("[regex_compile] REGEX_UTF8 needs a UTF-8 LC_CTYPE locale on the POSIX backend.");
            free(regex);
            return NULL;
        }
    }
    char* posix_pat = regex_pcre_to_posix(pattern, !(flags & REGEX_DOTALL));
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
    /* REGEX_DOTALL: POSIX '.' matches a newline when REG_NEWLINE is off, so
       the translator leaves '.' alone; without DOTALL it became "[^\n]". */
    int reg_rc = regcomp(&regex->compiled, posix_pat, cflags);
    free(posix_pat);
    if (reg_rc != 0) {
        REGEX_LOG("[regex_compile] POSIX regcomp failed (code %d).", reg_rc);
        free(regex);
        return NULL;
    }
    regex->valid = 1;
    regex->newline = (cflags & REG_NEWLINE) != 0;
#endif

    REGEX_LOG("[regex_compile] Compilation successful.");
    return regex;
}


/* Fills the group arrays of `match` from (start, end) offset pairs into
   `subject`; a pair with start < 0 is a group that did not participate and
   is reported as an empty group at the start of the whole match. */
static RegexResult regex_fill_groups(RegexMatch* match, const char* subject, int ngroups, const long* so, const long* eo) {
    match->group_count = ngroups;
    if (ngroups <= 0) {
        match->group_count = 0;
        return REGEX_SUCCESS;
    }
    match->group_starts  = (const char**) malloc((size_t)ngroups * sizeof(char*));
    match->group_lengths = (size_t*)      malloc((size_t)ngroups * sizeof(size_t));
    if (!match->group_starts || !match->group_lengths) {
        REGEX_LOG("[regex_fill_groups] OOM allocating group buffers.");
        free((void*)match->group_starts);
        free(match->group_lengths);
        match->group_starts = NULL;
        match->group_lengths = NULL;
        match->group_count = 0;
        return REGEX_MATCH_ERROR;
    }
    for (int i = 0; i < ngroups; i++) {
        if (so[i] >= 0 && eo[i] >= so[i]) {
            match->group_starts[i]  = subject + so[i];
            match->group_lengths[i] = (size_t)(eo[i] - so[i]);
        }
        else {
            match->group_starts[i]  = match->start;
            match->group_lengths[i] = 0;
        }
    }
    return REGEX_SUCCESS;
}


/**
 * Internal: searches `subject` (of length `subject_len`) for the first match
 * that starts at or after byte `offset`.
 *
 * Unlike matching on `subject + offset`, the text before `offset` stays
 * visible: '^' and \A do not match in the middle of the subject, and \b and
 * lookbehind see the preceding characters. Every scanning helper (find_all,
 * count, replace, split) walks the subject with this function.
 *
 * With REGEX_EXEC_NONEMPTY_AT_START only a non-empty match starting exactly
 * at `offset` is accepted.
 */
static RegexResult regex_exec_at(Regex* regex, const char* subject, size_t subject_len, size_t offset, int options, RegexMatch* match) {
    /* Make the match always safe to pass to regex_match_free(), even on the
       no-match / error paths below where the group buffers are never set. */
    if (match) {
        match->group_starts  = NULL;
        match->group_lengths = NULL;
        match->group_count   = 0;
    }
    if (offset > subject_len) {
        return REGEX_NO_MATCH;
    }

#if CSTD_REGEX_USE_PCRE
    if (!regex->compiled_pattern) {
        return REGEX_MATCH_ERROR;
    }
    if (subject_len > (size_t)INT_MAX) {
        REGEX_LOG("[regex_exec_at] Subject too long for PCRE (%zu bytes).", subject_len);
        return REGEX_MATCH_ERROR;
    }

    int capture_count = 0;
    if (pcre_fullinfo(regex->compiled_pattern, NULL, PCRE_INFO_CAPTURECOUNT, &capture_count) != 0 || capture_count < 0) {
        return REGEX_MATCH_ERROR;
    }
    /* Room for every group, so no group is silently dropped. */
    int ovecsize = (capture_count + 1) * 3;
    int stack_ovector[30];
    int* ovector = stack_ovector;
    if (ovecsize > 30) {
        ovector = (int*)malloc((size_t)ovecsize * sizeof(int));
        if (!ovector) {
            return REGEX_MATCH_ERROR;
        }
    }
    else {
        ovecsize = 30;
    }

    int pcre_options = 0;
    if (options & REGEX_EXEC_NONEMPTY_AT_START) {
        pcre_options |= PCRE_NOTEMPTY_ATSTART | PCRE_ANCHORED;
    }
    if (regex->utf8 && offset > 0) {
        /* Calls past offset 0 come only from a scan whose first call (at
           offset 0) already checked the whole subject, and the scan keeps
           offsets on character boundaries: skip the O(n) re-check. An
           invalid subject fails that first call (REGEX_MATCH_ERROR). */
        pcre_options |= PCRE_NO_UTF8_CHECK;
    }
    int rc = pcre_exec(regex->compiled_pattern, NULL, subject, (int)subject_len, (int)offset, pcre_options, ovector, ovecsize);

    if (rc < 0) {
        REGEX_LOG("[regex_exec_at] Match failed with code %d", rc);
        if (ovector != stack_ovector) {
            free(ovector);
        }
        return (rc == PCRE_ERROR_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }

    RegexResult result = REGEX_SUCCESS;
    if (match) {
        match->start  = subject + ovector[0];
        match->end    = subject + ovector[1];
        match->length = (size_t)(ovector[1] - ovector[0]);

        long so[64], eo[64];
        long* gso = so;
        long* geo = eo;
        if (capture_count > 64) {
            gso = (long*)malloc((size_t)capture_count * sizeof(long));
            geo = (long*)malloc((size_t)capture_count * sizeof(long));
        }
        if (!gso || !geo) {
            result = REGEX_MATCH_ERROR;
        }
        else {
            for (int i = 1; i <= capture_count; i++) {
                /* Groups at index >= rc did not participate (pcre leaves them unset). */
                gso[i - 1] = (i < rc) ? ovector[2 * i] : -1;
                geo[i - 1] = (i < rc) ? ovector[2 * i + 1] : -1;
            }
            result = regex_fill_groups(match, subject, capture_count, gso, geo);
        }
        if (gso != so) {
            free(gso);
        }
        if (geo != eo) {
            free(geo);
        }
        REGEX_LOG("[regex_exec_at] Match found: %.*s", (int)(match->length), match->start);
    }
    if (ovector != stack_ovector) {
        free(ovector);
    }
    return result;
#else
    size_t ngroups = regex->compiled.re_nsub;
    size_t nmatch  = ngroups + 1;
    regmatch_t* pm = (regmatch_t*) malloc(nmatch * sizeof(regmatch_t));
    if (!pm) {
        REGEX_LOG("[regex_exec_at] OOM allocating match buffer.");
        return REGEX_MATCH_ERROR;
    }
    /* '^' must not match at the offset unless it really is a line start. */
    int eflags = 0;
    if (offset > 0 && !(regex->newline && subject[offset - 1] == '\n')) {
        eflags |= REG_NOTBOL;
    }
#ifdef REG_STARTEND
    /* Search the whole subject from `offset`, so word boundaries can see the
       character before it; offsets come back relative to `subject`. */
    const char* base = subject;
    pm[0].rm_so = (regoff_t)offset;
    pm[0].rm_eo = (regoff_t)subject_len;
    int rc = regexec(&regex->compiled, subject, nmatch, pm, eflags | REG_STARTEND);
#else
    const char* base = subject + offset;
    int rc = regexec(&regex->compiled, base, nmatch, pm, eflags);
#endif
    if (rc != 0) {
        REGEX_LOG("[regex_exec_at] Match failed with code %d", rc);
        free(pm);
        return (rc == REG_NOMATCH) ? REGEX_NO_MATCH : REGEX_MATCH_ERROR;
    }
    /* POSIX returns the longest match at the leftmost position, so if that
       is empty or not at the offset there is no non-empty match there. */
    if ((options & REGEX_EXEC_NONEMPTY_AT_START) &&
        (base + pm[0].rm_so != subject + offset || pm[0].rm_eo == pm[0].rm_so)) {
        free(pm);
        return REGEX_NO_MATCH;
    }

    RegexResult result = REGEX_SUCCESS;
    if (match) {
        match->start  = base + pm[0].rm_so;
        match->end    = base + pm[0].rm_eo;
        match->length = (size_t)(pm[0].rm_eo - pm[0].rm_so);

        long* gso = (long*)malloc((ngroups ? ngroups : 1) * sizeof(long));
        long* geo = (long*)malloc((ngroups ? ngroups : 1) * sizeof(long));
        if (!gso || !geo) {
            result = REGEX_MATCH_ERROR;
        }
        else {
            for (size_t i = 1; i <= ngroups; i++) {
                gso[i - 1] = (pm[i].rm_so >= 0) ? (long)pm[i].rm_so : -1;
                geo[i - 1] = (pm[i].rm_so >= 0) ? (long)pm[i].rm_eo : -1;
            }
            result = regex_fill_groups(match, base, (int)ngroups, gso, geo);
        }
        free(gso);
        free(geo);
        REGEX_LOG("[regex_exec_at] Match found: %.*s", (int)(match->length), match->start);
    }
    free(pm);
    return result;
#endif
}


/* Internal: finds the next match of a left-to-right scan and moves `*pos` past
   it. Start a scan with *pos = 0 and *options = 0. After an empty match the
   next search must not return the same empty match, so it first asks for a
   non-empty match at that position and otherwise steps one byte forward (one
   whole character with REGEX_UTF8).
   Returns 1 with the match in `m` (free it with regex_match_free), or 0 when
   the scan is over; `*rr` is then REGEX_NO_MATCH, or REGEX_MATCH_ERROR if the
   engine failed. */
static int regex_scan_next(Regex* regex, const char* subject, size_t subject_len, size_t* pos, int* options, RegexMatch* m, RegexResult* rr) {
    for (;;) {
        *rr = regex_exec_at(regex, subject, subject_len, *pos, *options, m);
        if (*rr == REGEX_SUCCESS) {
            *pos = (size_t)(m->end - subject);
            *options = (m->length == 0) ? REGEX_EXEC_NONEMPTY_AT_START : 0;
            return 1;
        }
        if (*rr != REGEX_NO_MATCH) {
            return 0;      /* engine error */
        }
        if (*options == 0 || *pos >= subject_len) {
            return 0;      /* nothing more */
        }
        /* No non-empty match at the empty match's position: step forward. */
        (*pos)++;
        while (regex->utf8 && *pos < subject_len && ((unsigned char)subject[*pos] & 0xC0) == 0x80) {
            (*pos)++;   /* never stop inside a UTF-8 character */
        }
        *options = 0;
    }
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
        if (match) {
            match->group_starts  = NULL;
            match->group_lengths = NULL;
            match->group_count   = 0;
        }
        return REGEX_MATCH_ERROR;
    }
    REGEX_LOG("[regex_match] Matching string: %s", string);
    return regex_exec_at(regex, string, strlen(string), 0, 0, match);
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
        return -1;   /* as documented; 0 ("no match") hid the error */
    }
    REGEX_LOG("[regex_find_all] Finding all matches in string: %s", string);

    int count = 0;
    size_t len = strlen(string);
    size_t pos = 0;
    int options = 0;
    RegexResult rr = REGEX_NO_MATCH;
    RegexMatch temp_match;

    /* Scan the original string, so ^, \b and lookbehind keep their meaning
       after the first match. */
    while (count < max_matches && regex_scan_next(regex, string, len, &pos, &options, &temp_match, &rr)) {
        matches[count] = temp_match;
        REGEX_LOG("[regex_find_all] Match %d found: %.*s", count + 1, (int)temp_match.length, temp_match.start);
        count++;
    }
    if (count < max_matches && rr == REGEX_MATCH_ERROR) {
        REGEX_LOG("[regex_find_all] Matching failed after %d match(es).", count);
        for (int i = 0; i < count; i++) {
            regex_match_free(&matches[i]);
        }
        return -1;
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
    size_t len = strlen(string);
    size_t pos = 0;
    int options = 0;
    RegexResult rr = REGEX_NO_MATCH;
    RegexMatch m;

    while (regex_scan_next(r, string, len, &pos, &options, &m, &rr)) {
        count++;
        regex_match_free(&m);
    }
    regex_deallocate(r);
    if (rr == REGEX_MATCH_ERROR) {
        REGEX_LOG("[regex_count_matches]: matching failed -> -1");
        return -1;
    }
    REGEX_LOG("[regex_count_matches]: exit -> %d", count);
    return count;
}

/**
 * @brief Returns 1 if @p pattern matches the ENTIRE @p string, 0 if it does
 *        not, and -1 on compile / NULL-arg / OOM error.
 *
 * This is the `std::regex_match` / Python `re.fullmatch` counterpart to
 * @ref regex_test (which only asks whether the string *contains* a match):
 * the whole input — from the first byte to the last — must be consumed by
 * the pattern. It is the right primitive for input validation, e.g. "is
 * this string a well-formed IPv4 address / UUID / integer?".
 *
 * The pattern is anchored at both ends internally, so you do NOT write the
 * `^`...`$` yourself (and any `^`/`$` you do add still works). Unlike a bare
 * `$`, the end anchor is absolute: a trailing newline does NOT count as a
 * full match. Capture groups inside @p pattern are not reported (the
 * function returns only a yes/no answer).
 *
 * @code
 * regex_full_match("\\d{4}-\\d{2}-\\d{2}", "2026-06-14", REGEX_DEFAULT); // 1
 * regex_full_match("\\d{4}-\\d{2}-\\d{2}", "x 2026-06-14", REGEX_DEFAULT); // 0
 * regex_full_match("[a-z]+", "abc\n", REGEX_DEFAULT);                      // 0
 * @endcode
 *
 * @return 1 on a whole-string match, 0 on no match, -1 on error.
 */
int regex_full_match(const char* pattern, const char* string, RegexFlags flags) {
    REGEX_LOG("[regex_full_match]: enter (pattern='%s', flags=0x%X)",
              pattern ? pattern : "(null)", (unsigned)flags);
    if (!pattern || !string) {
        REGEX_LOG("[regex_full_match]: NULL input -> -1");
        return -1;
    }

    /* Wrap the pattern so the engine must consume the entire string.
       PCRE  : \A(?:...)\z  — absolute start/end (immune to MULTILINE and to
               the trailing-newline that a bare '$' tolerates); the (?: )
               group is non-capturing so it never shifts the user's groups.
       POSIX : ^(...)$      — ERE has no \A/\z or (?: ); '^'/'$' bracket the
               whole pattern and, with REG_NEWLINE off, anchor the absolute
               string ends. */
#if CSTD_REGEX_USE_PCRE
    const char* pre = "\\A(?:";
    const char* suf = ")\\z";
#else
    const char* pre = "^(";
    const char* suf = ")$";
#endif
    size_t prelen = strlen(pre);
    size_t suflen = strlen(suf);
    size_t plen   = strlen(pattern);

    char* wrapped = (char*)malloc(prelen + plen + suflen + 1);
    if (!wrapped) {
        REGEX_LOG("[regex_full_match]: OOM building wrapped pattern -> -1");
        return -1;
    }
    memcpy(wrapped, pre, prelen);
    memcpy(wrapped + prelen, pattern, plen);
    memcpy(wrapped + prelen + plen, suf, suflen + 1);  /* copy the NUL too */

    Regex* r = regex_compile(wrapped, flags);
    free(wrapped);
    if (!r) {
        REGEX_LOG("[regex_full_match]: compile failed -> -1");
        return -1;
    }

    RegexMatch m;
    RegexResult rr = regex_search(r, string, &m);
    int out;
    if (rr == REGEX_SUCCESS) {
        /* Belt-and-suspenders: require the match to span the whole input.
           Anchoring already guarantees this on both backends, but the check
           also defends against any end-of-line leniency. */
        out = (m.start == string && m.length == strlen(string)) ? 1 : 0;
        regex_match_free(&m);
    }
    else if (rr == REGEX_NO_MATCH) {
        out = 0;
    }
    else {
        out = -1;
    }
    regex_deallocate(r);

    REGEX_LOG("[regex_full_match]: exit -> %d", out);
    return out;
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


static int regex_append_with_refs(char** buf, size_t* len, size_t* cap,
                                  const char* replacement, const RegexMatch* m);


/**
 * @brief Internal: shared engine for regex_replace, regex_replace_first and
 *        regex_replace_groups.
 *
 * @param max_replacements -1 means "replace all". Any other non-negative
 *                         value caps the number of substitutions.
 * @param expand_refs      Non-zero to expand $N backreferences in
 *                         @p replacement; zero to insert it literally.
 */
static char* regex_replace_impl(const char* pattern, const char* string, const char* replacement, RegexFlags flags, int max_replacements, int expand_refs) {
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

    size_t len           = strlen(string);
    size_t copied        = 0;   /* string[0..copied) is already in `out` */
    size_t pos           = 0;
    int options          = 0;
    size_t replacements  = 0;
    size_t replacement_n = strlen(replacement);
    RegexResult rr       = REGEX_NO_MATCH;

    RegexMatch m;
    while ((max_replacements < 0 || (int)replacements < max_replacements) &&
           regex_scan_next(r, string, len, &pos, &options, &m, &rr)) {
        /* Unmatched bytes up to the match start, then the replacement. */
        size_t ms = (size_t)(m.start - string);
        int ok = regex_buf_append(&out, &outl, &outc, string + copied, ms - copied);
        if (ok) {
            ok = expand_refs ? regex_append_with_refs(&out, &outl, &outc, replacement, &m)
                             : regex_buf_append(&out, &outl, &outc, replacement, replacement_n);
        }
        copied = (size_t)(m.end - string);
        regex_match_free(&m);
        if (!ok) {
            regex_deallocate(r);
            free(out);
            return NULL;
        }
        replacements++;
    }
    regex_deallocate(r);
    if (rr == REGEX_MATCH_ERROR && (max_replacements < 0 || (int)replacements < max_replacements)) {
        free(out);
        return NULL;
    }
    /* Append the rest of the string. */
    if (!regex_buf_append(&out, &outl, &outc, string + copied, len - copied)) {
        free(out);
        return NULL;
    }
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
    char* out = regex_replace_impl(pattern, string, replacement, flags, -1, 0);
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
    char* out = regex_replace_impl(pattern, string, replacement, flags, 1, 0);
    REGEX_LOG("[regex_replace_first]: exit -> %p", (void*)out);
    return out;
}


/**
 * @brief Internal: append @p replacement to the output buffer, expanding
 *        backreferences against the current match @p m.
 *
 * Substitution grammar:
 *   - `$0`        -> the whole match.
 *   - `$1`..`$N`  -> capture group N (the maximal run of digits is the group
 *                    number); a group that does not exist or did not
 *                    participate expands to the empty string.
 *   - `$$`        -> a literal `$`.
 *   - `$` + other -> a literal `$` followed by that character.
 *
 * Returns 1 on success, 0 on OOM.
 */
static int regex_append_with_refs(char** buf, size_t* len, size_t* cap,
                                  const char* replacement, const RegexMatch* m) {
    for (size_t i = 0; replacement[i]; ) {
        if (replacement[i] == '$') {
            char n = replacement[i + 1];      /* safe: at worst the NUL terminator */
            if (n == '$') {
                if (!regex_buf_append(buf, len, cap, "$", 1)) {
                    return 0;
                }
                i += 2;
                continue;
            }
            if (n >= '0' && n <= '9') {
                int    g = 0;
                size_t j = i + 1;
                while (replacement[j] >= '0' && replacement[j] <= '9') {
                    /* saturate: a long digit run used to overflow int (UB);
                       INT_MAX names no group, so it expands to nothing */
                    int d = replacement[j] - '0';
                    g = (g > (INT_MAX - d) / 10) ? INT_MAX : g * 10 + d;
                    j++;
                }
                const char* gs = NULL;
                size_t      gl = 0;
                if (g == 0) {
                    gs = m->start;
                    gl = m->length;
                }
                else if (g >= 1 && g <= m->group_count &&
                         m->group_starts && m->group_lengths) {
                    gs = m->group_starts[g - 1];
                    gl = m->group_lengths[g - 1];
                }
                /* gl == 0 (missing / non-participating group) -> append nothing. */
                if (gl && !regex_buf_append(buf, len, cap, gs, gl)) {
                    return 0;
                }
                i = j;
                continue;
            }
            /* A '$' not followed by a digit or '$' is emitted literally. */
            if (!regex_buf_append(buf, len, cap, "$", 1)) {
                return 0;
            }
            i += 1;
            continue;
        }
        if (!regex_buf_append(buf, len, cap, &replacement[i], 1)) {
            return 0;
        }
        i += 1;
    }
    return 1;
}


/**
 * @brief Like @ref regex_replace (replaces every match), but the replacement
 *        text may contain backreferences to captured groups.
 *
 * Whereas @ref regex_replace treats the replacement as literal text, this
 * function expands `$0`..`$N` to the corresponding capture group of each
 * match (`$0` = the whole match), `$$` to a literal `$`, and a lone `$`
 * (not followed by a digit or `$`) to a literal `$`. A referenced group
 * that does not exist or did not participate expands to nothing. This is
 * the standard way to *reformat* text — e.g. swapping fields or restyling
 * a date — without manually splicing strings.
 *
 * @code
 * // Swap "First Last" -> "Last, First"
 * char* a = regex_replace_groups("(\\w+)\\s+(\\w+)", "Ada Lovelace", "$2, $1", REGEX_DEFAULT);
 * // a -> "Lovelace, Ada"
 * free(a);
 *
 * // Reformat an ISO date to DD/MM/YYYY for every date in the string.
 * char* b = regex_replace_groups("(\\d{4})-(\\d{2})-(\\d{2})", "2026-06-14", "$3/$2/$1", REGEX_DEFAULT);
 * // b -> "14/06/2026"
 * free(b);
 * @endcode
 *
 * @return Newly-allocated result string (caller must `free`), or NULL on
 *         compile failure / OOM / NULL inputs.
 */
char* regex_replace_groups(const char* pattern, const char* string,
                           const char* replacement, RegexFlags flags) {
    REGEX_LOG("[regex_replace_groups]: enter (pattern='%s', flags=0x%X)",
              pattern ? pattern : "(null)", (unsigned)flags);
    char* out = regex_replace_impl(pattern, string, replacement, flags, -1, 1);
    REGEX_LOG("[regex_replace_groups]: exit -> %p", (void*)out);
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

    const char* cursor = string;   /* start of the current token */
    size_t len = strlen(string);
    size_t pos = 0;
    int options = 0;
    RegexResult rr = REGEX_NO_MATCH;
    RegexMatch m;
    while (regex_scan_next(r, string, len, &pos, &options, &m, &rr)) {
        /* Empty matches do not split (documented: split at non-empty matches). */
        if (m.length == 0) {
            regex_match_free(&m);
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
    if (rr == REGEX_MATCH_ERROR) {
        for (int i = 0; i < ntokens; i++) {
            free(tokens[i]);
        }
        free(tokens);
        regex_deallocate(r);
        REGEX_LOG("[regex_split]: matching failed -> NULL");
        return NULL;
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
