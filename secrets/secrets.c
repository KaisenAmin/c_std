#include "secrets.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifdef _WIN32
    #include <windows.h>
    #include <bcrypt.h>
#else
    #include <fcntl.h>
    #include <unistd.h>
#endif


/**
 * @brief Generate cryptographically secure random bytes.
 * 
 * @param buffer Pointer to the byte array where random bytes will be stored.
 * @param size Number of bytes to generate.
 */
void secrets_token_bytes(unsigned char *buffer, size_t size) {
#ifdef _WIN32
    if (!BCRYPT_SUCCESS(BCryptGenRandom(NULL, buffer, (ULONG)size, BCRYPT_USE_SYSTEM_PREFERRED_RNG))) {
        fprintf(stderr, "Error: BCryptGenRandom failed\n");
        exit(EXIT_FAILURE);
    }
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Error: Unable to open /dev/urandom\n");
        exit(EXIT_FAILURE);
    }
    if (read(fd, buffer, size) != (ssize_t)size) {
        fprintf(stderr, "Error: Unable to read from /dev/urandom\n");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
#endif
}

/**
 * @brief Generate a cryptographically secure random integer in the range [0, n).
 * 
 * @param n Upper limit (exclusive).
 * @return A cryptographically secure random integer.
 */
int secrets_randbelow(int n) {
    unsigned char buffer[sizeof(int)];
    secrets_token_bytes(buffer, sizeof(buffer));
    int random_value = *(int *)buffer;
    return abs(random_value) % n;
}

/**
 * @brief Generate a cryptographically secure random token in hexadecimal format.
 * 
 * @param buffer Pointer to the array where the token will be stored.
 * @param nbytes Number of bytes to generate (each byte is converted to two hex digits).
 */
void secrets_token_hex(char *buffer, size_t nbytes) {
    unsigned char *bytes = malloc(nbytes);
    secrets_token_bytes(bytes, nbytes);
    
    for (size_t i = 0; i < nbytes; i++) {
        sprintf(buffer + (i * 2), "%02x", bytes[i]);
    }

    free(bytes);
}

/**
 * @brief Generate a cryptographically secure random URL-safe token.
 * 
 * @param buffer A pointer to the array where the token will be stored.
 * @param nbytes The number of random bytes to encode as Base64.
 */
void secrets_token_urlsafe(char *buffer, size_t nbytes) {
    static const char urlsafe_table[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789-_";

    unsigned char *bytes = malloc(nbytes);
    secrets_token_bytes(bytes, nbytes);

    for (size_t i = 0; i < nbytes; i++) {
        buffer[i] = urlsafe_table[bytes[i] % 64];
    }

    free(bytes);
}

/**
 * @brief Compare two byte sequences in constant time to prevent timing attacks.
 * 
 * @param a First byte sequence.
 * @param b Second byte sequence.
 * @param length Length of the sequences to compare.
 * @return 1 if the sequences are equal, 0 otherwise.
 */
int secrets_compare_digest(const unsigned char *a, const unsigned char *b, size_t length) {
    unsigned char result = 0;
    for (size_t i = 0; i < length; i++) {
        result |= a[i] ^ b[i];
    }
    return result == 0;
}

/**
 * @brief Select a random element from a sequence (array) using cryptographically secure randomness.
 * 
 * @param seq Pointer to the sequence (array) of elements.
 * @param size The number of elements in the sequence.
 * @param elem_size Size of each element in bytes.
 * @return A pointer to the randomly selected element.
 */
void* secrets_choice(const void* seq, size_t size, size_t elem_size) {
    if (size == 0) {
        fprintf(stderr, "Error: Cannot choose from an empty sequence.\n");
        return NULL;
    }

    int random_index = secrets_randbelow(size);  // Get a random index securely
    return (void*)((char*)seq + (random_index * elem_size)); 
}

/**
 * @brief Generate a non-negative integer with exactly k random bits.
 * 
 * @param k Number of random bits to generate (must be between 1 and 32).
 * @return A random integer with exactly k random bits.
 */
unsigned int secrets_randbits(int k) {
    if (k <= 0 || k > (int)(sizeof(unsigned int) * CHAR_BIT)) {
        fprintf(stderr, "Error: k must be between 1 and %lu\n", sizeof(unsigned int) * CHAR_BIT);
        exit(EXIT_FAILURE);
    }

    unsigned int random_value = 0;
    secrets_token_bytes((unsigned char *)&random_value, sizeof(random_value));
    random_value &= (1U << k) - 1; // Mask the unused bits (if k < 32)
    
    return random_value;
}