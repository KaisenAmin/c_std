/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Xml
 *
 * Declarations only. All Doxygen contracts for the functions below
 * live above their DEFINITIONS in xml.c (file-level convention).
 */

#ifndef XML_H_
#define XML_H_

#include <stddef.h>
#include <stdio.h>

#ifdef __linux__
#include <sys/stat.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* #define XML_LOGGING_ENABLE */

#ifdef XML_LOGGING_ENABLE
    #define XML_LOG(fmt, ...) \
        do { fprintf(stderr, "[XML LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define XML_LOG(...) do { } while (0)
#endif


typedef struct XmlDocument  XmlDocument;
typedef struct XmlNode      XmlNode;
typedef struct XmlAttribute XmlAttribute;

/* Internal ezxml backend handle; surfaced because the public
 * `XmlNode` carries it as `internal_node`. */
typedef struct ezxml* ezxml_t;


struct ezxml {
    char*    name;
    char**   attr;
    char*    txt;
    size_t   off;
    ezxml_t  next;
    ezxml_t  sibling;
    ezxml_t  ordered;
    ezxml_t  child;
    ezxml_t  parent;
    short    flags;
};


struct XmlNode {
    void*            internal_node;   /* pointer to the internal ezxml node */
    char*            tag_name;        /* cached tag name (borrowed from ezxml node) */
    char*            text;            /* cached text content (borrowed from ezxml node) */
    struct XmlNode*  parent;
    struct XmlNode*  children;
    struct XmlNode*  next_sibling;
    struct XmlNode*  attributes;
    struct XmlDocument* owner;        /* doc that owns/registers this wrapper, or NULL */
};

struct XmlDocument {
    XmlNode* root;
    /* Registry of every wrapper this document handed out via getters
     * (xml_get_root, xml_find_*, xml_get_element, xml_get_children) and via
     * xml_create_element. xml_deallocate_document frees each of these wrapper
     * structs exactly once. The ezxml tree they point into is owned by the
     * document and freed separately (once) — wrappers never free internal_node
     * or tag_name (both are borrowed from the ezxml tree). */
    XmlNode** reg_nodes;
    size_t    reg_count;
    size_t    reg_cap;
};

struct XmlAttribute {
    char*                 name;
    char*                 value;
    struct XmlAttribute*  next;
};


/* ------------------------------------------------------------------ */
/* Document construction / parsing                                    */
/* ------------------------------------------------------------------ */

XmlDocument* xml_parse_file                   (const char* filename);
XmlDocument* xml_parse_string                 (const char* xml_content);
XmlDocument* xml_parse_file_stream            (FILE* fp);
XmlDocument* xml_create_document              (const char* root_element_name);


/* ------------------------------------------------------------------ */
/* Document I/O and destruction                                       */
/* ------------------------------------------------------------------ */

int          xml_save_to_file                 (XmlDocument* doc, const char* filename);
char*        xml_to_string                    (XmlDocument* doc);
void         xml_deallocate_document          (XmlDocument* doc);


/* ------------------------------------------------------------------ */
/* Element construction / tree mutation                               */
/* ------------------------------------------------------------------ */

XmlNode*     xml_create_element               (XmlDocument* doc, const char* tag_name);
void         xml_append_child                 (XmlNode* parent, XmlNode* child);
void         xml_cut                          (XmlNode* node);
void         xml_deallocate_node              (XmlNode* node);


/* ------------------------------------------------------------------ */
/* Element accessors / queries                                        */
/* ------------------------------------------------------------------ */

XmlNode*     xml_get_root                     (XmlDocument* doc);
XmlNode*     xml_get_element                  (XmlNode* root, ...);
XmlNode*     xml_find_element_by_tag          (XmlNode* root, const char* tag_name);
XmlNode*     xml_find_element_by_attribute    (XmlNode* root, const char* attr_name, const char* attr_value);
XmlNode**    xml_get_children                 (XmlNode* node, size_t* out_count);
const char*  xml_get_tag_name                 (XmlNode* node);
const char*  xml_get_element_text             (XmlNode* element);
const char*  xml_get_element_attribute        (XmlNode* element, const char* name);
size_t       xml_count_children               (const XmlNode* node);
size_t       xml_count_elements_by_tag        (XmlNode* root, const char* tag_name);


/* ------------------------------------------------------------------ */
/* Element mutators                                                   */
/* ------------------------------------------------------------------ */

void         xml_set_element_text             (XmlNode* element, const char* text);
void         xml_set_element_attribute        (XmlNode* element, const char* name, const char* value);
int          xml_remove_element_attribute     (XmlNode* element, const char* name);


/* ------------------------------------------------------------------ */
/* Debug / printing / diagnostics                                     */
/* ------------------------------------------------------------------ */

void         xml_print                        (XmlNode* node);
const char*  xml_get_error                    (XmlDocument* doc);
const char** xml_get_processing_instructions  (XmlDocument* doc, const char* target);


#ifdef __cplusplus
}
#endif

#endif 
