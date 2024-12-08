/**
 * @author Amin Tahmasebi
 * @date 2024
 * @class Secrets
*/

#ifndef SECRETS_H_
#define SECRETS_H_

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus 
extern "C" {
#endif 

// #define SECRETS_LOGGING_ENABLE 

#ifdef SECRETS_LOGGING_ENABLE
    #define SECRETS_LOG(fmt, ...) \
        do { fprintf(stderr, "[SECRETS LOG] " fmt "\n", ##__VA_ARGS__); } while (0)
#else
    #define SECRETS_LOG(fmt, ...) do { } while (0)
#endif

int secrets_randbelow(int n);
int secrets_compare_digest(const unsigned char *a, const unsigned char *b, size_t length);

void secrets_token_bytes(unsigned char *buffer, size_t nbytes);
void secrets_token_hex(char *buffer, size_t nbytes);
void secrets_token_urlsafe(char *buffer, size_t nbytes);

void* secrets_choice(const void* seq, size_t size, size_t elem_size);
unsigned int secrets_randbits(int k);

#ifdef __cplusplus 
}
#endif 

#endif 