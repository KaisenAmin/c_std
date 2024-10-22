#include "fmt/fmt.h"
#include "secrets/secrets.h"

int main() {
    // Generate a random integer with 8 random bits
    unsigned int random_8bits = secrets_randbits(8);
    fmt_printf("Random 8-bit integer: %lu\n", random_8bits);

    // Generate a random integer with 16 random bits
    unsigned int random_16bits = secrets_randbits(16);
    fmt_printf("Random 16-bit integer: %lu\n", random_16bits);

    // Generate a random integer with 32 random bits
    unsigned int random_32bits = secrets_randbits(32);
    fmt_printf("Random 32-bit integer: %lu\n", random_32bits);

    return 0;
}