#include "xml/xml.h"
#include "fmt/fmt.h"
#include <stdlib.h>

int main() {
    XmlDocument* doc = xml_parse_string("<?xml version='1.0'?><catalog><book id='bk101'><title>XML Developer's Guide</title><author>John Doe</author></book></catalog>");

    if (!doc) {
        fmt_printf("Failed to parse document.\n");
        return 1;
    }

    XmlNode* root = xml_get_root(doc);
    
    // Retrieve the 'book' element inside 'catalog'
    XmlNode* book = xml_get_element(root, "book", NULL);
    if (book) {
        // Retrieve the 'title' element inside 'book'
        XmlNode* title = xml_get_element(book, "title", NULL);
        if (title && title->tag_name) {
            const char* title_text = xml_get_element_text(title);
            if (title_text) {
                fmt_printf("Title: '%s'\n", title_text);
                free((void*)title_text);
            } 
            else {
                fmt_printf("Title text not found.\n");
            }
        } 
        else {
            fmt_printf("Title element not found.\n");
        }

        // Retrieve the 'author' element inside 'book'
        XmlNode* author = xml_get_element(book, "author", NULL);
        if (author && author->tag_name) {
            const char* author_text = xml_get_element_text(author);
            if (author_text) {
                fmt_printf("Author: '%s'\n", author_text);
                free((void*)author_text);
            } 
            else {
                fmt_printf("Author text not found.\n");
            }
        } else {
            fmt_printf("Author element not found.\n");
        }
    } else {
        fmt_printf("Book element not found.\n");
    }

    xml_deallocate_document(doc);
    return 0;
}