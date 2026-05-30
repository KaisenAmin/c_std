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

### `XmlDocument* xml_parse_file(const char* filename)`

**Purpose**: Parses an XML file and returns an `XmlDocument` object. The XML file is loaded, parsed, and converted into an internal structure that represents the document.

**Parameters**:
- `filename`: The path to the XML file to be parsed.

**Return Value**: An `XmlDocument` object representing the parsed XML structure. Returns `NULL` if the file cannot be read or parsed.

**Usage Case**: Use when reading XML data from a file on disk.

---

### `XmlDocument* xml_parse_string(const char* xml_content)`

**Purpose**: Parses an XML string and returns an `XmlDocument` object. This function is useful for working with XML content stored in memory.

**Parameters**:
- `xml_content`: A string containing the XML data to be parsed.

**Return Value**: An `XmlDocument` object representing the parsed XML content. Returns `NULL` if the parsing fails.

**Usage Case**: Use when XML content is already in a string buffer in memory.

---

### `XmlDocument* xml_create_document(const char* root_element_name)`

**Purpose**: Creates a new XML document with a specified root element. The root element is the first element in the document.

**Parameters**:
- `root_element_name`: The name of the root element to be created.

**Return Value**: A new `XmlDocument` object with the specified root element.

**Usage Case**: Use when building an XML document from scratch rather than parsing an existing one.

---

### `XmlNode* xml_create_element(XmlDocument* doc, const char* tag_name)`

**Purpose**: Creates a new XML element within the document. The element is not automatically added to the document and must be appended to a parent node.

**Parameters**:
- `doc`: The XML document where the element will belong.
- `tag_name`: The tag name of the element to be created.

**Return Value**: An `XmlNode` representing the newly created element.

**Usage Case**: Use to create child elements before attaching them to a parent with `xml_append_child`.

---

### `XmlNode* xml_get_root(XmlDocument* doc)`

**Purpose**: Retrieves the root element of the XML document. This is typically the top-level element that contains all other elements.

**Parameters**:
- `doc`: The document from which to retrieve the root element.

**Return Value**: The `XmlNode` representing the root element, or `NULL` if no root is found.

**Usage Case**: Use as the starting point for any tree traversal or search operation.

---

### `XmlNode* xml_find_element_by_tag(XmlNode* root, const char* tag_name)`

**Purpose**: Finds the first child element with a matching tag name. This function searches the immediate children of the specified root node.

**Parameters**:
- `root`: The root node where the search begins.
- `tag_name`: The tag name to search for.

**Return Value**: The `XmlNode` representing the first found element with the matching tag name, or `NULL` if not found.

**Usage Case**: Use to quickly locate a known child element by its tag when the document structure is familiar.

---

### `XmlNode* xml_get_element(XmlNode* root, ...)`

**Purpose**: Retrieves a specific sub-element based on a variable argument list of tag names. This allows for traversing a path of nested elements.

**Parameters**:
- `root`: The root node where the search begins.
- `...`: A variable argument list representing the sequence of tag names to follow (terminated by `NULL`).

**Return Value**: The `XmlNode` at the specified path, or `NULL` if the path is invalid.

**Usage Case**: Use to navigate a known path of nested elements in one call.

---

### `void xml_append_child(XmlNode* parent, XmlNode* child)`

**Purpose**: Appends a child element to a parent node, making it a sub-element of the parent.

**Parameters**:
- `parent`: The parent node to which the child will be appended.
- `child`: The child node to be appended.

**Return Value**: None.

**Usage Case**: Use after creating an element with `xml_create_element` to attach it to the document tree.

---

### `void xml_set_element_text(XmlNode* element, const char* text)`

**Purpose**: Sets the text content of an XML element. If the element already has text, it will be replaced.

**Parameters**:
- `element`: The XML element whose text will be set.
- `text`: The text content to assign to the element.

**Return Value**: None.

**Usage Case**: Use to assign or update the textual content of a leaf element.

---

### `void xml_set_element_attribute(XmlNode* element, const char* name, const char* value)`

**Purpose**: Sets an attribute for an XML element. If the attribute already exists, its value will be updated; otherwise, a new attribute will be added.

**Parameters**:
- `element`: The element to which the attribute will be added.
- `name`: The name of the attribute.
- `value`: The value of the attribute.

**Return Value**: None.

**Usage Case**: Use to add or update key-value metadata on an element.

---

### `const char* xml_get_element_text(XmlNode* element)`

**Purpose**: Retrieves the text content of an XML element.

**Parameters**:
- `element`: The XML element from which to retrieve the text.

**Return Value**: The text content of the element, or `NULL` if the element has no text.

**Usage Case**: Use to extract the text value from a leaf element.

---

### `const char* xml_get_element_attribute(XmlNode* element, const char* name)`

**Purpose**: Retrieves the value of an attribute from an XML element.

**Parameters**:
- `element`: The XML element to retrieve the attribute from.
- `name`: The name of the attribute to retrieve.

**Return Value**: The value of the attribute, or `NULL` if the attribute is not found.

**Usage Case**: Use to read a specific attribute value from an element.

---

### `const char* xml_get_tag_name(XmlNode* node)`

**Purpose**: Retrieves the tag name of a given XML node. This is useful for identifying the element type, allowing you to access or manipulate elements based on their tag names.

**Parameters**:
- `node`: The node whose tag name is being requested.

**Return Value**: The tag name of the node, or `NULL` if the node has no tag name.

**Usage Case**: Use when iterating over children and you need to check or display each element's type.

---

### `char* xml_to_string(XmlDocument* doc)`

**Purpose**: Converts an XML document to a string format, which can then be printed or saved to a file.

**Parameters**:
- `doc`: The XML document to convert.

**Return Value**: A string representing the XML document. The caller is responsible for freeing the string when done.

**Usage Case**: Use to serialize the document to a string for printing, logging, or transmission.

---

### `int xml_save_to_file(XmlDocument* doc, const char* filename)`

**Purpose**: Saves an XML document to a file. The document is converted to its string representation and written to the specified file.

**Parameters**:
- `doc`: The XML document to save.
- `filename`: The name of the file to save the document to.

**Return Value**: `1` if the document was saved successfully, `0` if there was an error.

**Usage Case**: Use to persist an XML document to disk after creating or modifying it.

---

### `void xml_cut(XmlNode* node)`

**Purpose**: Removes a node and its subtags from the document without deallocating memory. This is useful when you want to move or manipulate a node before deletion.

**Parameters**:
- `node`: The node to be removed.

**Return Value**: None.

**Usage Case**: Use to detach a subtree from the document before moving or freeing it.

---

### `void xml_deallocate_document(XmlDocument* doc)`

**Purpose**: Frees the memory used by the entire XML document, including all nodes and attributes.

**Parameters**:
- `doc`: The document to deallocate.

**Return Value**: None.

**Usage Case**: Call when finished with a document to release all associated memory.

---

### `void xml_deallocate_node(XmlNode* node)`

**Purpose**: Frees the memory used by a specific XML node and its children, but does not affect other parts of the document.

**Parameters**:
- `node`: The node to deallocate.

**Return Value**: None.

**Usage Case**: Use after `xml_cut` to free a detached subtree without destroying the whole document.

---

### `const char* xml_get_error(XmlDocument* doc)`

**Purpose**: Returns the last error message encountered while parsing the document. If no error occurred, it returns an empty string.

**Parameters**:
- `doc`: The document from which to retrieve the error message.

**Return Value**: A string describing the last error, or an empty string if no error occurred.

**Usage Case**: Use after a parse operation to check whether any errors were reported.
---

### `const char** xml_get_processing_instructions(XmlDocument* doc, const char* target)`

**Purpose**: Retrieves processing instructions for the XML document based on the specified target. Processing instructions provide directions for applications processing the document.

**Parameters**:
- `doc`: The document containing the processing instructions.
- `target`: The target of the processing instruction to retrieve.

**Return Value**: A null-terminated array of strings representing the processing instructions, or `NULL` if none are found.

**Usage Case**: Use to inspect `<?target ...?>` directives embedded in an XML document.


---

### `size_t xml_count_children(const XmlNode* node)`

**Purpose**: Count the **direct** children of an element. Unlike `xml_count_elements_by_tag`, this does not recurse — only first-level children are counted.

**Parameters**:
- `node`: Element whose children should be counted. May be `NULL`.

**Return Value**: The number of direct children. `0` if `node` is `NULL`, has no internal node, or has no children.

**Usage Case**: Use to quickly determine how many immediate children an element has.

---

### `XmlNode** xml_get_children(XmlNode* node, size_t* out_count)`

**Purpose**: Snapshot the direct children of `node` as a `NULL`-terminated array of fresh `XmlNode*` wrappers.

**Parameters**:
- `node`: Element to enumerate. May be `NULL`.
- `out_count`: Receives the number of children (excluding the trailing `NULL`). May be `NULL` if you don't care.

**Return Value**: A newly-allocated, `NULL`-terminated array of wrappers, or `NULL` on bad input / allocation failure. Caller frees every wrapper with `free()` AND the array itself with `free()`. `tag_name` and `text` inside each wrapper are borrowed pointers into the document — do not free them individually.

**Usage Case**: Use when you need to iterate over all direct children of a node.

---

### `XmlNode* xml_find_element_by_attribute(XmlNode* root, const char* attr_name, const char* attr_value)`

**Purpose**: Depth-first search for the first element (in document order) whose attribute `attr_name` equals `attr_value`. If `attr_value` is `NULL`, matches any element that simply HAS the attribute (regardless of its value).

**Parameters**:
- `root`: Root of the subtree to search. May be `NULL`.
- `attr_name`: Attribute name (case-sensitive). Must NOT be `NULL`.
- `attr_value`: Attribute value to match. `NULL` means "any value".

**Return Value**: A newly-allocated `XmlNode*` wrapper around the match (caller frees with `free()`), or `NULL` if no match / bad input. The root itself is checked first, so a root carrying the attribute is a valid result.

**Usage Case**: Use to locate an element by a known attribute value (e.g., find element with `id="b2"`).

---

### `int xml_remove_element_attribute(XmlNode* element, const char* name)`

**Purpose**: Remove a named attribute from an element. Symmetric to `xml_set_element_attribute`. Removing a missing attribute is a silent success (return `1`).

**Parameters**:
- `element`: Element to modify. May be `NULL`.
- `name`: Attribute name to remove. May be `NULL`.

**Return Value**: `1` on success (removed OR wasn't present); `0` if `element` / `name` is `NULL`.

**Usage Case**: Use to strip an unwanted attribute from an element before serialization.

---

### `size_t xml_count_elements_by_tag(XmlNode* root, const char* tag_name)`

**Purpose**: Count every descendant element (recursively) with the given tag name. The root itself is NOT counted — only its strict descendants are.

**Parameters**:
- `root`: Root of the subtree. May be `NULL`.
- `tag_name`: Tag name (case-sensitive). May be `NULL`.

**Return Value**: The number of matching descendants. `0` on bad input.

**Usage Case**: Use to count how many elements with a given tag exist anywhere beneath a node.


## Examples

## Example 1: Basic XML Parsing and Element Access

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
**Result**
```
Root element: catalog
Author: Gambardella, Matthew
```

---

## Example 2: Creating and Modifying an XML Document

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

    xml_save_to_file(doc, "./output.xml");
    xml_deallocate_document(doc);

    return 0;
}
```
**Result**
```
Created book node at 0x... (address may vary)
Appending book node to root at 0x... (address may vary)
```

---

## Example 3: Setting and Retrieving Element Attributes

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
**Result**
```
Root internal node pointer: 0x... (address may vary)
Root tag name assigned correctly: catalog
Book ID: bk102
```

---

## Example 4: Converting an XML Document to a String

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
**Result**
```
XML as string:
<catalog>
   <book>Programming in C</book>
</catalog>
```

---

## Example 5: Adding Multiple Child Elements and Attributes

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
**Result**
```
(no stdout output — the document is saved to catalog.xml)
```

---

## Example 6 : read xml file 

```c
#include <stdlib.h>
#include <string.h>
#include "fmt/fmt.h"
#include "xml/xml.h"


int main() {
    XmlDocument* doc = xml_parse_file("./sources/xml_sample.xml");

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
        free((void*)author_name);   /* xml_get_element_text returns a malloc'd copy */
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

    if (!xml_save_to_file(doc, "./updated_xml_sample.xml")) {
        fmt_fprintf(stderr, "Failed to save the updated XML file.\n");
    } 
    else {
        fmt_printf("Updated XML saved to 'updated_xml_sample.xml'.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}
```
**Result**
```
Author node found, tag name: author
Author: Gambardella, Matthew
Updated XML:
<catalog>
   <book id="bk101" updated="yes">
      <author>Gambardella, Matthew</author>
      <title>Updated XML Developer's Guide</title>
      <genre>Computer</genre>
      <price>44.95</price>
      <publish_date>2000-10-01</publish_date>
      <description>An in-depth look at creating applications
      with XML.</description>
   </book>
   <book id="bk102">
      <author>Ralls, Kim</author>
      <title>Midnight Rain</title>
      <genre>Fantasy</genre>
      <price>5.95</price>
      <publish_date>2000-12-16</publish_date>
      <description>A former architect battles corporate zombies,
      an evil sorceress, and her own childhood to become queen
      of the world.</description>
   </book>
   <book id="bk103">
      <author>Corets, Eva</author>
      <title>Maeve Ascendant</title>
      <genre>Fantasy</genre>
      <price>5.95</price>
      <publish_date>2000-11-17</publish_date>
      <description>After the collapse of a nanotechnology
      society in England, the young survivors lay the
      foundation for a new society.</description>
   </book>
   <book id="bk104">
      <author>Corets, Eva</author>
      <title>Oberon's Legacy</title>
      <genre>Fantasy</genre>
      <price>5.95</price>
      <publish_date>2001-03-10</publish_date>
      <description>In post-apocalypse England, the mysterious
      agent known only as Oberon helps to create a new life
      for the inhabitants of London. Sequel to Maeve
      Ascendant.</description>
   </book>
   <book id="bk105">
      <author>Corets, Eva</author>
      <title>The Sundered Grail</title>
      <genre>Fantasy</genre>
      <price>5.95</price>
      <publish_date>2001-09-10</publish_date>
      <description>The two daughters of Maeve, half-sisters,
      battle one another for control of England. Sequel to
      Oberon's Legacy.</description>
   </book>
   <book id="bk106">
      <author>Randall, Cynthia</author>
      <title>Lover Birds</title>
      <genre>Romance</genre>
      <price>4.95</price>
      <publish_date>2000-09-02</publish_date>
      <description>When Carla meets Paul at an ornithology
      conference, tempers fly as feathers get ruffled.</description>
   </book>
   <book id="bk107">
      <author>Thurman, Paula</author>
      <title>Splish Splash</title>
      <genre>Romance</genre>
      <price>4.95</price>
      <publish_date>2000-11-02</publish_date>
      <description>A deep sea diver finds true love twenty
      thousand leagues beneath the sea.</description>
   </book>
   <book id="bk108">
      <author>Knorr, Stefan</author>
      <title>Creepy Crawlies</title>
      <genre>Horror</genre>
      <price>4.95</price>
      <publish_date>2000-12-06</publish_date>
      <description>An anthology of horror stories about roaches,
      centipedes, scorpions  and other insects.</description>
   </book>
   <book id="bk109">
      <author>Kress, Peter</author>
      <title>Paradox Lost</title>
      <genre>Science Fiction</genre>
      <price>6.95</price>
      <publish_date>2000-11-02</publish_date>
      <description>After an inadvertant trip through a Heisenberg
      Uncertainty Device, James Salway discovers the problems
      of being quantum.</description>
   </book>
   <book id="bk110">
      <author>O'Brien, Tim</author>
      <title>Microsoft .NET: The Programming Bible</title>
      <genre>Computer</genre>
      <price>36.95</price>
      <publish_date>2000-12-09</publish_date>
      <description>Microsoft's .NET initiative is explored in
      detail in this deep programmer's reference.</description>
   </book>
   <book id="bk111">
      <author>O'Brien, Tim</author>
      <title>MSXML3: A Comprehensive Guide</title>
      <genre>Computer</genre>
      <price>36.95</price>
      <publish_date>2000-12-01</publish_date>
      <description>The Microsoft MSXML3 parser is covered in
      detail, with attention to XML DOM interfaces, XSLT processing,
      SAX and more.</description>
   </book>
   <book id="bk112">
      <author>Galos, Mike</author>
      <title>Visual Studio 7: A Comprehensive Guide</title>
      <genre>Computer</genre>
      <price>49.95</price>
      <publish_date>2001-04-16</publish_date>
      <description>Microsoft Visual Studio 7 is explored in depth,
      looking at how Visual Basic, Visual C++, C#, and ASP+ are
      integrated into a comprehensive development
      environment.</description>
   </book>
</catalog>
Updated XML saved to 'updated_xml_sample.xml'.
```

---

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
**Result**
```
Title: 'XML Developer's Guide'
Author: 'John Doe'
```

---

## Example 8: Adding Nested Elements and Modifying Attributes

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    // create root element as `store`
    XmlDocument* doc = xml_create_document("store");

    XmlNode* product = xml_create_element(doc, "product");
    xml_append_child(xml_get_root(doc), product);

    xml_set_element_attribute(product, "id", "p1001");
    xml_set_element_attribute(product, "category", "electronics");

    // Create a nested 'name' element and set its text content
    XmlNode* name = xml_create_element(doc, "name");
    xml_set_element_text(name, "Laptop");
    xml_append_child(product, name);

    // Create a nested 'price' element and set its text content
    XmlNode* price = xml_create_element(doc, "price");
    xml_set_element_text(price, "1200.00");
    xml_append_child(product, price);

    xml_set_element_attribute(product, "discounted", "yes");

    char* xml_string = xml_to_string(doc);
    if (xml_string) {
        fmt_printf("Generated XML:\n%s\n", xml_string);
        free(xml_string);
    }

    if (!xml_save_to_file(doc, "store.xml")) {
        fmt_fprintf(stderr, "Failed to save the XML document.\n");
    } 
    else {
        fmt_printf("XML saved to 'store.xml'.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}
```

**Result**
```
Generated XML:
<store>
   <product id="p1001" category="electronics" discounted="yes">
      <name>Laptop</name>
      <price>1200.00</price>
   </product>
</store>

XML saved to 'store.xml'.
```

---

## Example 9: Traversing and Editing an XML Document

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"

int main() {
    const char* xml_data = "<?xml version='1.0'?><library><book id='b001'><title>Programming in C</title><author>John Doe</author></book></library>";
    XmlDocument* doc = xml_parse_string(xml_data);

    if (!doc) {
        fmt_fprintf(stderr, "Failed to parse XML string.\n");
        return 1;
    }

    XmlNode* root = xml_get_root(doc);
    if (!root) {
        fmt_fprintf(stderr, "Failed to get root element.\n");
        xml_deallocate_document(doc);
        return 1;
    }

    XmlNode* book = xml_find_element_by_tag(root, "book");
    if (book) {
        const char* id = xml_get_element_attribute(book, "id");
        if (id) {
            fmt_printf("Book ID: %s\n", id);
        }

        XmlNode* title = xml_find_element_by_tag(book, "title");
        if (title) {
            const char* title_text = xml_get_element_text(title);
            fmt_printf("Original Title: %s\n", title_text);
            free((void*)title_text);

            // Modify the book title
            xml_set_element_text(title, "Advanced C Programming");
        }

        XmlNode* author = xml_find_element_by_tag(book, "author");
        if (author) {
            const char* author_text = xml_get_element_text(author);
            fmt_printf("Author: %s\n", author_text);
            free((void*)author_text);
        }

        // Add a new 'year' element
        XmlNode* year = xml_create_element(doc, "year");
        xml_set_element_text(year, "2024");
        xml_append_child(book, year);
    }

    char* updated_xml = xml_to_string(doc);
    if (updated_xml) {
        fmt_printf("Updated XML:\n%s\n", updated_xml);
        free(updated_xml);
    }

    if (!xml_save_to_file(doc, "library_updated.xml")) {
        fmt_fprintf(stderr, "Failed to save the updated XML file.\n");
    } 
    else {
        fmt_printf("Updated XML saved to 'library_updated.xml'.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}
```

**Result**
```
Book ID: b001
Original Title: Programming in C
Author: John Doe
Updated XML:
<library>
   <book id="b001">
      <title>Advanced C Programming</title>
      <author>John Doe</author>
      <year>2024</year>
   </book>
</library>

Updated XML saved to 'library_updated.xml'.
```

---

## Example 10: Counting direct children with `xml_count_children`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    const char* src =
        "<order>"
          "<item id=\"a\"/>"
          "<item id=\"b\"/>"
          "<item id=\"c\"/>"
        "</order>";

    XmlDocument* d = xml_parse_string(src);
    fmt_printf("order has %zu direct children\n", xml_count_children(xml_get_root(d)));

    xml_deallocate_document(d);
    return 0;
}
```

**Result**
```
order has 3 direct children
```

---

## Example 11: Iterating children with `xml_get_children`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    XmlDocument* d = xml_parse_string(
        "<users>"
          "<user name=\"alice\"/>"
          "<user name=\"bob\"/>"
          "<user name=\"carol\"/>"
        "</users>");

    size_t n = 0;
    XmlNode** kids = xml_get_children(xml_get_root(d), &n);
    for (size_t i = 0; i < n; ++i) {
        fmt_printf("[%zu] tag=%s name=%s\n",
                   i,
                   xml_get_tag_name(kids[i]),
                   xml_get_element_attribute(kids[i], "name"));
    }

    /* The child wrappers are owned by the document; free only the array. */
    free(kids);
    xml_deallocate_document(d);

    return 0;
}
```

**Result**
```
[0] tag=user name=alice
[1] tag=user name=bob
[2] tag=user name=carol
```

---

## Example 12: Searching by attribute with `xml_find_element_by_attribute`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    XmlDocument* d = xml_parse_string(
        "<library>"
          "<book id=\"b1\"><title>The C Programming Language</title></book>"
          "<book id=\"b2\"><title>Programming Pearls</title></book>"
          "<book id=\"b3\"><title>Code</title></book>"
        "</library>");

    XmlNode* hit = xml_find_element_by_attribute(
        xml_get_root(d), "id", "b2");

    if (hit) {
        XmlNode* title = xml_find_element_by_tag(hit, "title");
        const char* title_text = title ? xml_get_element_text(title) : NULL;
        fmt_printf("found book id=b2: %s\n", title_text ? title_text : "(no title)");
        free((void*)title_text);
        /* `hit` and `title` are owned by the document — no per-wrapper free. */
    }
    xml_deallocate_document(d);
    return 0;
}
```

**Result**
```
found book id=b2: Programming Pearls
```

---

## Example 13: Stripping an attribute with `xml_remove_element_attribute`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    XmlDocument* d = xml_parse_string(
        "<config draft=\"yes\" version=\"2\"><tls/></config>");

    XmlNode* root = xml_get_root(d);
    xml_remove_element_attribute(root, "draft");

    char* out = xml_to_string(d);
    fmt_printf("%s\n", out);

    free(out);
    xml_deallocate_document(d);

    return 0;
}
```

**Result**
```
<config version="2"><tls></tls></config>
```

> The serializer emits flat, unindented XML and writes empty elements as `<tag></tag>` rather than the self-closing `<tag/>` form. The output is still well-formed; load it back via `xml_parse_string` if you want to verify.

---

## Example 14: Counting descendants by tag with `xml_count_elements_by_tag`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    XmlDocument* d = xml_parse_string(
        "<library>"
          "<section name=\"fiction\">"
             "<book><title>Dune</title></book>"
             "<book><title>1984</title></book>"
          "</section>"
          "<section name=\"non-fiction\">"
             "<book><title>SICP</title></book>"
          "</section>"
        "</library>");

    XmlNode* root = xml_get_root(d);
    fmt_printf("books across all sections: %zu\n", xml_count_elements_by_tag(root, "book"));
    fmt_printf("section count: %zu\n", xml_count_elements_by_tag(root, "section"));

    xml_deallocate_document(d);
    return 0;
}
```

**Result**
```
books across all sections: 3
section count: 2
```

---

## Example 15: Removing a subtree with `xml_cut`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main(void) {
    XmlDocument* d = xml_parse_string(
        "<catalog>"
          "<book id=\"keep\">Keeper</book>"
          "<book id=\"remove\">Gone</book>"
          "<book id=\"also-keep\">Survivor</book>"
        "</catalog>");

    XmlNode* root = xml_get_root(d);
    fmt_printf("children before cut: %zu\n", xml_count_children(root));

    /* Find and detach the book we want to remove. */
    XmlNode* gone = xml_find_element_by_attribute(root, "id", "remove");
    if (gone) {
        xml_cut(gone);           /* detaches from tree, does NOT free */
        xml_deallocate_node(gone); /* free the detached subtree + wrapper */
    }

    fmt_printf("children after cut:  %zu\n", xml_count_children(root));

    char* out = xml_to_string(d);
    fmt_printf("%s\n", out);
    free(out);

    /* `root` (and any other getter wrapper) is owned by the document. */
    xml_deallocate_document(d);

    return 0;
}
```

**Result**
```
children before cut: 3
children after cut:  2
<catalog><book id="keep">Keeper</book><book id="also-keep">Survivor</book></catalog>
```

---

## Example 16: Parsing from a `FILE*` stream with `xml_parse_file_stream`

```c
#include <stdio.h>
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    /* Write a small XML file to disk. */
    FILE* fw = fopen("data.xml", "w");
    fprintf(fw, "<data><value>42</value><label>hello</label></data>");
    fclose(fw);

    /* Re-open and parse via the stream API. */
    FILE* fr = fopen("data.xml", "r");
    XmlDocument* doc = xml_parse_file_stream(fr);
    fclose(fr);

    XmlNode* root  = xml_get_root(doc);
    XmlNode* value = xml_find_element_by_tag(root, "value");
    XmlNode* label = xml_find_element_by_tag(root, "label");

    const char* value_text = xml_get_element_text(value);
    const char* label_text = xml_get_element_text(label);
    fmt_printf("value: %s\n", value_text);
    fmt_printf("label: %s\n", label_text);
    free((void*)value_text);
    free((void*)label_text);

    /* `root`, `value`, `label` are owned by the document. */
    xml_deallocate_document(doc);

    return 0;
}
```

**Result**
```
value: 42
label: hello
```

---

## Example 17: Reading processing instructions with `xml_get_processing_instructions`

```c
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    XmlDocument* doc = xml_parse_string(
        "<?xml-stylesheet type=\"text/xsl\" href=\"style.xsl\"?>"
        "<root><item/></root>");

    const char** pi = xml_get_processing_instructions(doc, "xml-stylesheet");
    if (pi && pi[0]) {
        fmt_printf("xml-stylesheet PI: %s\n", pi[0]);
    }

    /* Unknown target returns an empty (non-NULL) array. */
    const char** none = xml_get_processing_instructions(doc, "nonexistent");
    fmt_printf("nonexistent PI[0] is %s\n", (none && none[0]) ? none[0] : "(empty)");

    xml_deallocate_document(doc);
    return 0;
}
```

**Result**
```
xml-stylesheet PI: type="text/xsl" href="style.xsl"
nonexistent PI[0] is (empty)
```

---

## Example 18: Save and reload — `xml_save_to_file` + `xml_parse_file` round-trip

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    /* Build a small document and persist it. */
    XmlDocument* doc = xml_create_document("inventory");
    XmlNode* root    = xml_get_root(doc);
    XmlNode* item    = xml_create_element(doc, "item");

    xml_append_child(root, item);
    xml_set_element_attribute(item, "sku",  "A123");
    xml_set_element_text(item, "Widget");

    if (xml_save_to_file(doc, "inventory.xml")) {
        fmt_printf("Saved to inventory.xml\n");
    }
    /* `item` (from xml_create_element) and `root` (from xml_get_root) are
     * owned by the document and freed by xml_deallocate_document. */
    xml_deallocate_document(doc);

    /* Re-parse from file and verify contents. */
    XmlDocument* doc2 = xml_parse_file("inventory.xml");
    XmlNode* root2    = xml_get_root(doc2);
    XmlNode* item2    = xml_find_element_by_tag(root2, "item");

    fmt_printf("root tag : %s\n", xml_get_tag_name(root2));
    fmt_printf("sku attr : %s\n", xml_get_element_attribute(item2, "sku"));

    const char* txt = xml_get_element_text(item2);
    fmt_printf("text     : %s\n", txt);
    free((void*)txt);

    /* `root2` and `item2` are owned by doc2. */
    xml_deallocate_document(doc2);

    return 0;
}
```

**Result**
```
Saved to inventory.xml
root tag : inventory
sku attr : A123
text     : Widget
```

---

## Example 19: Attribute overwrite and removal — `xml_set_element_attribute` + `xml_remove_element_attribute`

```c
#include <stdlib.h>
#include "xml/xml.h"
#include "fmt/fmt.h"

int main(void) {
    XmlDocument* doc  = xml_create_document("server");
    XmlNode*     root = xml_get_root(doc);

    /* Set three attributes. */
    xml_set_element_attribute(root, "host", "localhost");
    xml_set_element_attribute(root, "port", "8080");
    xml_set_element_attribute(root, "ssl",  "no");

    /* Overwrite port and ssl. */
    xml_set_element_attribute(root, "port", "443");
    xml_set_element_attribute(root, "ssl",  "yes");

    /* Remove port entirely. */
    xml_remove_element_attribute(root, "port");

    char* out = xml_to_string(doc);
    fmt_printf("%s\n", out);   /* port is gone; host and ssl remain */
    free(out);

    /* Removing a non-existent attribute is a silent no-op (returns 1). */
    int rc = xml_remove_element_attribute(root, "port");
    fmt_printf("remove non-existent returned: %d\n", rc);

    /* `root` is owned by the document. */
    xml_deallocate_document(doc);

    return 0;
}
```

**Result**
```
<server host="localhost" ssl="yes"></server>
remove non-existent returned: 1
```

---

## License

This project is open-source and available under [ISC License].