# XML Library in C

**Author:** Amin Tahmasebi  
**Release Date:** 2024  
**License:** ISC License

## Overview

This XML library in C provides a comprehensive and easy-to-use API for parsing, creating, modifying, and traversing XML documents. It is designed to be lightweight, efficient, and fully cross-platform, making it suitable for embedded systems, desktop applications, and server environments. The library abstracts the complexity of XML manipulation, offering intuitive functions to work with XML elements, attributes, and text content.

## Key Features

- **XML Parsing from File or String:** Parse XML content from files or strings to easily load and work with XML documents.
- **Element Creation and Manipulation:** Create new XML elements, set text content, and modify or add attributes.
- **Tree Traversal:** Traverse XML trees to access parent and child elements, as well as siblings.
- **Document Serialization:** Convert XML documents back to a string or save them to a file.
- **Cross-Platform:** Compatible with Linux, Windows, and other platforms.
- **Doxygen Documentation:** Fully documented using Doxygen for clear and comprehensive guidance on each function.

## Installation and Compilation

To use this library, include `xml.h` in your project and compile the source files with your C compiler. For GCC, the following command can be used:

```bash
gcc -std=c17 -O3 -march=native -flto -funroll-loops -Wall -Wextra -pedantic -s -o main ./main.c ./xml/xml.c
```

## Documentation

The documentation includes detailed descriptions of all the functions provided by the library, along with usage examples. It covers basic XML operations such as parsing XML files, creating elements, and traversing nodes in an XML document.


### Function Descriptions:

- **`XmlDocument* xml_parse_file(const char* filename)`**  
  Parses an XML file and returns an `XmlDocument` object. The XML file is loaded, parsed, and converted into an internal structure that represents the document.
  - **Parameters:**  
    - `filename`: The path to the XML file to be parsed.
  - **Returns:**  
    - An `XmlDocument` object representing the parsed XML structure. Returns `NULL` if the file cannot be read or parsed.

- **`XmlDocument* xml_parse_string(const char* xml_content)`**  
  Parses an XML string and returns an `XmlDocument` object. This function is useful for working with XML content stored in memory.
  - **Parameters:**  
    - `xml_content`: A string containing the XML data to be parsed.
  - **Returns:**  
    - An `XmlDocument` object representing the parsed XML content. Returns `NULL` if the parsing fails.

- **`XmlDocument* xml_create_document(const char* root_element_name)`**  
  Creates a new XML document with a specified root element. The root element is the first element in the document.
  - **Parameters:**  
    - `root_element_name`: The name of the root element to be created.
  - **Returns:**  
    - A new `XmlDocument` object with the specified root element.

- **`XmlNode* xml_create_element(XmlDocument* doc, const char* tag_name)`**  
  Creates a new XML element within the document. The element is not automatically added to the document and must be appended to a parent node.
  - **Parameters:**  
    - `doc`: The XML document where the element will belong.  
    - `tag_name`: The tag name of the element to be created.
  - **Returns:**  
    - An `XmlNode` representing the newly created element.

- **`XmlNode* xml_get_root(XmlDocument* doc)`**  
  Retrieves the root element of the XML document. This is typically the top-level element that contains all other elements.
  - **Parameters:**  
    - `doc`: The document from which to retrieve the root element.
  - **Returns:**  
    - The `XmlNode` representing the root element, or `NULL` if no root is found.

- **`XmlNode* xml_find_element_by_tag(XmlNode* root, const char* tag_name)`**  
  Finds the first child element with a matching tag name. This function searches the immediate children of the specified root node.
  - **Parameters:**  
    - `root`: The root node where the search begins.  
    - `tag_name`: The tag name to search for.
  - **Returns:**  
    - The `XmlNode` representing the first found element with the matching tag name, or `NULL` if not found.

- **`XmlNode* xml_get_element(XmlNode* root, ...)`**  
  Retrieves a specific sub-element based on a variable argument list of tag names. This allows for traversing a path of nested elements.
  - **Parameters:**  
    - `root`: The root node where the search begins.  
    - `...`: A variable argument list representing the sequence of tag names to follow.
  - **Returns:**  
    - The `XmlNode` at the specified path, or `NULL` if the path is invalid.

- **`void xml_append_child(XmlNode* parent, XmlNode* child)`**  
  Appends a child element to a parent node, making it a sub-element of the parent.
  - **Parameters:**  
    - `parent`: The parent node to which the child will be appended.  
    - `child`: The child node to be appended.

- **`void xml_set_element_text(XmlNode* element, const char* text)`**  
  Sets the text content of an XML element. If the element already has text, it will be replaced.
  - **Parameters:**  
    - `element`: The XML element whose text will be set.  
    - `text`: The text content to assign to the element.

- **`void xml_set_element_attribute(XmlNode* element, const char* name, const char* value)`**  
  Sets an attribute for an XML element. If the attribute already exists, its value will be updated; otherwise, a new attribute will be added.
  - **Parameters:**  
    - `element`: The element to which the attribute will be added.  
    - `name`: The name of the attribute.  
    - `value`: The value of the attribute.

- **`const char* xml_get_element_text(XmlNode* element)`**  
  Retrieves the text content of an XML element.
  - **Parameters:**  
    - `element`: The XML element from which to retrieve the text.
  - **Returns:**  
    - The text content of the element, or `NULL` if the element has no text.

- **`const char* xml_get_element_attribute(XmlNode* element, const char* name)`**  
  Retrieves the value of an attribute from an XML element.
  - **Parameters:**  
    - `element`: The XML element to retrieve the attribute from.  
    - `name`: The name of the attribute to retrieve.
  - **Returns:**  
    - The value of the attribute, or `NULL` if the attribute is not found.

- **`const char* xml_get_tag_name(XmlNode* node)`**  
  Retrieves the tag name of a given XML node. This is useful for identifying the element type, allowing you to access or manipulate elements based on their tag names.
  - **Parameters:**  
    - `node`: The node whose tag name is being requested.
  - **Returns:**  
    - The tag name of the node, or `NULL` if the node has no tag name.

- **`char* xml_to_string(XmlDocument* doc)`**  
  Converts an XML document to a string format, which can then be printed or saved to a file.
  - **Parameters:**  
    - `doc`: The XML document to convert.
  - **Returns:**  
    - A string representing the XML document. The caller is responsible for freeing the string when done.

- **`int xml_save_to_file(XmlDocument* doc, const char* filename)`**  
  Saves an XML document to a file. The document is converted to its string representation and written to the specified file.
  - **Parameters:**  
    - `doc`: The XML document to save.  
    - `filename`: The name of the file to save the document to.
  - **Returns:**  
    - `1` if the document was saved successfully, `0` if there was an error.

- **`void xml_cut(XmlNode* node)`**  
  Removes a node and its subtags from the document without deallocating memory. This is useful when you want to move or manipulate a node before deletion.
  - **Parameters:**  
    - `node`: The node to be removed.

- **`void xml_deallocate_document(XmlDocument* doc)`**  
  Frees the memory used by the entire XML document, including all nodes and attributes.
  - **Parameters:**  
    - `doc`: The document to deallocate.

- **`void xml_deallocate_node(XmlNode* node)`**  
  Frees the memory used by a specific XML node and its children, but does not affect other parts of the document.
  - **Parameters:**  
    - `node`: The node to deallocate.

- **`const char* xml_get_error(XmlDocument* doc)`**  
  Returns the last error message encountered while parsing the document. If no error occurred, it returns an empty string.
  - **Parameters:**  
    - `doc`: The document from which to retrieve the error message.
  - **Returns:**  
    - A string describing the last error, or an empty string if no error occurred.

- **`const char** xml_get_processing_instructions(XmlDocument* doc, const char* target)`**  
  Retrieves processing instructions for the XML document based on the specified target. Processing instructions provide directions for applications processing the document.
  - **Parameters:**  
    - `doc`: The document containing the processing instructions.  
    - `target`: The target of the processing instruction to retrieve.
  - **Returns:**  
    - A null-terminated array of strings representing the processing instructions, or `NULL` if none are found.


## Examples

### Example 1: Basic XML Parsing and Element Access

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    XmlDocument* doc = xml_parse_file("sample.xml");
    XmlNode* root = xml_get_root(doc);
    
    if (root) {
        fmt_printf("Root element: %s\n", xml_get_tag_name(root));

        XmlNode* author = xml_find_element_by_tag(root, "author");
        if (author) {
            fmt_printf("Author: %s\n", xml_get_element_text(author));
        }
    }

    xml_deallocate_document(doc);
    return 0;
}
```
**Result:**
```
Root element: catalog
Author: John Doe
```

### Example 2: Creating and Modifying an XML Document

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    XmlDocument* doc = xml_create_document("catalog");
    XmlNode* book = xml_create_element(doc, "book");

    fmt_printf("Created book node at %p\n", book);
    fmt_printf("Appending book node to root at %p\n", xml_get_root(doc));

    // Append the 'book' element to the root node
    if (book != xml_get_root(doc)) {
        xml_append_child(xml_get_root(doc), book);
    } 
    else {
        fmt_printf("Error: Attempting to append a node to itself or its parent.\n");
        xml_deallocate_document(doc);
        return 1;
    }

    // Set attributes and text for the 'book' node
    xml_set_element_attribute(book, "id", "bk101");
    xml_set_element_text(book, "XML Developer's Guide");

    xml_save_to_file(doc, "C:/Users/asus/OneDrive/Desktop/project/c_std/sources/output.xml");
    xml_deallocate_document(doc);

    return 0;
}
```
**Result:**
```
Generated XML:
<catalog>
   <book id="bk101">XML Developer's Guide</book>
</catalog>
```

### Example 3: Setting and Retrieving Element Attributes

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    XmlDocument* doc = xml_parse_string("<?xml version='1.0'?><catalog><book id='bk102'>C Programming</book></catalog>");
    
    if (!doc) {
        fmt_printf("Failed to parse document.\n");
        return 1;
    }

    XmlNode* root = xml_get_root(doc);
    if (!root) {
        fmt_printf("Failed to get root element.\n");
        xml_deallocate_document(doc);
        return 1;
    }

    fmt_printf("Root internal node pointer: %p\n", root->internal_node);
    if (root->tag_name) {
        fmt_printf("Root tag name assigned correctly: %s\n", root->tag_name);
    } 
    else {
        fmt_printf("Root tag name is empty or not assigned.\n");
    }

    XmlNode* book = xml_find_element_by_tag(root, "book");
    if (book) {
        const char* id = xml_get_element_attribute(book, "id");
        if (id) {
            fmt_printf("Book ID: %s\n", id);
        }
    } 
    else {
        fmt_printf("Book element not found.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}

```
**Result:**
```
Book ID: bk102
```

### Example 4: Converting an XML Document to a String

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"


int main() {
    XmlDocument* doc = xml_parse_string("<?xml version='1.0'?><catalog><book>Programming in C</book></catalog>");

    char* xml_string = xml_to_string(doc);
    if (xml_string) {
        fmt_printf("XML as string:\n%s\n", xml_string);
        free(xml_string);
    }

    xml_deallocate_document(doc);
    return 0;
}
```
**Result:**
```
XML as string:
<catalog>
   <book>Programming in C</book>
</catalog>
```

### Example 5: Adding Multiple Child Elements and Attributes

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    XmlDocument* doc = xml_create_document("catalog");

    XmlNode* book1 = xml_create_element(doc, "book");
    xml_append_child(xml_get_root(doc), book1);
    xml_set_element_attribute(book1, "id", "bk101");
    xml_set_element_text(book1, "XML Developer's Guide");

    XmlNode* book2 = xml_create_element(doc, "book");
    xml_append_child(xml_get_root(doc), book2);
    xml_set_element_attribute(book2, "id", "bk102");
    xml_set_element_text(book2, "Advanced C Programming");

    xml_save_to_file(doc, "catalog.xml");
    xml_deallocate_document(doc);

    return 0;
}
```
**Result:**
```
Generated XML:
<catalog>
   <book id="bk101">XML Developer's Guide</book>
   <book id="bk102">Advanced C Programming</book>
</catalog>
```

## Example 6 : read xml file 

```c
#include <stdlib.h>
#include <string.h>
#include "fmt/fmt.h"
#include "xml/xml.h"


int main() {
    XmlDocument* doc = xml_parse_file("C:/Users/asus/OneDrive/Desktop/project/c_std/sources/xml_sample.xml");

    if (doc == NULL) {
        fmt_fprintf(stderr, "Failed to parse the XML file.\n");
        return 1;
    }

    XmlNode* root = xml_get_root(doc);
    if (root == NULL) {
        fmt_fprintf(stderr, "Failed to get root element.\n");
        xml_deallocate_document(doc);
        return 1;
    }

    XmlNode* book = xml_find_element_by_tag(root, "book");
    if (book == NULL) {
        fmt_fprintf(stderr, "Failed to find <book> element.\n");
        xml_deallocate_document(doc);
        return 1;
    }

    XmlNode* author = xml_find_element_by_tag(book, "author");

    if (author != NULL) {
        const char* tag_name = xml_get_tag_name(author);
        fmt_printf("Author node found, tag name: %s\n", tag_name ? tag_name : "Unknown");
        
        const char* author_name = xml_get_element_text(author);
        if (author_name != NULL && strlen(author_name) > 0) { 
            fmt_printf("Author: %s\n", author_name);
        } 
        else {
            fmt_printf("Author node found, but no text available.\n");
        }
    } 
    else {
        fmt_printf("Author node not found.\n");
    }

    XmlNode* title = xml_find_element_by_tag(book, "title");
    if (title != NULL) {
        xml_set_element_text(title, "Updated XML Developer's Guide");
    }

    xml_set_element_attribute(book, "updated", "yes");

    char* xml_string = xml_to_string(doc); 
    if (xml_string != NULL) {
        fmt_printf("Updated XML:\n%s\n", xml_string);
        free(xml_string);
    }

    if (!xml_save_to_file(doc, "C:/Users/asus/OneDrive/Desktop/project/c_std/sources/updated_xml_sample.xml")) {
        fmt_fprintf(stderr, "Failed to save the updated XML file.\n");
    } 
    else {
        fmt_printf("Updated XML saved to 'updated_xml_sample.xml'.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}
```

## Example 7 : get elements 

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"


int main() {
    XmlDocument* doc = xml_parse_string("<?xml version='1.0'?><catalog><book id='bk101'><title>XML Developer's Guide</title><author>John Doe</author></book></catalog>");
    XmlNode* root = xml_get_root(doc);
    XmlNode* book = xml_get_element(root, "book", NULL);

    if (book) {
        XmlNode* title = xml_get_element(book, "title", NULL);

        if (title && title->tag_name) {
            const char* title_text = xml_get_element_text(title);
            if (title_text) {
                fmt_printf("Title: '%s'\n", title_text);
                free((void*)title_text);
            } 
        } 
    
        XmlNode* author = xml_get_element(book, "author", NULL);

        if (author && author->tag_name) {
            const char* author_text = xml_get_element_text(author);
            if (author_text) {
                fmt_printf("Author: '%s'\n", author_text);
                free((void*)author_text);
            } 
            
        } 
    } 

    xml_deallocate_document(doc);
    return 0;
}
```
## Conclusion

This XML library simplifies the process of working with XML documents in C projects, providing intuitive functions for common XML tasks such as parsing, element manipulation, and serialization. The provided examples illustrate the ease with which this library can be integrated into a variety of C-based applications.
