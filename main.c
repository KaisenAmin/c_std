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