#ifndef ENCODING_H
#define ENCODING_H

#include <stddef.h>

char* encoding_base64_encode(const char* input, size_t length);
char* encoding_base64_decode(const char* input, size_t length);
char* encoding_url_encode(const char* input, size_t length);
char* encoding_url_decode(const char* input, size_t lenght);

#endif // ENCODING_H