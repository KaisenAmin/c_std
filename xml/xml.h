/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Xml
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

// #define XML_LOGGING_ENABLE 

#ifdef XML_LOGGING_ENABLE
    #define XML_LOG(fmt, ...) \
        do { fprintf(stderr, "[XML LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define XML_LOG(fmt, ...) do { } while (0)
#endif

typedef struct XmlDocument XmlDocument;
typedef struct XmlNode XmlNode;
typedef struct XmlAttribute XmlAttribute; 
typedef struct ezxml *ezxml_t;

struct ezxml {
    char *name;      
    char **attr;     
    char *txt;       
    size_t off;      
    ezxml_t next;    
    ezxml_t sibling; 
    ezxml_t ordered; 
    ezxml_t child;   
    ezxml_t parent;  
    short flags;     
};

struct XmlNode {
    void* internal_node;  // Pointer to the internal ezxml node
    char* tag_name;       // Cached tag name (optional, can be retrieved from ezxml)
    char* text;           // Cached text content (optional)
    struct XmlNode* parent;
    struct XmlNode* children;
    struct XmlNode* next_sibling;
    struct XmlNode* attributes;
};

struct XmlDocument {
    XmlNode* root;
};

struct XmlAttribute {
    char* name;
    char* value;
    struct XmlAttribute* next; 
};

XmlDocument* xml_parse_file(const char* filename);
XmlDocument* xml_parse_string(const char* xml_content);
XmlDocument* xml_create_document(const char* root_element_name);
XmlDocument* xml_parse_file_stream(FILE* fp);

XmlNode* xml_create_element(XmlDocument* doc, const char* tag_name);
XmlNode* xml_get_root(XmlDocument* doc);
XmlNode* xml_find_element_by_tag(XmlNode* root, const char* tag_name);
XmlNode* xml_get_element(XmlNode* root, ...);

void xml_print(XmlNode* node);
void xml_append_child(XmlNode* parent, XmlNode* child);
void xml_set_element_text(XmlNode* element, const char* text);
void xml_set_element_attribute(XmlNode* element, const char* name, const char* value);
void xml_deallocate_document(XmlDocument* doc);
void xml_deallocate_node(XmlNode* node);
void xml_cut(XmlNode* node);

const char* xml_get_element_text(XmlNode* element);
const char* xml_get_element_attribute(XmlNode* element, const char* name);
const char* xml_get_error(XmlDocument* doc);
const char* xml_get_tag_name(XmlNode* node);

const char** xml_get_processing_instructions(XmlDocument* doc, const char* target);
char* xml_to_string(XmlDocument* doc);
int xml_save_to_file(XmlDocument* doc, const char* filename);

#ifdef __cplusplus 
}
#endif 

#endif
