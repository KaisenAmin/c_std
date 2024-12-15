/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Xml
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "xml.h"
#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h> // memory mapping headers for Linux
#elif _WIN32
#include <windows.h> 
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

#ifndef EZXML_NOMMAP
#ifdef __linux__
#include <sys/mman.h>
#endif
#endif 

#ifdef _WIN32
#define fstat _fstat
#define open _open
#define close _close
#define read _read
#define PROT_READ 1
#define PROT_WRITE 2
#define MAP_PRIVATE 0x02
#define MAP_FAILED ((void*)-1)
#define MADV_SEQUENTIAL 0
#define MADV_NORMAL 0
#define _SC_PAGESIZE 1
#endif

#ifdef _WIN32
void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    (void)addr;   // Cast to void to suppress unused parameter warning
    (void)prot;
    (void)flags;
    (void)offset;

    HANDLE h = CreateFileMapping((HANDLE)_get_osfhandle(fd), NULL, PAGE_READWRITE, 0, 0, NULL);
    if (!h) {
        return MAP_FAILED;
    }
    return MapViewOfFile(h, FILE_MAP_READ, 0, 0, length);
}

int munmap(void* addr, size_t length) {
    (void)length;  // Cast to void to suppress unused parameter warning
    return UnmapViewOfFile(addr) ? 0 : -1;
}

long sysconf(int name) {
    if (name == _SC_PAGESIZE) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        return si.dwPageSize;
    }
    return -1;
}
#endif


#define EZXML_BUFSIZE 1024 // size of internal memory buffers
#define EZXML_NAMEM   (unsigned char)0x80
#define EZXML_TXTM    0x40 // txt is malloced
#define EZXML_DUP     0x20 // attribute name and value are strduped

#define EZXML_WS   "\t\r\n "  // whitespace
#define EZXML_ERRL 128        // maximum error string length


typedef struct ezxml_root *ezxml_root_t;
struct ezxml_root {       // additional data for the root tag
    struct ezxml xml;     // is a super-struct built on top of ezxml struct
    ezxml_t cur;          // current xml tree insertion point
    char *m;              // original xml string
    size_t len;           // length of allocated memory for mmap, -1 for malloc
    char *u;              // UTF-8 conversion of string if original was UTF-16
    char *s;              // start of work area
    char *e;              // end of work area
    char **ent;           // general entities (ampersand sequences)
    char ***attr;         // default attributes
    char ***pi;           // processing instructions
    short standalone;     // non-zero if <?xml standalone="yes"?>
    char err[EZXML_ERRL]; // error string
};

static ezxml_t ezxml_parse_str(char *s, size_t len);
static ezxml_t ezxml_parse_fd(int fd);
static ezxml_t ezxml_parse_file(const char *file);
static ezxml_t ezxml_parse_fp(FILE *fp);
static ezxml_t ezxml_child(ezxml_t xml, const char *name);

// returns the next tag of the same name in the same section and depth or NULL
// if not found
#define ezxml_next(xml) ((xml) ? xml->next : NULL)

// Returns the Nth tag with the same name in the same section at the same depth
// or NULL if not found. An index of 0 returns the tag given.
// static ezxml_t ezxml_idx(ezxml_t xml, int idx);

#define ezxml_name(xml) ((xml) ? xml->name : NULL)
#define ezxml_txt(xml) ((xml) ? (char *)xml->text : "")

// returns the value of the requested tag attribute, or NULL if not found
static const char *ezxml_attr(ezxml_t xml, const char *attr);
static char *ezxml_toxml(ezxml_t xml);
static const char **ezxml_pi(ezxml_t xml, const char *target);
static void ezxml_free(ezxml_t xml);
static const char *ezxml_error(ezxml_t xml);
static ezxml_t ezxml_new(const char *name);

// wrapper for ezxml_new() that strdup()s name
#define ezxml_new_d(name) ezxml_set_flag(ezxml_new(strdup(name)), EZXML_NAMEM)

// Adds a child tag. off is the offset of the child tag relative to the start
// of the parent tag's character content. Returns the child tag.
static ezxml_t ezxml_add_child(ezxml_t xml, const char *name, size_t off);

// wrapper for ezxml_add_child() that strdup()s name
#define ezxml_add_child_d(xml, name, off) \
    ezxml_set_flag(ezxml_add_child(xml, strdup(name), off), EZXML_NAMEM)

// sets the character content for the given tag and returns the tag
static ezxml_t ezxml_set_txt(ezxml_t xml, const char *txt);

// wrapper for ezxml_set_txt() that strdup()s txt
#define ezxml_set_txt_d(xml, txt) \
    ezxml_set_flag(ezxml_set_txt(xml, strdup(txt)), EZXML_TXTM)

// Sets the given tag attribute or adds a new attribute if not found. A value
// of NULL will remove the specified attribute. Returns the tag given.
static ezxml_t ezxml_set_attr(ezxml_t xml, const char *name, const char *value);

// Wrapper for ezxml_set_attr() that strdup()s name/value. Value cannot be NULL
#define ezxml_set_attr_d(xml, name, value) \
    ezxml_set_attr(ezxml_set_flag(xml, EZXML_DUP), strdup(name), strdup(value))

static ezxml_t ezxml_set_flag(ezxml_t xml, short flag);
static ezxml_t ezxml_cut(ezxml_t xml);
static ezxml_t ezxml_insert(ezxml_t xml, ezxml_t dest, size_t off);

#define ezxml_move(xml, dest, off) ezxml_insert(ezxml_cut(xml), dest, off)
#define ezxml_remove(xml) ezxml_free(ezxml_cut(xml))

char *EZXML_NIL[] = { NULL }; // empty, null terminated array of strings

// returns the first child tag with the given name or NULL if not found
static ezxml_t ezxml_child(ezxml_t xml, const char *name) {
    // printf("ezxml_child: Searching for child named '%s'\n", name);
    xml = (xml) ? xml->child : NULL;

    while (xml && strcmp(name, xml->name)) {
        // printf("ezxml_child: Checking sibling: %s\n", xml->name);
        xml = xml->sibling;
    }

    // printf("ezxml_child: Found node: %p, Name: %s\n", xml, xml ? xml->name : "(null)");
    return xml;
}

// returns the Nth tag with the same name in the same subsection or NULL if not
// static ezxml_t ezxml_idx(ezxml_t xml, int idx) {
//     for (; xml && idx; idx--) {
//         xml = xml->next;
//     }
//     return xml;
// }

// returns the value of the requested tag attribute or NULL if not found
static const char *ezxml_attr(ezxml_t xml, const char *attr)
{
    int i = 0, j = 1;
    ezxml_root_t root = (ezxml_root_t)xml;

    if (! xml || ! xml->attr) return NULL;
    while (xml->attr[i] && strcmp(attr, xml->attr[i])) i += 2;
    if (xml->attr[i]) return xml->attr[i + 1]; // found attribute

    while (root->xml.parent) root = (ezxml_root_t)root->xml.parent; // root tag
    for (i = 0; root->attr[i] && strcmp(xml->name, root->attr[i][0]); i++);
    if (! root->attr[i]) return NULL; // no matching default attributes
    while (root->attr[i][j] && strcmp(attr, root->attr[i][j])) j += 3;
    return (root->attr[i][j]) ? root->attr[i][j + 1] : NULL; // found default
}

// same as ezxml_get but takes an already initialized va_list
// static ezxml_t ezxml_vget(ezxml_t xml, va_list ap) {
//     if (!xml) {
//         fprintf(stderr, "xml is NULL!\n");
//         return NULL;
//     }

//     char *name = va_arg(ap, char *);
//     int idx = -1;

//     if (name && *name) {
//         idx = va_arg(ap, int);    
//         xml = ezxml_child(xml, name);
//     }

//     return (idx < 0) ? xml : ezxml_vget(ezxml_idx(xml, idx), ap);
// }

// returns a null terminated array of processing instructions for the given
static const char **ezxml_pi(ezxml_t xml, const char *target) {
    ezxml_root_t root = (ezxml_root_t)xml;
    int i = 0;

    if (! root) {
        return (const char **)EZXML_NIL;
    }

    while (root->xml.parent) {
        root = (ezxml_root_t)root->xml.parent; // root tag
    }
    while (root->pi[i] && strcmp(target, root->pi[i][0])) {
        i++; // find target
    }

    return (const char **)((root->pi[i]) ? root->pi[i] + 1 : EZXML_NIL);
}

// set an error string and return root
static ezxml_t ezxml_err(ezxml_root_t root, char *s, const char *err, ...) {
    va_list ap;
    int line = 1;
    char *t, fmt[EZXML_ERRL];
    
    for (t = root->s; t < s; t++) {
        if (*t == '\n') { 
            line++;
        }
    }
    snprintf(fmt, EZXML_ERRL, "[error near line %d]: %s", line, err);

    va_start(ap, err);
    vsnprintf(root->err, EZXML_ERRL, fmt, ap);
    va_end(ap);

    return &root->xml;
}

// Recursively decodes entity and character references and normalizes new lines
// ent is a null terminated array of alternating entity names and values. set t
// to '&' for general entity decoding, '%' for parameter entity decoding, 'c'
// for cdata sections, ' ' for attribute normalization, or '*' for non-cdata
// attribute normalization. Returns s, or if the decoded string is longer than
// s, returns a malloced string that must be freed.
static char *ezxml_decode(char *s, char **ent, char t)
{
    char *e, *r = s, *m = s;
    long b, c, d, l;

    for (; *s; s++) { // normalize line endings
        while (*s == '\r') {
            *(s++) = '\n';
            if (*s == '\n') memmove(s, (s + 1), strlen(s));
        }
    }
    
    for (s = r; ; ) {
        while (*s && *s != '&' && (*s != '%' || t != '%') && !isspace(*s)) s++;

        if (! *s) break;
        else if (t != 'c' && ! strncmp(s, "&#", 2)) { // character reference
            if (s[2] == 'x') c = strtol(s + 3, &e, 16); // base 16
            else c = strtol(s + 2, &e, 10); // base 10
            if (! c || *e != ';') { s++; continue; } // not a character ref

            if (c < 0x80) *(s++) = c; // US-ASCII subset
            else { // multi-byte UTF-8 sequence
                for (b = 0, d = c; d; d /= 2) b++; // number of bits in c
                b = (b - 2) / 5; // number of bytes in payload
                *(s++) = (0xFF << (7 - b)) | (c >> (6 * b)); // head
                while (b) *(s++) = 0x80 | ((c >> (6 * --b)) & 0x3F); // payload
            }

            memmove(s, strchr(s, ';') + 1, strlen(strchr(s, ';')));
        }
        else if ((*s == '&' && (t == '&' || t == ' ' || t == '*')) ||
                 (*s == '%' && t == '%')) { // entity reference
            for (b = 0; ent[b] && strncmp(s + 1, ent[b], strlen(ent[b]));
                 b += 2); // find entity in entity list

            if (ent[b++]) { // found a match
                if ((c = strlen(ent[b])) - 1 > (e = strchr(s, ';')) - s) {
                    l = (d = (s - r)) + c + strlen(e); // new length
                    r = (r == m) ? strcpy((char*)malloc(l), r) : (char*)realloc(r, l);
                    e = strchr((s = r + d), ';'); // fix up pointers
                }

                memmove(s + c, e + 1, strlen(e)); // shift rest of string
                strncpy(s, ent[b], c); // copy in replacement text
            }
            else s++; // not a known entity
        }
        else if ((t == ' ' || t == '*') && isspace(*s)) *(s++) = ' ';
        else s++; // no decoding needed
    }

    if (t == '*') { // normalize spaces for non-cdata attributes
        for (s = r; *s; s++) {
            if ((l = strspn(s, " "))) memmove(s, s + l, strlen(s + l) + 1);
            while (*s && *s != ' ') s++;
        }
        if (--s >= r && *s == ' ') *s = '\0'; // trim any trailing space
    }
    return r;
}

// called when parser finds start of new tag
static void ezxml_open_tag(ezxml_root_t root, char *name, char **attr) {
    ezxml_t xml = root->cur;
    
    if (xml->name) {
        xml = ezxml_add_child(xml, name, strlen(xml->txt));
    }
    else {
        xml->name = name; // first open tag
    }

    xml->attr = attr;
    root->cur = xml; // update tag insertion point
}

// called when parser finds character content between open and closing tag
static void ezxml_char_content(ezxml_root_t root, char *s, size_t len, char t)
{
    ezxml_t xml = root->cur;
    char *m = s;
    size_t l;

    if (! xml || ! xml->name || ! len) return; // sanity check

    s[len] = '\0'; // null terminate text (calling functions anticipate this)
    len = strlen(s = ezxml_decode(s, root->ent, t)) + 1;

    if (! *(xml->txt)) xml->txt = s; // initial character content
    else { // allocate our own memory and make a copy
        xml->txt = (xml->flags & EZXML_TXTM) // allocate some space
                   ? (char*)realloc(xml->txt, (l = strlen(xml->txt)) + len)
                   : strcpy((char*)malloc((l = strlen(xml->txt)) + len), xml->txt);
        strcpy(xml->txt + l, s); // add new char content
        if (s != m) free(s); // free s if it was malloced by ezxml_decode()
    }

    if (xml->txt != m) ezxml_set_flag(xml, EZXML_TXTM);
}

// called when parser finds closing tag
static ezxml_t ezxml_close_tag(ezxml_root_t root, char *name, char *s) {
    if (! root->cur || ! root->cur->name || strcmp(name, root->cur->name)) {
        return ezxml_err(root, s, "unexpected closing tag </%s>", name);
    }

    root->cur = root->cur->parent;
    return NULL;
}

// checks for circular entity references, returns non-zero if no circular
// references are found, zero otherwise
static int ezxml_ent_ok(char *name, char *s, char **ent) {
    int i;

    for (; ; s++) {
        while (*s && *s != '&') {
            s++; // find next entity reference
        }
        if (! *s) {
            return 1;
        }
        if (! strncmp(s + 1, name, strlen(name))) {
             return 0; // circular ref.
        }
        for (i = 0; ent[i] && strncmp(ent[i], s + 1, strlen(ent[i])); i += 2);
        if (ent[i] && ! ezxml_ent_ok(name, ent[i + 1], ent)) {
            return 0;
        }
    }
}

// called when the parser finds a processing instruction
static void ezxml_proc_inst(ezxml_root_t root, char *s, size_t len)
{
    int i = 0, j = 1;
    char *target = s;

    s[len] = '\0'; // null terminate instruction
    if (*(s += strcspn(s, EZXML_WS))) {
        *s = '\0'; // null terminate target
        s += strspn(s + 1, EZXML_WS) + 1; // skip whitespace after target
    }

    if (! strcmp(target, "xml")) { // <?xml ... ?>
        if ((s = strstr(s, "standalone")) && ! strncmp(s + strspn(s + 10,
            EZXML_WS "='\"") + 10, "yes", 3)) root->standalone = 1;
        return;
    }

    if (! root->pi[0]) *(root->pi = (char***)malloc(sizeof(char **))) = NULL; //first pi

    while (root->pi[i] && strcmp(target, root->pi[i][0])) i++; // find target
    if (! root->pi[i]) { // new target
        root->pi = (char***)realloc(root->pi, sizeof(char **) * (i + 2));
        root->pi[i] = (char**)malloc(sizeof(char *) * 3);
        root->pi[i][0] = target;
        root->pi[i][1] = (char *)(root->pi[i + 1] = NULL); // terminate pi list
        root->pi[i][2] = strdup(""); // empty document position list
    }

    while (root->pi[i][j]) j++; // find end of instruction list for this target
    root->pi[i] = (char**)realloc(root->pi[i], sizeof(char *) * (j + 3));
    root->pi[i][j + 2] = (char*)realloc(root->pi[i][j + 1], j + 1);
    strcpy(root->pi[i][j + 2] + j - 1, (root->xml.name) ? ">" : "<");
    root->pi[i][j + 1] = NULL; // null terminate pi list for this target
    root->pi[i][j] = s; // set instruction
}

// called when the parser finds an internal doctype subset
static short ezxml_internal_dtd(ezxml_root_t root, char *s, size_t len)
{
    char q, *c, *t, *n = NULL, *v, **ent, **pe;
    int i, j;
    
    pe = (char**)memcpy((char**)malloc(sizeof(EZXML_NIL)), EZXML_NIL, sizeof(EZXML_NIL));

    for (s[len] = '\0'; s; ) {
        while (*s && *s != '<' && *s != '%') s++; // find next declaration

        if (! *s) break;
        else if (! strncmp(s, "<!ENTITY", 8)) { // parse entity definitions
            c = s += strspn(s + 8, EZXML_WS) + 8; // skip white space separator
            n = s + strspn(s, EZXML_WS "%"); // find name
            *(s = n + strcspn(n, EZXML_WS)) = ';'; // append ; to name

            v = s + strspn(s + 1, EZXML_WS) + 1; // find value
            if ((q = *(v++)) != '"' && q != '\'') { // skip externals
                s = strchr(s, '>');
                continue;
            }

            for (i = 0, ent = (*c == '%') ? pe : root->ent; ent[i]; i++);
            ent = (char**)realloc(ent, (i + 3) * sizeof(char *)); // space for next ent
            if (*c == '%') pe = ent;
            else root->ent = ent;

            *(++s) = '\0'; // null terminate name
            if ((s = strchr(v, q))) *(s++) = '\0'; // null terminate value
            ent[i + 1] = ezxml_decode(v, pe, '%'); // set value
            ent[i + 2] = NULL; // null terminate entity list
            if (! ezxml_ent_ok(n, ent[i + 1], ent)) { // circular reference
                if (ent[i + 1] != v) free(ent[i + 1]);
                ezxml_err(root, v, "circular entity declaration &%s", n);
                break;
            }
            else ent[i] = n; // set entity name
        }
        else if (! strncmp(s, "<!ATTLIST", 9)) { // parse default attributes
            t = s + strspn(s + 9, EZXML_WS) + 9; // skip whitespace separator
            if (! *t) { ezxml_err(root, t, "unclosed <!ATTLIST"); break; }
            if (*(s = t + strcspn(t, EZXML_WS ">")) == '>') continue;
            else *s = '\0'; // null terminate tag name
            for (i = 0; root->attr[i] && strcmp(n, root->attr[i][0]); i++);

            s++;
            n = s + strspn(s, EZXML_WS);
            while (*n && *n != '>') {
                if (*(s = n + strcspn(n, EZXML_WS))) *s = '\0'; // attr name
                else { ezxml_err(root, t, "malformed <!ATTLIST"); break; }

                s += strspn(s + 1, EZXML_WS) + 1; // find next token
                c = (strncmp(s, "CDATA", 5)) ? (char*)"*" : (char*)" "; // is it cdata?
                if (! strncmp(s, "NOTATION", 8))
                    s += strspn(s + 8, EZXML_WS) + 8;
                s = (*s == '(') ? strchr(s, ')') : s + strcspn(s, EZXML_WS);
                if (! s) { ezxml_err(root, t, "malformed <!ATTLIST"); break; }

                s += strspn(s, EZXML_WS ")"); // skip white space separator
                if (! strncmp(s, "#FIXED", 6))
                    s += strspn(s + 6, EZXML_WS) + 6;
                if (*s == '#') { // no default value
                    s += strcspn(s, EZXML_WS ">") - 1;
                    if (*c == ' ') continue; // cdata is default, nothing to do
                    v = NULL;
                }
                else if ((*s == '"' || *s == '\'')  &&  // default value
                         (s = strchr(v = s + 1, *s))) *s = '\0';
                else { ezxml_err(root, t, "malformed <!ATTLIST"); break; }

                if (! root->attr[i]) { // new tag name
                    root->attr = (! i) ? (char***)malloc(2 * sizeof(char **))
                                       : (char***)realloc(root->attr,
                                                 (i + 2) * sizeof(char **));
                    root->attr[i] = (char**)malloc(2 * sizeof(char *));
                    root->attr[i][0] = t; // set tag name
                    root->attr[i][1] = (char *)(root->attr[i + 1] = NULL);
                }

                for (j = 1; root->attr[i][j]; j += 3); // find end of list
                root->attr[i] = (char**)realloc(root->attr[i],
                                        (j + 4) * sizeof(char *));

                root->attr[i][j + 3] = NULL; // null terminate list
                root->attr[i][j + 2] = c; // is it cdata?
                root->attr[i][j + 1] = (v) ? ezxml_decode(v, root->ent, *c)
                                           : NULL;
                root->attr[i][j] = n; // attribute name 
            }
        }
        else if (! strncmp(s, "<!--", 4)) s = strstr(s + 4, "-->"); // comments
        else if (! strncmp(s, "<?", 2)) { // processing instructions
            if ((s = strstr(c = s + 2, "?>")))
                ezxml_proc_inst(root, c, s++ - c);
        }
        else if (*s == '<') s = strchr(s, '>'); // skip other declarations
        else if (*(s++) == '%' && ! root->standalone) break;
    }

    free(pe);
    return ! *root->err;
}

// Converts a UTF-16 string to UTF-8. Returns a new string that must be freed
// or NULL if no conversion was needed.
static char *ezxml_str2utf8(char **s, size_t *len)
{
    char *u;
    size_t l = 0, sl, max = *len;
    long c, d;
    int b, be = (**s == '\xFE') ? 1 : (**s == '\xFF') ? 0 : -1;

    if (be == -1) return NULL; // not UTF-16

    u = (char*)malloc(max);
    for (sl = 2; sl < *len - 1; sl += 2) {
        c = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF)  //UTF-16BE
                 : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF); //UTF-16LE
        if (c >= 0xD800 && c <= 0xDFFF && (sl += 2) < *len - 1) { // high-half
            d = (be) ? (((*s)[sl] & 0xFF) << 8) | ((*s)[sl + 1] & 0xFF)
                     : (((*s)[sl + 1] & 0xFF) << 8) | ((*s)[sl] & 0xFF);
            c = (((c & 0x3FF) << 10) | (d & 0x3FF)) + 0x10000;
        }

        while (l + 6 > max) u = (char*)realloc(u, max += EZXML_BUFSIZE);
        if (c < 0x80) u[l++] = c; // US-ASCII subset
        else { // multi-byte UTF-8 sequence
            for (b = 0, d = c; d; d /= 2) b++; // bits in c
            b = (b - 2) / 5; // bytes in payload
            u[l++] = (0xFF << (7 - b)) | (c >> (6 * b)); // head
            while (b) u[l++] = 0x80 | ((c >> (6 * --b)) & 0x3F); // payload
        }
    }
    return *s = (char*)realloc(u, *len = l);
}

// frees a tag attribute list
static void ezxml_free_attr(char **attr) {
    int i = 0;
    char *m;
    
    if (! attr || attr == EZXML_NIL) {
        return; // nothing to free
    }
    while (attr[i]) {
        i += 2; // find end of attribute list
    }

    m = attr[i + 1]; // list of which names and values are malloced
    for (i = 0; m[i]; i++) {
        if (m[i] & EZXML_NAMEM) {
            free(attr[i * 2]);
        }
        if (m[i] & EZXML_TXTM) { 
            free(attr[(i * 2) + 1]);
        }
    }
    
    free(m);
    free(attr);
}

// parse the given xml string and return an ezxml structure
static ezxml_t ezxml_parse_str(char *s, size_t len)
{
    ezxml_root_t root = (ezxml_root_t)ezxml_new(NULL);
    char q, e, *d, **attr, **a = NULL; // initialize a to avoid compile warning
    int l, i, j;

    root->m = s;
    if (! len) return ezxml_err(root, NULL, "root tag missing");
    root->u = ezxml_str2utf8(&s, &len); // convert utf-16 to utf-8
    root->e = (root->s = s) + len; // record start and end of work area
    
    e = s[len - 1]; // save end char
    s[len - 1] = '\0'; // turn end char into null terminator

    while (*s && *s != '<') s++; // find first tag
    if (! *s) return ezxml_err(root, s, "root tag missing");

    for (; ; ) {
        attr = (char **)EZXML_NIL;
        d = ++s;
        
        if (isalpha(*s) || *s == '_' || *s == ':' || *s < '\0') { // new tag
            if (! root->cur)
                return ezxml_err(root, d, "markup outside of root element");

            s += strcspn(s, EZXML_WS "/>");
            while (isspace(*s)) *(s++) = '\0'; // null terminate tag name
  
            if (*s && *s != '/' && *s != '>') // find tag in default attr list
                for (i = 0; (a = root->attr[i]) && strcmp(a[0], d); i++);

            for (l = 0; *s && *s != '/' && *s != '>'; l += 2) { // new attrib
                attr = (l) ? (char**)realloc(attr, (l + 4) * sizeof(char *))
                           : (char**)malloc(4 * sizeof(char *)); // allocate space
                attr[l + 3] = (l) ? (char*)realloc(attr[l + 1], (l / 2) + 2)
                                  : (char*)malloc(2); // mem for list of maloced vals
                strcpy(attr[l + 3] + (l / 2), " "); // value is not malloced
                attr[l + 2] = NULL; // null terminate list
                attr[l + 1] = (char*)""; // temporary attribute value
                attr[l] = s; //set attribute name

                s += strcspn(s, EZXML_WS "=/>");
                if (*s == '=' || isspace(*s)) { 
                    *(s++) = '\0'; // null terminate tag attribute name
                    q = *(s += strspn(s, EZXML_WS "="));
                    if (q == '"' || q == '\'') { // attribute value
                        attr[l + 1] = ++s;
                        while (*s && *s != q) s++;
                        if (*s) *(s++) = '\0'; // null terminate attribute val
                        else {
                            ezxml_free_attr(attr);
                            return ezxml_err(root, d, "missing %c", q);
                        }

                        for (j = 1; a && a[j] && strcmp(a[j], attr[l]); j +=3);
                        attr[l + 1] = ezxml_decode(attr[l + 1], root->ent, (a
                                                   && a[j]) ? *a[j + 2] : ' ');
                        if (attr[l + 1] < d || attr[l + 1] > s)
                            attr[l + 3][l / 2] = EZXML_TXTM; // value malloced
                    }
                }
                while (isspace(*s)) s++;
            }

            if (*s == '/') { // self closing tag
                *(s++) = '\0';
                if ((*s && *s != '>') || (! *s && e != '>')) {
                    if (l) ezxml_free_attr(attr);
                    return ezxml_err(root, d, "missing >");
                }
                ezxml_open_tag(root, d, attr);
                ezxml_close_tag(root, d, s);
            }
            else if ((q = *s) == '>' || (! *s && e == '>')) { // open tag
                *s = '\0'; // temporarily null terminate tag name
                ezxml_open_tag(root, d, attr);
                *s = q;
            }
            else {
                if (l) ezxml_free_attr(attr);
                return ezxml_err(root, d, "missing >"); 
            }
        }
        else if (*s == '/') { // close tag
            s += strcspn(d = s + 1, EZXML_WS ">") + 1;
            if (! (q = *s) && e != '>') return ezxml_err(root, d, "missing >");
            *s = '\0'; // temporarily null terminate tag name
            if (ezxml_close_tag(root, d, s)) return &root->xml;
            if (isspace(*s = q)) s += strspn(s, EZXML_WS);
        }
        else if (! strncmp(s, "!--", 3)) { // xml comment
            if (! (s = strstr(s + 3, "--")) || (*(s += 2) != '>' && *s) ||
                (! *s && e != '>')) return ezxml_err(root, d, "unclosed <!--");
        }
        else if (! strncmp(s, "![CDATA[", 8)) { // cdata
            if ((s = strstr(s, "]]>")))
                ezxml_char_content(root, d + 8, (s += 2) - d - 10, 'c');
            else return ezxml_err(root, d, "unclosed <![CDATA[");
        }
        else if (! strncmp(s, "!DOCTYPE", 8)) { // dtd
            for (l = 0; *s && ((! l && *s != '>') || (l && (*s != ']' || 
                 *(s + strspn(s + 1, EZXML_WS) + 1) != '>')));
                 l = (*s == '[') ? 1 : l) s += strcspn(s + 1, "[]>") + 1;
            if (! *s && e != '>')
                return ezxml_err(root, d, "unclosed <!DOCTYPE");
            d = (l) ? strchr(d, '[') + 1 : d;
            if (l && ! ezxml_internal_dtd(root, d, s++ - d)) return &root->xml;
        }
        else if (*s == '?') { // <?...?> processing instructions
            do { s = strchr(s, '?'); } while (s && *(++s) && *s != '>');
            if (! s || (! *s && e != '>')) 
                return ezxml_err(root, d, "unclosed <?");
            else ezxml_proc_inst(root, d + 1, s - d - 2);
        }
        else return ezxml_err(root, d, "unexpected <");
        
        if (! s || ! *s) break;
        *s = '\0';
        d = ++s;
        if (*s && *s != '<') { // tag character content
            while (*s && *s != '<') s++;
            if (*s) {
                ezxml_char_content(root, d, s - d, '&');
                // printf("Parsed text content: %.*s\n", (int)(s - d), d);
            }
            else break;
        }
        else if (! *s) break;
    }

    if (! root->cur) return &root->xml;
    else if (! root->cur->name) return ezxml_err(root, d, "root tag missing");
    else return ezxml_err(root, d, "unclosed tag <%s>", root->cur->name);
}

// Wrapper for ezxml_parse_str() that accepts a file stream. Reads the entire
// stream into memory and then parses it. For xml files, use ezxml_parse_file()
// or ezxml_parse_fd()
static ezxml_t ezxml_parse_fp(FILE *fp)
{
    ezxml_root_t root;
    size_t l, len = 0;
    char *s;

    if (! (s = (char*)malloc(EZXML_BUFSIZE))) return NULL;
    do {
        len += (l = fread((s + len), 1, EZXML_BUFSIZE, fp));
        if (l == EZXML_BUFSIZE) s = (char*)realloc(s, len + EZXML_BUFSIZE);
    } while (s && l == EZXML_BUFSIZE);

    if (! s) return NULL;
    root = (ezxml_root_t)ezxml_parse_str(s, len);
    root->len = -1; // so we know to free s in ezxml_free()
    return &root->xml;
}

// A wrapper for ezxml_parse_str() that accepts a file descriptor. First
// attempts to mem map the file. Failing that, reads the file into memory.
// Returns NULL on failure.
static ezxml_t ezxml_parse_fd(int fd)
{
    ezxml_root_t root;
    #ifdef __linux__
    struct stat st;
    #else 
    struct _stat64i32 st;
    #endif 
    size_t l;
    void *m;

    if (fd < 0) return NULL;
    fstat(fd, &st);

#ifndef EZXML_NOMMAP
    l = (st.st_size + sysconf(_SC_PAGESIZE) - 1) & ~(sysconf(_SC_PAGESIZE) -1);
    if ((m = mmap(NULL, l, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) !=
        MAP_FAILED) {
        #ifdef __linux__
            madvise(m, l, MADV_SEQUENTIAL); // optimize for sequential access
        #endif

        root = (ezxml_root_t)ezxml_parse_str((char*)m, st.st_size);
        #ifdef __linux__
            madvise(m, root->len = l, MADV_NORMAL); // put it back to normal
        #endif
    }
    else { // mmap failed, read file into memory
#endif // EZXML_NOMMAP
        l = read(fd, (m = (char*)malloc(st.st_size)), st.st_size);
        root = (ezxml_root_t)ezxml_parse_str((char*)m, l);
        root->len = -1; // so we know to free s in ezxml_free()
#ifndef EZXML_NOMMAP
    }
#endif // EZXML_NOMMAP
    return &root->xml;
}

// a wrapper for ezxml_parse_fd that accepts a file name
static ezxml_t ezxml_parse_file(const char *file)
{
    int fd = open(file, O_RDONLY, 0);
    ezxml_t xml = ezxml_parse_fd(fd);
    
    if (fd >= 0) close(fd);
    return xml;
}

// Encodes ampersand sequences appending the results to *dst, reallocating *dst
// if length excedes max. a is non-zero for attribute encoding. Returns *dst
static char *ezxml_ampencode(const char *s, size_t len, char **dst, size_t *dlen,
                      size_t *max, short a)
{
    const char *e;
    
    for (e = s + len; s != e; s++) {
        while (*dlen + 10 > *max) *dst = (char*)realloc(*dst, *max += EZXML_BUFSIZE);

        switch (*s) {
        case '\0': return *dst;
        case '&': *dlen += sprintf(*dst + *dlen, "&amp;"); break;
        case '<': *dlen += sprintf(*dst + *dlen, "&lt;"); break;
        case '>': *dlen += sprintf(*dst + *dlen, "&gt;"); break;
        case '"': *dlen += sprintf(*dst + *dlen, (a) ? "&quot;" : "\""); break;
        case '\n': *dlen += sprintf(*dst + *dlen, (a) ? "&#xA;" : "\n"); break;
        case '\t': *dlen += sprintf(*dst + *dlen, (a) ? "&#x9;" : "\t"); break;
        case '\r': *dlen += sprintf(*dst + *dlen, "&#xD;"); break;
        default: (*dst)[(*dlen)++] = *s;
        }
    }
    return *dst;
}

// Recursively converts each tag to xml appending it to *s. Reallocates *s if
// its length excedes max. start is the location of the previous tag in the
// parent tag's character content. Returns *s.
static char *ezxml_toxml_r(ezxml_t xml, char **s, size_t *len, size_t *max,
                    size_t start, char ***attr)
{
    int i, j;
    char *txt = (xml->parent) ? xml->parent->txt : (char*)"";
    size_t off = 0;

    // parent character content up to this tag
    *s = ezxml_ampencode(txt + start, xml->off - start, s, len, max, 0);

    while (*len + strlen(xml->name) + 4 > *max) // reallocate s
        *s = (char*)realloc(*s, *max += EZXML_BUFSIZE);

    *len += sprintf(*s + *len, "<%s", xml->name); // open tag
    for (i = 0; xml->attr[i]; i += 2) { // tag attributes
        if (ezxml_attr(xml, xml->attr[i]) != xml->attr[i + 1]) continue;
        while (*len + strlen(xml->attr[i]) + 7 > *max) // reallocate s
            *s = (char*)realloc(*s, *max += EZXML_BUFSIZE);

        *len += sprintf(*s + *len, " %s=\"", xml->attr[i]);
        ezxml_ampencode(xml->attr[i + 1], -1, s, len, max, 1);
        *len += sprintf(*s + *len, "\"");
    }

    for (i = 0; attr[i] && strcmp(attr[i][0], xml->name); i++);
    for (j = 1; attr[i] && attr[i][j]; j += 3) { // default attributes
        if (! attr[i][j + 1] || ezxml_attr(xml, attr[i][j]) != attr[i][j + 1])
            continue; // skip duplicates and non-values
        while (*len + strlen(attr[i][j]) + 7 > *max) // reallocate s
            *s = (char*)realloc(*s, *max += EZXML_BUFSIZE);

        *len += sprintf(*s + *len, " %s=\"", attr[i][j]);
        ezxml_ampencode(attr[i][j + 1], -1, s, len, max, 1);
        *len += sprintf(*s + *len, "\"");
    }
    *len += sprintf(*s + *len, ">");

    *s = (xml->child) ? ezxml_toxml_r(xml->child, s, len, max, 0, attr) //child
                      : ezxml_ampencode(xml->txt, -1, s, len, max, 0);  //data
    
    while (*len + strlen(xml->name) + 4 > *max) // reallocate s
        *s = (char*)realloc(*s, *max += EZXML_BUFSIZE);

    *len += sprintf(*s + *len, "</%s>", xml->name); // close tag

    while (txt[off] && off < xml->off) off++; // make sure off is within bounds
    return (xml->ordered) ? ezxml_toxml_r(xml->ordered, s, len, max, off, attr)
                          : ezxml_ampencode(txt + off, -1, s, len, max, 0);
}

// Converts an ezxml structure back to xml. Returns a string of xml data that
// must be freed.
static char *ezxml_toxml(ezxml_t xml)
{
    ezxml_t p = (xml) ? xml->parent : NULL, o = (xml) ? xml->ordered : NULL;
    ezxml_root_t root = (ezxml_root_t)xml;
    size_t len = 0, max = EZXML_BUFSIZE;
    char *s = strcpy((char*)malloc(max), ""), *t, *n;
    int i, j, k;

    if (! xml || ! xml->name) return (char*)realloc(s, len + 1);
    while (root->xml.parent) root = (ezxml_root_t)root->xml.parent; // root tag

    for (i = 0; ! p && root->pi[i]; i++) { // pre-root processing instructions
        for (k = 2; root->pi[i][k - 1]; k++);
        for (j = 1; (n = root->pi[i][j]); j++) {
            if (root->pi[i][k][j - 1] == '>') continue; // not pre-root
            while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max)
                s = (char*)realloc(s, max += EZXML_BUFSIZE);
            len += sprintf(s + len, "<?%s%s%s?>\n", t, *n ? " " : "", n);
        }
    }

    xml->parent = xml->ordered = NULL;
    s = ezxml_toxml_r(xml, &s, &len, &max, 0, root->attr);
    xml->parent = p;
    xml->ordered = o;

    for (i = 0; ! p && root->pi[i]; i++) { // post-root processing instructions
        for (k = 2; root->pi[i][k - 1]; k++);
        for (j = 1; (n = root->pi[i][j]); j++) {
            if (root->pi[i][k][j - 1] == '<') continue; // not post-root
            while (len + strlen(t = root->pi[i][0]) + strlen(n) + 7 > max)
                s = (char*)realloc(s, max += EZXML_BUFSIZE);
            len += sprintf(s + len, "\n<?%s%s%s?>", t, *n ? " " : "", n);
        }
    }
    return (char*)realloc(s, len + 1);
}

// free the memory allocated for the ezxml structure
static void ezxml_free(ezxml_t xml)
{
    ezxml_root_t root = (ezxml_root_t)xml;
    int i, j;
    char **a, *s;

    if (!xml) return;
    ezxml_free(xml->child);
    ezxml_free(xml->ordered);

    if (! xml->parent) { // free root tag allocations
        for (i = 10; root->ent[i]; i += 2) // 0 - 9 are default entites (<>&"')
            if ((s = root->ent[i + 1]) < root->s || s > root->e) free(s);
        free(root->ent); // free list of general entities

        for (i = 0; (a = root->attr[i]); i++) {
            for (j = 1; a[j++]; j += 2) // free malloced attribute values
                if (a[j] && (a[j] < root->s || a[j] > root->e)) free(a[j]);
            free(a);
        }
        if (root->attr[0]) free(root->attr); // free default attribute list

        for (i = 0; root->pi[i]; i++) {
            for (j = 1; root->pi[i][j]; j++);
            free(root->pi[i][j + 1]);
            free(root->pi[i]);
        }            
        if (root->pi[0]) free(root->pi); // free processing instructions

        if ((int)root->len == -1) free(root->m); // malloced xml data
#ifndef EZXML_NOMMAP
        else if (root->len) munmap(root->m, root->len); // mem mapped xml data
#endif // EZXML_NOMMAP
        if (root->u) free(root->u); // utf8 conversion
    }

    ezxml_free_attr(xml->attr); // tag attributes
    if ((xml->flags & EZXML_TXTM)) free(xml->txt); // character content
    if ((xml->flags & EZXML_NAMEM)) free(xml->name); // tag name
    free(xml);
}

// return parser error message or empty string if none
static const char *ezxml_error(ezxml_t xml) {
    while (xml && xml->parent) {
         xml = xml->parent; // find root tag
    }

    return (xml) ? ((ezxml_root_t)xml)->err : "";
}

// returns a new empty ezxml structure with the given root tag name
static ezxml_t ezxml_new(const char *name)
{
    static const char *ent[] = { "lt;", "&#60;", "gt;", "&#62;", "quot;", "&#34;",
                           "apos;", "&#39;", "amp;", "&#38;", NULL };
    ezxml_root_t root = (ezxml_root_t)memset(malloc(sizeof(struct ezxml_root)), 
                                             '\0', sizeof(struct ezxml_root));
    root->xml.name = (char *)name;
    root->cur = &root->xml;
    strcpy(root->err, root->xml.txt = (char*)"");
    root->ent = (char**)memcpy((char**)malloc(sizeof(ent)), ent, sizeof(ent));
    root->attr = root->pi = (char ***)(root->xml.attr = EZXML_NIL);
    return &root->xml;
}

// inserts an existing tag into an ezxml structure
static ezxml_t ezxml_insert(ezxml_t xml, ezxml_t dest, size_t off)
{
    ezxml_t cur, prev, head;

    xml->next = xml->sibling = xml->ordered = NULL;
    xml->off = off;
    xml->parent = dest;

    if ((head = dest->child)) { // already have sub tags
        if (head->off <= off) { // not first subtag
            for (cur = head; cur->ordered && cur->ordered->off <= off;
                 cur = cur->ordered);
            xml->ordered = cur->ordered;
            cur->ordered = xml;
        }
        else { // first subtag
            xml->ordered = head;
            dest->child = xml;
        }

        for (cur = head, prev = NULL; cur && strcmp(cur->name, xml->name);
             prev = cur, cur = cur->sibling); // find tag type
        if (cur && cur->off <= off) { // not first of type
            while (cur->next && cur->next->off <= off) cur = cur->next;
            xml->next = cur->next;
            cur->next = xml;
        }
        else { // first tag of this type
            if (prev && cur) prev->sibling = cur->sibling; // remove old first
            xml->next = cur; // old first tag is now next
            for (cur = head, prev = NULL; cur && cur->off <= off;
                 prev = cur, cur = cur->sibling); // new sibling insert point
            xml->sibling = cur;
            if (prev) prev->sibling = xml;
        }
    }
    else dest->child = xml; // only sub tag

    return xml;
}

// Adds a child tag. off is the offset of the child tag relative to the start
// of the parent tag's character content. Returns the child tag.
static ezxml_t ezxml_add_child(ezxml_t xml, const char *name, size_t off) {
    ezxml_t child;
    if (!xml) { 
        return NULL;
    }

    child = (ezxml_t)memset(malloc(sizeof(struct ezxml)), '\0', sizeof(struct ezxml));
    child->name = (char *)name;
    child->attr = EZXML_NIL;
    child->txt = (char*)"";

    return ezxml_insert(child, xml, off);
}

// sets the character content for the given tag and returns the tag
static ezxml_t ezxml_set_txt(ezxml_t xml, const char *txt) {
    if (! xml) {
        return NULL;
    }
    if (xml->flags & EZXML_TXTM) {
        free(xml->txt); // existing txt was malloced
    }

    xml->flags &= ~EZXML_TXTM;
    xml->txt = (char *)txt;
    
    return xml;
}

// Sets the given tag attribute or adds a new attribute if not found. A value
// of NULL will remove the specified attribute. Returns the tag given.
static ezxml_t ezxml_set_attr(ezxml_t xml, const char *name, const char *value) {
    int l = 0, c;

    if (!xml) { 
        return NULL;
    }
    while (xml->attr[l] && strcmp(xml->attr[l], name)) {
        l += 2;
    }

    if (! xml->attr[l]) { // not found, add as new attribute
        if (! value) {
            return xml; // nothing to do
        }
        if (xml->attr == EZXML_NIL) { // first attribute
            xml->attr = (char**)malloc(4 * sizeof(char *));
            xml->attr[1] = strdup(""); // empty list of malloced names/vals
        }
        else {
            xml->attr = (char**)realloc(xml->attr, (l + 4) * sizeof(char *));
        }

        xml->attr[l] = (char *)name; // set attribute name
        xml->attr[l + 2] = NULL; // null terminate attribute list
        xml->attr[l + 3] = (char*)realloc(xml->attr[l + 1], (c = strlen(xml->attr[l + 1])) + 2);
        strcpy(xml->attr[l + 3] + c, " "); // set name/value as not malloced

        if (xml->flags & EZXML_DUP) { 
            xml->attr[l + 3][c] = EZXML_NAMEM;
        }
    }
    else if (xml->flags & EZXML_DUP) {
        free((char *)name); // name was strduped
    }

    for (c = l; xml->attr[c]; c += 2); // find end of attribute list
     
    if (xml->attr[c + 1][l / 2] & EZXML_TXTM) { 
        free(xml->attr[l + 1]); //old val
    }
    if (xml->flags & EZXML_DUP) {
         xml->attr[c + 1][l / 2] |= EZXML_TXTM;
    }
    else { 
        xml->attr[c + 1][l / 2] &= ~EZXML_TXTM;
    }

    if (value) {
        xml->attr[l + 1] = (char *)value; // set attribute value
    }
    else { // remove attribute
        if (xml->attr[c + 1][l / 2] & EZXML_NAMEM) {
            free(xml->attr[l]);
        }
        memmove(xml->attr + l, xml->attr + l + 2, (c - l + 2) * sizeof(char*));
        xml->attr = (char**)realloc(xml->attr, (c + 2) * sizeof(char *));
        memmove(xml->attr[c + 1] + (l / 2), xml->attr[c + 1] + (l / 2) + 1, (c / 2) - (l / 2)); // fix list of which name/vals are malloced
    }

    xml->flags &= ~EZXML_DUP; // clear strdup() flag
    return xml;
}

// sets a flag for the given tag and returns the tag
static ezxml_t ezxml_set_flag(ezxml_t xml, short flag) {
    if (xml) {
        xml->flags |= flag;
    }
    return xml;
}

// removes a tag along with its subtags without freeing its memory
static ezxml_t ezxml_cut(ezxml_t xml) {
    ezxml_t cur;

    if (! xml) {
        return NULL; // nothing to do
    }
    if (xml->next) {
        xml->next->sibling = xml->sibling; // patch sibling list
    }

    if (xml->parent) { // not root tag
        cur = xml->parent->child; // find head of subtag list
        if (cur == xml) {
            xml->parent->child = xml->ordered; // first subtag
        }
        else { // not first subtag
            while (cur->ordered != xml) {
                cur = cur->ordered;
            }
            cur->ordered = cur->ordered->ordered; // patch ordered list

            cur = xml->parent->child; // go back to head of subtag list
            if (strcmp(cur->name, xml->name)) { // not in first sibling list
                while (strcmp(cur->sibling->name, xml->name)) {
                    cur = cur->sibling;
                }
                if (cur->sibling == xml) { // first of a sibling list
                    cur->sibling = (xml->next) ? xml->next: cur->sibling->sibling;
                }
                else {
                    cur = cur->sibling; // not first of a sibling list
                }
            }

            while (cur->next && cur->next != xml) {
                cur = cur->next;
            }
            if (cur->next) {
                cur->next = cur->next->next; // patch next list
            }
        }        
    }
    xml->ordered = xml->sibling = xml->next = NULL;
    return xml;
}

static ezxml_t ezxml_document_root(ezxml_t doc) {
    while (doc && doc->parent) {
        doc = doc->parent;  // Traverse up to the root
    }
    return doc;
}

/**
 * @brief Parses an XML file and returns a corresponding XmlDocument structure.
 * 
 * This function reads the contents of the specified XML file and parses it into an XmlDocument structure.
 * If the file cannot be parsed or memory allocation fails, an error is logged, and NULL is returned.
 * 
 * @param filename The path to the XML file to be parsed.
 * @return A pointer to the XmlDocument structure representing the parsed XML file, or NULL if an error occurs.
 */
XmlDocument* xml_parse_file(const char* filename) {
    XML_LOG("[xml_parse_file] Parsing XML file: %s", filename);

    ezxml_t xml = ezxml_parse_file(filename);
    if (!xml) {
        XML_LOG("[xml_parse_file] Error: Failed to parse file: %s", filename);
        return NULL;
    }

    XmlDocument* doc = (XmlDocument*)malloc(sizeof(XmlDocument));
    if (!doc) {
        XML_LOG("[xml_parse_file] Error: Failed to allocate memory for XmlDocument.");
        return NULL;
    }

    doc->root = (XmlNode*)malloc(sizeof(XmlNode));
    if (!doc->root) {
        XML_LOG("[xml_parse_file] Error: Failed to allocate memory for root XmlNode.");
        free(doc);
        return NULL;
    }

    doc->root->internal_node = (void*)xml;
    XML_LOG("[xml_parse_file] Successfully parsed XML file: %s", filename);
    
    return doc;
}

/**
 * @brief Parses an XML string and returns a corresponding XmlDocument structure.
 * 
 * This function parses the provided XML string content into an XmlDocument structure.
 * If the string cannot be parsed or memory allocation fails, an error is logged, and NULL is returned.
 * 
 * @param xml_content The XML string content to be parsed.
 * @return A pointer to the XmlDocument structure representing the parsed XML string, or NULL if an error occurs.
 */
XmlDocument* xml_parse_string(const char* xml_content) {
    XML_LOG("[xml_parse_string] Parsing XML string content.");

    size_t len = strlen(xml_content);
    char* xml_copy = strdup(xml_content);
    if (!xml_copy) {
        XML_LOG("[xml_parse_string] Error: Failed to copy XML string content.");
        return NULL;
    }

    ezxml_t xml = ezxml_parse_str(xml_copy, len);
    if (!xml) {
        XML_LOG("[xml_parse_string] Error: Failed to parse XML string.");
        free(xml_copy);
        return NULL;
    }

    XmlDocument* doc = (XmlDocument*)malloc(sizeof(XmlDocument));
    if (!doc) {
        XML_LOG("[xml_parse_string] Error: Failed to allocate memory for XmlDocument.");
        ezxml_free(xml);  // Free the ezxml structure before returning
        free(xml_copy);
        return NULL;
    }

    doc->root = (XmlNode*)malloc(sizeof(XmlNode));
    if (!doc->root) {
        XML_LOG("[xml_parse_string] Error: Failed to allocate memory for root XmlNode.");
        ezxml_free(xml);
        free(xml_copy);
        free(doc);
        return NULL;
    }

    doc->root->internal_node = (void*)xml;
    XML_LOG("[xml_parse_string] Successfully parsed XML string content.");
    
    return doc;
}

/**
 * @brief Creates a new XML document with the specified root element name.
 * 
 * This function initializes a new XML document and sets the root element to the specified tag name. 
 * It allocates memory for both the XmlDocument and the root XmlNode. If the document or root node 
 * cannot be created, an error is logged and NULL is returned.
 * 
 * @param root_element_name The name of the root element to be created. 
 * @return A pointer to the newly created XmlDocument structure, or NULL if an error occurs.
 */
XmlDocument* xml_create_document(const char* root_element_name) {
    XML_LOG("[xml_create_document] Creating new XML document with root element: %s", root_element_name);

    ezxml_t xml = ezxml_new(root_element_name); // Use ezxml to create a new document
    if (!xml) {
        XML_LOG("[xml_create_document] Error: Failed to create root element: %s", root_element_name);
        return NULL;
    }

    XmlDocument* doc = (XmlDocument*)malloc(sizeof(XmlDocument));
    if (!doc) {
        XML_LOG("[xml_create_document] Error: Failed to allocate memory for XmlDocument.");
        ezxml_free(xml);
        return NULL;
    }

    doc->root = (XmlNode*)malloc(sizeof(XmlNode));
    if (!doc->root) {
        XML_LOG("[xml_create_document] Error: Failed to allocate memory for root XmlNode.");
        ezxml_free(xml);
        free(doc);
        return NULL;
    }

    doc->root->internal_node = (void*)xml; // Store the internal ezxml pointer
    XML_LOG("[xml_create_document] Successfully created XML document with root element: %s", root_element_name);

    return doc;
}

/**
 * @brief Creates a new XML element with the specified tag name.
 * 
 * This function creates a new XML element (node) with the given tag name. It allocates memory 
 * for the XmlNode structure and uses the ezxml library to create an isolated XML element.
 * If memory allocation or element creation fails, an error is logged, and NULL is returned.
 * 
 * @param doc Pointer to the XmlDocument structure (optional, can be NULL).
 * @param tag_name The tag name for the new XML element.
 * @return A pointer to the newly created XmlNode structure, or NULL if an error occurs.
 */
XmlNode* xml_create_element(XmlDocument* doc, const char* tag_name) {
    (void)doc;
    XML_LOG("[xml_create_element] Creating new XML element with tag name: %s", tag_name);

    ezxml_t new_elem = ezxml_new(tag_name);  // Create an isolated ezxml node
    if (!new_elem) {
        XML_LOG("[xml_create_element] Error: Failed to create element with tag name: %s", tag_name);
        return NULL;
    }

    XmlNode* node = (XmlNode*)malloc(sizeof(XmlNode));
    if (!node) {
        XML_LOG("[xml_create_element] Error: Failed to allocate memory for XmlNode.");
        ezxml_free(new_elem);
        return NULL;
    }

    node->internal_node = (void*)new_elem;   // Store the new element
    node->tag_name = new_elem->name;         // Set the tag name
    node->text = new_elem->txt;              // Set the text content (initially empty)

    XML_LOG("[xml_create_element] Successfully created XML element with tag name: %s", tag_name);

    return node;
}

/**
 * @brief Retrieves the root element of the XML document.
 * 
 * This function fetches the root element of the given XML document. It allocates memory for a new XmlNode structure representing the root node. 
 * If the document is invalid or the root element cannot be found, the function logs an error and returns NULL.
 * 
 * @param doc Pointer to the XmlDocument structure representing the XML document.
 * @return Pointer to an XmlNode structure representing the root element, or NULL if an error occurs.
 */
XmlNode* xml_get_root(XmlDocument* doc) {
    XML_LOG("[xml_get_root] Retrieving root element from XML document.");
    
    if (!doc || !doc->root || !doc->root->internal_node) {
        XML_LOG("[xml_get_root] Error: Invalid document or root internal node.");
        return NULL;
    }

    ezxml_t root = ezxml_document_root((ezxml_t)doc->root->internal_node);
    if (!root) {
        XML_LOG("[xml_get_root] Error: No root element found in document.");
        return NULL;
    }

    XmlNode* node = (XmlNode*)malloc(sizeof(XmlNode));
    if (!node) {
        XML_LOG("[xml_get_root] Error: Failed to allocate memory for XmlNode.");
        return NULL;
    }

    node->internal_node = (void*)root;

    if (!root->name) {
        XML_LOG("[xml_get_root] Error: Root element has no tag name.");
        free(node);
        return NULL;
    }

    node->tag_name = strdup(root->name); 
    if (!node->tag_name) {
        XML_LOG("[xml_get_root] Error: Failed to allocate memory for tag name.");
        free(node);
        return NULL;
    }

    node->text = root->txt; 
    XML_LOG("[xml_get_root] Successfully retrieved root element: %s", node->tag_name);
    return node;
}

/**
 * @brief Finds an XML element by its tag name.
 * 
 * This function searches for the first child element with the specified tag name in the given root node.
 * If the root or tag name is invalid, or if no matching element is found, the function logs an error and returns NULL.
 * 
 * @param root Pointer to the XmlNode structure representing the root node.
 * @param tag_name The tag name of the element to search for.
 * @return Pointer to the found XmlNode structure, or NULL if not found or an error occurs.
 */
XmlNode* xml_find_element_by_tag(XmlNode* root, const char* tag_name) {
    XML_LOG("[xml_find_element_by_tag] Searching for element with tag name: %s", tag_name);
    
    if (!root || !tag_name) {
        XML_LOG("[xml_find_element_by_tag] Error: Invalid root node or tag name.");
        return NULL;
    }

    ezxml_t xml = (ezxml_t)root->internal_node;
    ezxml_t child = ezxml_child(xml, tag_name);

    if (!child) {
        XML_LOG("[xml_find_element_by_tag] Warning: Element with tag name '%s' not found.", tag_name);
        return NULL;
    }

    XmlNode* result = (XmlNode*)malloc(sizeof(XmlNode));
    if (!result) {
        XML_LOG("[xml_find_element_by_tag] Error: Failed to allocate memory for XmlNode.");
        return NULL;
    }

    result->internal_node = (void*)child;
    result->tag_name = child->name;
    result->text = child->txt;

    XML_LOG("[xml_find_element_by_tag] Successfully found element with tag name: %s", tag_name);
    return result;
}

/**
 * @brief Prints an XML node to the standard output.
 * 
 * This function converts the given XML node to a string representation and prints it to the standard output.
 * If the node or its conversion fails, the function logs an error.
 * 
 * @param node Pointer to the XmlNode structure to be printed.
 */
void xml_print(XmlNode* node) {
    XML_LOG("[xml_print] Printing XML node.");

    if (!node) {
        XML_LOG("[xml_print] Error: Node is NULL.");
        return;
    }

    char* xml_str = ezxml_toxml((ezxml_t)node->internal_node);

    if (xml_str) {
        printf("%s\n", xml_str);
        XML_LOG("[xml_print] Successfully printed XML node.");
        free(xml_str);
    } 
    else {
        XML_LOG("[xml_print] Error: Failed to convert node to XML string.");
    }
}

/**
 * @brief Appends a child node to a parent node.
 * 
 * This function appends the given child node to the specified parent node.
 * If either the parent or child is invalid, or if there is an attempt to append a node to itself or its parent, the function logs an error and returns.
 * 
 * @param parent Pointer to the XmlNode structure representing the parent node.
 * @param child Pointer to the XmlNode structure representing the child node.
 */
void xml_append_child(XmlNode* parent, XmlNode* child) {
    XML_LOG("[xml_append_child] Appending child node to parent.");

    if (!parent || !child) {
        XML_LOG("[xml_append_child] Error: Parent or child node is NULL.");
        return;
    }

    ezxml_t parent_node = (ezxml_t)parent->internal_node;
    ezxml_t child_node = (ezxml_t)child->internal_node;

    if (child_node == parent_node || child_node->parent == parent_node) {
        XML_LOG("[xml_append_child] Error: Attempting to append a node to itself or its parent.");
        return;
    }

    ezxml_insert(child_node, parent_node, 0);
    XML_LOG("[xml_append_child] Successfully appended child node.");
}

/**
 * @brief Sets the text content of an XML element.
 * 
 * This function sets the text content of the provided XML element. 
 * If the element or the text is NULL, the function logs an error and returns without performing any action.
 * 
 * @param element Pointer to the XmlNode structure representing the element.
 * @param text The text content to set for the element.
 */
void xml_set_element_text(XmlNode* element, const char* text) {
    XML_LOG("[xml_set_element_text] Setting text for XML element.");

    if (!element || !text) {
        XML_LOG("[xml_set_element_text] Error: Element or text is NULL.");
        return;
    }

    ezxml_set_txt((ezxml_t)element->internal_node, text);
    XML_LOG("[xml_set_element_text] Successfully set text for XML element.");
}

/**
 * @brief Sets an attribute for an XML element.
 * 
 * This function sets an attribute with a specified name and value for the provided XML element.
 * If the element, name, or value is NULL, the function logs an error and returns without performing any action.
 * 
 * @param element Pointer to the XmlNode structure representing the element.
 * @param name The name of the attribute to set.
 * @param value The value of the attribute to set.
 */
void xml_set_element_attribute(XmlNode* element, const char* name, const char* value) {
    XML_LOG("[xml_set_element_attribute] Setting attribute for XML element.");

    if (!element || !name || !value) {
        XML_LOG("[xml_set_element_attribute] Error: Element, name, or value is NULL.");
        return;
    }

    ezxml_set_attr((ezxml_t)element->internal_node, name, value);
    XML_LOG("[xml_set_element_attribute] Successfully set attribute '%s' with value '%s'.", name, value);
}

/**
 * @brief Deallocates an XML document and its contents.
 * 
 * This function deallocates the memory associated with an XML document, including the root node and the document structure itself.
 * If the document is NULL, the function logs an error and returns without performing any action.
 * 
 * @param doc Pointer to the XmlDocument structure to be deallocated.
 */
void xml_deallocate_document(XmlDocument* doc) {
    XML_LOG("[xml_deallocate_document] Deallocating XML document.");

    if (!doc) {
        XML_LOG("[xml_deallocate_document] Error: Document is NULL.");
        return;
    }

    ezxml_free((ezxml_t)doc->root->internal_node);  
    free(doc->root);  
    free(doc);        

    XML_LOG("[xml_deallocate_document] Successfully deallocated XML document.");
}

/**
 * @brief Deallocates an XML node and its contents.
 * 
 * This function deallocates the memory associated with an individual XML node, including the internal node.
 * If the node is NULL, the function logs an error and returns without performing any action.
 * 
 * @param node Pointer to the XmlNode structure to be deallocated.
 */
void xml_deallocate_node(XmlNode* node) {
    XML_LOG("[xml_deallocate_node] Deallocating XML node.");

    if (!node) {
        XML_LOG("[xml_deallocate_node] Error: Node is NULL.");
        return;
    }

    ezxml_free((ezxml_t)node->internal_node);  
    free(node);  
    XML_LOG("[xml_deallocate_node] Successfully deallocated XML node.");
}

/**
 * @brief Copies the provided text to a new dynamically allocated memory block.
 * 
 * This function duplicates a given text string by allocating new memory 
 * and copying the contents of the string into it. If the input text is NULL, 
 * or if memory allocation fails, the function returns NULL.
 * 
 * @param text The text string to be copied.
 * @return A pointer to the newly allocated copy of the text, or NULL if an error occurs.
 * @note The caller is responsible for freeing the memory allocated for the copied text.
 */
char* xml_copy_text(const char* text) {
    XML_LOG("[xml_copy_text] Copying text.");

    if (!text) {
        XML_LOG("[xml_copy_text] Error: Text is NULL.");
        return NULL;
    }

    size_t len = strlen(text) + 1;  
    char* copy = (char*)malloc(len);
    if (copy) {
        strncpy(copy, text, len);   
        XML_LOG("[xml_copy_text] Successfully copied text.");
    } 
    else {
        XML_LOG("[xml_copy_text] Error: Memory allocation failed.");
    }

    return copy;
}

/**
 * @brief Retrieves the text content of an XML element.
 * 
 * This function retrieves the text content from the provided XML element. 
 * If the element or its internal node is NULL, or if the element does not contain text, 
 * the function returns NULL. The returned text is duplicated in new memory.
 * 
 * @param element Pointer to the XmlNode structure representing the element.
 * @return A pointer to the duplicated text, or NULL if no text is found or an error occurs.
 * @note The caller is responsible for freeing the memory allocated for the duplicated text.
 */
const char* xml_get_element_text(XmlNode* element) {
    XML_LOG("[xml_get_element_text] Retrieving text from XML element.");

    if (!element || !element->internal_node) {
        XML_LOG("[xml_get_element_text] Error: Element or internal node is NULL.");
        return NULL;
    }

    ezxml_t result = (ezxml_t)element->internal_node;

    if (result && result->txt && strlen(result->txt) > 0) {
        char* duplicated_text = strdup(result->txt);
        if (duplicated_text) {
            XML_LOG("[xml_get_element_text] Successfully retrieved and duplicated text.");
            return duplicated_text;  
        } 
        else {
            XML_LOG("[xml_get_element_text] Error: Memory allocation for duplicated text failed.");
        }
    }

    XML_LOG("[xml_get_element_text] No text found for the element.");
    return NULL;  
}

/**
 * @brief Retrieves the tag name of an XML element.
 * 
 * This function retrieves the tag name of the provided XML node. If the node or its 
 * internal representation is NULL, the function returns NULL.
 * 
 * @param node Pointer to the XmlNode structure representing the element.
 * @return A pointer to the tag name of the element, or NULL if the node or its internal representation is NULL.
 */
const char* xml_get_tag_name(XmlNode* node) {
    XML_LOG("[xml_get_tag_name] Retrieving tag name.");

    if (node && node->internal_node) {
        XML_LOG("[xml_get_tag_name] Successfully retrieved tag name.");
        return ((ezxml_t)node->internal_node)->name;  
    }

    XML_LOG("[xml_get_tag_name] Error: Node or internal node is NULL.");
    return NULL;  
}

/**
 * @brief Retrieves the value of an attribute for a given XML element.
 * 
 * This function retrieves the value of the specified attribute from an XML element.
 * If the attribute exists, its value is returned. If the attribute is not found, 
 * or if the element or name is NULL, the function returns NULL.
 * 
 * @param element Pointer to the XmlNode structure representing the element.
 * @param name The name of the attribute to retrieve. 
 * @return The value of the attribute as a string, or NULL if the attribute is not found or if an error occurs.
 * @note The caller must ensure that the element and attribute name are not NULL before calling this function.
 */
const char* xml_get_element_attribute(XmlNode* element, const char* name) {
    XML_LOG("[xml_get_element_attribute] Retrieving attribute value for '%s'.", name);

    if (!element || !element->internal_node || !name) {
        XML_LOG("[xml_get_element_attribute] Error: Element, internal node, or attribute name is NULL.");
        return NULL;
    }

    const char* attr_value = ezxml_attr((ezxml_t)element->internal_node, name);
    if (attr_value) {
        XML_LOG("[xml_get_element_attribute] Successfully retrieved attribute value.");
    } 
    else {
        XML_LOG("[xml_get_element_attribute] Attribute '%s' not found.", name);
    }

    return attr_value;
}

/**
 * @brief Converts the XML document to a string representation.
 * 
 * This function converts the entire XML document to a string. The resulting string
 * contains the serialized XML, which can be printed or saved to a file. If the conversion
 * fails, the function returns NULL.
 * 
 * @param doc Pointer to the XmlDocument structure representing the XML document.
 * @return A string containing the serialized XML document, or NULL if an error occurs.
 * @note The caller must ensure that the document and its root node are not NULL before calling this function.
 */
char* xml_to_string(XmlDocument* doc) {
    XML_LOG("[xml_to_string] Converting XML document to string.");

    if (!doc || !doc->root || !doc->root->internal_node) {
        XML_LOG("[xml_to_string] Error: Document, root, or internal node is NULL.");
        return NULL;
    }

    char* xml_str = ezxml_toxml((ezxml_t)doc->root->internal_node);
    if (xml_str) {
        XML_LOG("[xml_to_string] Successfully converted XML document to string.");
    } 
    else {
        XML_LOG("[xml_to_string] Error: Failed to convert XML document to string.");
    }

    return xml_str;
}

/**
 * @brief Saves the XML document to a file.
 * 
 * This function saves the XML document to the specified file by first converting the document 
 * to a string and then writing the string to the file. If the operation is successful, the 
 * function returns 1. If the conversion or file writing fails, the function returns 0.
 * 
 * @param doc Pointer to the XmlDocument structure representing the XML document.
 * @param filename The name of the file to which the XML document will be saved.
 * @return 1 if the document is saved successfully, 0 otherwise.
 * @note The caller must ensure that the document and its root node are not NULL before calling this function.
 */
int xml_save_to_file(XmlDocument* doc, const char* filename) {
    XML_LOG("[xml_save_to_file] Saving XML document to file: %s", filename);

    if (!doc || !doc->root || !doc->root->internal_node) {
        XML_LOG("[xml_save_to_file] Error: Document or root is NULL.");
        return 0;
    }

    char* xml_str = ezxml_toxml((ezxml_t)doc->root->internal_node);  // Convert to string
    if (!xml_str) {
        XML_LOG("[xml_save_to_file] Error: Failed to convert XML document to string.");
        return 0;  // Return 0 if conversion fails
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        XML_LOG("[xml_save_to_file] Error: Failed to open file %s for writing.", filename);
        free(xml_str); 
        return 0;
    }

    fprintf(file, "%s", xml_str); 
    fclose(file);
    free(xml_str);  

    XML_LOG("[xml_save_to_file] Successfully saved XML document to file.");
    return 1; 
}

/**
 * @brief Removes an XML node and its subtags without freeing the memory.
 * 
 * This function removes the specified node from the XML document tree, but does not
 * free the memory associated with the node. The node is cut out of the tree but can 
 * still be accessed and manipulated separately.
 * 
 * @param node Pointer to the XmlNode structure representing the node to be removed.
 * @return void
 * @note The caller must ensure that the node and its internal node are not NULL before calling this function.
 */
void xml_cut(XmlNode* node) {
    XML_LOG("[xml_cut] Cutting XML node.");
    if (!node || !node->internal_node) {
        XML_LOG("[xml_cut] Error: Node or internal node is NULL.");
        return;
    }

    ezxml_cut((ezxml_t)node->internal_node);  // Use ezxml to remove the node
    XML_LOG("[xml_cut] Successfully removed the XML node.");
}

/**
 * @brief Retrieves the last XML parsing error, or an empty string if no errors occurred.
 * 
 * This function checks for any parsing errors that may have occurred during the last XML
 * operation. If there was an error, the error message is returned. If no error occurred,
 * an empty string is returned.
 * 
 * @param doc Pointer to the XmlDocument structure representing the XML document.
 * @return A string containing the last parsing error message, or an empty string if no error occurred.
 * @note The caller must ensure that the document, its root, and its internal node are not NULL before calling this function.
 */
const char* xml_get_error(XmlDocument* doc) {
    XML_LOG("[xml_get_error] Retrieving last XML parsing error.");
    if (!doc || !doc->root || !doc->root->internal_node) {
        XML_LOG("[xml_get_error] Error: Document or root is NULL.");
        return "";
    }

    const char* error = ezxml_error((ezxml_t)doc->root->internal_node); 
    if (error && strlen(error) > 0) {
        XML_LOG("[xml_get_error] Error found: %s", error);
    } 
    else {
        XML_LOG("[xml_get_error] No error found.");
    }

    return error;
}

/**
 * @brief Parses XML data from a file pointer (stream).
 * 
 * This function parses XML data from the given file stream and returns a pointer to the 
 * XmlDocument structure representing the parsed XML document. If the parsing fails, 
 * the function returns NULL.
 * 
 * @param fp File pointer to the input stream containing the XML data.
 * @return A pointer to the parsed XmlDocument structure, or NULL if an error occurs.
 * @note The caller must ensure that the file pointer is valid and not NULL before calling this function.
 */
XmlDocument* xml_parse_file_stream(FILE* fp) {
    XML_LOG("[xml_parse_file_stream] Parsing XML from file stream.");
    if (!fp) {
        XML_LOG("[xml_parse_file_stream] Error: File pointer is NULL.");
        return NULL;
    }

    ezxml_t xml = ezxml_parse_fp(fp); 
    if (!xml) {
        XML_LOG("[xml_parse_file_stream] Error: Failed to parse XML from file stream.");
        return NULL;  
    }

    XmlDocument* doc = (XmlDocument*)malloc(sizeof(XmlDocument));
    if (!doc) {
        XML_LOG("[xml_parse_file_stream] Error: Memory allocation failed for XmlDocument.");
        return NULL;
    }

    doc->root = (XmlNode*)malloc(sizeof(XmlNode));
    if (!doc->root) {
        XML_LOG("[xml_parse_file_stream] Error: Memory allocation failed for XmlNode.");
        free(doc);
        return NULL;
    }

    doc->root->internal_node = (void*)xml; 
    XML_LOG("[xml_parse_file_stream] Successfully parsed XML from file stream.");
    return doc;  
}

/**
 * @brief Retrieves processing instructions from the XML document based on a given target.
 * 
 * This function searches for and returns the processing instructions (if any) associated 
 * with the specified target in the XML document.
 * 
 * @param doc Pointer to the XmlDocument structure representing the XML document.
 * @param target The target of the processing instructions ("xml-stylesheet").
 * 
 * @return A pointer to an array of strings containing the processing instructions, or NULL 
 *         if no processing instructions are found or if an error occurs.
 * 
 * @note The caller must ensure that the document and target are not NULL.
 */
const char** xml_get_processing_instructions(XmlDocument* doc, const char* target) {
    XML_LOG("[xml_get_processing_instructions] Retrieving processing instructions for target: %s", target);

    if (!doc || !doc->root || !doc->root->internal_node) {
        XML_LOG("[xml_get_processing_instructions] Error: Document or root is NULL.");
        return NULL;
    }

    const char** instructions = ezxml_pi((ezxml_t)doc->root->internal_node, target); 
    if (instructions) {
        XML_LOG("[xml_get_processing_instructions] Successfully retrieved processing instructions.");
    } 
    else {
        XML_LOG("[xml_get_processing_instructions] No processing instructions found.");
    }

    return instructions;
}

/**
 * @brief Retrieves a nested XML element from the root node by tag names.
 * 
 * This function retrieves a nested XML element by navigating through the hierarchy
 * of the XML document using variable-length arguments that represent the tag names.
 * 
 * @param root Pointer to the XmlNode structure representing the root node.
 * @param ... Variable-length arguments representing the tag names in the hierarchy 
 *            ("root", "child", "subchild").
 * 
 * @return A pointer to the XmlNode representing the found element, or NULL if the element
 *         is not found or an error occurs.
 * @note The caller must ensure that the root and internal node are not NULL.
 */
XmlNode* xml_get_element(XmlNode* root, ...) {
    XML_LOG("[xml_get_element] Retrieving nested element from root node.");
    if (!root || !root->internal_node) {
        XML_LOG("[xml_get_element] Error: Null root or internal node.");
        return NULL;
    }

    va_list args;
    va_start(args, root);

    ezxml_t current_node = (ezxml_t)root->internal_node;
    const char* tag_name = va_arg(args, const char*);

    while (tag_name) {
        if (strlen(tag_name) == 0) {
            XML_LOG("[xml_get_element] Warning: Empty tag name encountered.");
            break;
        }

        XML_LOG("[xml_get_element] Looking for child tag: %s", tag_name);
        current_node = ezxml_child(current_node, tag_name);
        if (!current_node) {
            XML_LOG("[xml_get_element] Error: Element '%s' not found.", tag_name);
            va_end(args);
            return NULL;
        }

        tag_name = va_arg(args, const char*);
    }

    va_end(args);

    if (current_node) {
        XmlNode* node = (XmlNode*)malloc(sizeof(XmlNode));
        if (node) {
            node->internal_node = (void*)current_node;
            node->tag_name = strdup(current_node->name);
            node->text = current_node->txt;

            XML_LOG("[xml_get_element] Successfully retrieved element '%s'.", node->tag_name);
        } 
        else {
            XML_LOG("[xml_get_element] Error: Memory allocation failed for XmlNode.");
        }

        return node;
    }

    XML_LOG("[xml_get_element] Error: Could not retrieve the element.");
    return NULL;
}
