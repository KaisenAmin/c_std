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
  - **Purpose**: Parses an XML file and returns an `XmlDocument` object. The XML file is loaded, parsed, and converted into an internal structure that represents the document.
  - **Parameters:**  
    - `filename`: The path to the XML file to be parsed.
  - **Returns:**  
    - An `XmlDocument` object representing the parsed XML structure. Returns `NULL` if the file cannot be read or parsed.

---

### `XmlDocument* xml_parse_string(const char* xml_content)`   
  - **Purpose**: Parses an XML string and returns an `XmlDocument` object. This function is useful for working with XML content stored in memory.
  - **Parameters:**  
    - `xml_content`: A string containing the XML data to be parsed.
  - **Returns:**  
    - An `XmlDocument` object representing the parsed XML content. Returns `NULL` if the parsing fails.

---

### `XmlDocument* xml_create_document(const char* root_element_name)` 
  - **Purpose**: Creates a new XML document with a specified root element. The root element is the first element in the document.
  - **Parameters:**  
    - `root_element_name`: The name of the root element to be created.
  - **Returns:**  
    - A new `XmlDocument` object with the specified root element.

---

### `XmlNode* xml_create_element(XmlDocument* doc, const char* tag_name)` 
  - **Purpose**: Creates a new XML element within the document. The element is not automatically added to the document and must be appended to a parent node.
  - **Parameters:**  
    - `doc`: The XML document where the element will belong.  
    - `tag_name`: The tag name of the element to be created.
  - **Returns:**  
    - An `XmlNode` representing the newly created element.

---

### `XmlNode* xml_get_root(XmlDocument* doc)`   
  - **Purpose**: Retrieves the root element of the XML document. This is typically the top-level element that contains all other elements.
  - **Parameters:**  
    - `doc`: The document from which to retrieve the root element.
  - **Returns:**  
    - The `XmlNode` representing the root element, or `NULL` if no root is found.

---

### `XmlNode* xml_find_element_by_tag(XmlNode* root, const char* tag_name)`
  - **Purpose**: Finds the first child element with a matching tag name. This function searches the immediate children of the specified root node.
  - **Parameters:**  
    - `root`: The root node where the search begins.  
    - `tag_name`: The tag name to search for.
  - **Returns:**  
    - The `XmlNode` representing the first found element with the matching tag name, or `NULL` if not found.

---

### `XmlNode* xml_get_element(XmlNode* root, ...)`  
  - **Purpose**: Retrieves a specific sub-element based on a variable argument list of tag names. This allows for traversing a path of nested elements.
  - **Parameters:**  
    - `root`: The root node where the search begins.  
    - `...`: A variable argument list representing the sequence of tag names to follow.
  - **Returns:**  
    - The `XmlNode` at the specified path, or `NULL` if the path is invalid.

---

### `void xml_append_child(XmlNode* parent, XmlNode* child)`  
  - **Purpose**: Appends a child element to a parent node, making it a sub-element of the parent.
  - **Parameters:**  
    - `parent`: The parent node to which the child will be appended.  
    - `child`: The child node to be appended.

---

### `void xml_set_element_text(XmlNode* element, const char* text)`  
  - **Purpose**: Sets the text content of an XML element. If the element already has text, it will be replaced.
  - **Parameters:**  
    - `element`: The XML element whose text will be set.  
    - `text`: The text content to assign to the element.

---

### `void xml_set_element_attribute(XmlNode* element, const char* name, const char* value)` 
  - **Purpose**: Sets an attribute for an XML element. If the attribute already exists, its value will be updated; otherwise, a new attribute will be added.
  - **Parameters:**  
    - `element`: The element to which the attribute will be added.  
    - `name`: The name of the attribute.  
    - `value`: The value of the attribute.

---

### `const char* xml_get_element_text(XmlNode* element)` 
  - **Purpose**: Retrieves the text content of an XML element.
  - **Parameters:**  
    - `element`: The XML element from which to retrieve the text.
  - **Returns:**  
    - The text content of the element, or `NULL` if the element has no text.

---

### `const char* xml_get_element_attribute(XmlNode* element, const char* name)`  
  - **Purpose**: Retrieves the value of an attribute from an XML element.
  - **Parameters:**  
    - `element`: The XML element to retrieve the attribute from.  
    - `name`: The name of the attribute to retrieve.
  - **Returns:**  
    - The value of the attribute, or `NULL` if the attribute is not found.

---

### `const char* xml_get_tag_name(XmlNode* node)` 
  - **Purpose**: Retrieves the tag name of a given XML node. This is useful for identifying the element type, allowing you to access or manipulate elements based on their tag names.
  - **Parameters:**  
    - `node`: The node whose tag name is being requested.
  - **Returns:**  
    - The tag name of the node, or `NULL` if the node has no tag name.

---

### `char* xml_to_string(XmlDocument* doc)`   
  - **Purpose**: Converts an XML document to a string format, which can then be printed or saved to a file.
  - **Parameters:**  
    - `doc`: The XML document to convert.
  - **Returns:**  
    - A string representing the XML document. The caller is responsible for freeing the string when done.

---

### `int xml_save_to_file(XmlDocument* doc, const char* filename)`  
  - **Purpose**: Saves an XML document to a file. The document is converted to its string representation and written to the specified file.
  - **Parameters:**  
    - `doc`: The XML document to save.  
    - `filename`: The name of the file to save the document to.
  - **Returns:**  
    - `1` if the document was saved successfully, `0` if there was an error.

---

### `void xml_cut(XmlNode* node)`   
  - **Purpose**: Removes a node and its subtags from the document without deallocating memory. This is useful when you want to move or manipulate a node before deletion.
  - **Parameters:**  
    - `node`: The node to be removed.

---

### `void xml_deallocate_document(XmlDocument* doc)`   
  - **Purpose**: Frees the memory used by the entire XML document, including all nodes and attributes.
  - **Parameters:**  
    - `doc`: The document to deallocate.

---

- **`void xml_deallocate_node(XmlNode* node)`  
  - **Purpose**: Frees the memory used by a specific XML node and its children, but does not affect other parts of the document.
  - **Parameters:**  
    - `node`: The node to deallocate.

---

### `const char* xml_get_error(XmlDocument* doc)`  
  - **Purpose**: Returns the last error message encountered while parsing the document. If no error occurred, it returns an empty string.
  - **Parameters:**  
    - `doc`: The document from which to retrieve the error message.
  - **Returns:**  
    - A string describing the last error, or an empty string if no error occurred.
---

### `const char** xml_get_processing_instructions(XmlDocument* doc, const char* target)`**  
  - **Purpose**: Retrieves processing instructions for the XML document based on the specified target. Processing instructions provide directions for applications processing the document.
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
<?xml version="1.0"?>
<catalog>
   
   <author>Gambardella, Matthew</author>
   <title>XML Developer's Guide</title>
   <genre>Computer</genre>
   <price>44.95</price>
   <publish_date>2000-10-01</publish_date>
   <description>An in-depth look at creating applications with XML.</description>
</catalog>

Root element: catalog
Author: Gambardella, Matthew
```

---

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

    xml_save_to_file(doc, "./output.xml");
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

---

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
Root internal node pointer: 000001fe02512490
Root tag name assigned correctly: catalog
Book ID: bk102
```

---

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

---

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

### Example 8: Adding Nested Elements and Modifying Attributes

```c
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

**Result:**
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

### Example 9: Traversing and Editing an XML Document

```c
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
            fmt_printf("Original Title: %s\n", xml_get_element_text(title));

            // Modify the book title
            xml_set_element_text(title, "Advanced C Programming");
        }

        XmlNode* author = xml_find_element_by_tag(book, "author");
        if (author) {
            fmt_printf("Author: %s\n", xml_get_element_text(author));
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

**Result:**
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

## License

This project is open-source and available under [ISC License].