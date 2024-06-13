#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    unsigned int state;

    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 1, random_randint(1, 100));
    }

    // Get the current state
    random_getstate(&state);
    fmt_printf("State saved.\n");

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 6, random_randint(1, 100));
    }

    // Restore the state
    random_setstate(&state);
    fmt_printf("State restored.\n");

    for (int i = 0; i < 5; i++) {
        fmt_printf("Random number %d: %d\n", i + 6, random_randint(1, 100));
    }

    return 0;
}
