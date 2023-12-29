#include "string/string.h"
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>

int main() 
{
    char* originalStr = u8"Привет, мир! مرحبا بالعالم"; // "Hello, world!" in Russian and Arabic

    // Convert to wide string (Unicode)
    wchar_t* unicodeStr = string_to_unicode(originalStr);
    if (unicodeStr == NULL) 
    {
        printf("Conversion to wide string failed.\n");
        return 1;
    }

    wprintf(L"Unicode String: %ls\n", unicodeStr);

    // Convert back to String object
    String* convertedStr = string_from_unicode(unicodeStr);
    if (convertedStr == NULL) 
    {
        printf("Conversion from wide string failed.\n");
        free(unicodeStr);
        return 1;
    }
    printf("Converted String: %s\n", convertedStr->dataStr);

    // Clean up
    free(unicodeStr);
    string_deallocate(convertedStr);

    return 0;
}