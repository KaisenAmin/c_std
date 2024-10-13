#ifndef SECRETS_H_
#define SECRETS_H_

#include <stddef.h>

int secrets_randbelow(int n);
int secrets_compare_digest(const unsigned char *a, const unsigned char *b, size_t length);

void secrets_token_bytes(unsigned char *buffer, size_t nbytes);
void secrets_token_hex(char *buffer, size_t nbytes);
void secrets_token_urlsafe(char *buffer, size_t nbytes);

void* secrets_choice(const void* seq, size_t size, size_t elem_size);
unsigned int secrets_randbits(int k);

#endif 