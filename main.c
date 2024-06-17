#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());
    
    int random = 0;
    for (int i = 0; i < 5; i++) {
        random = random_randrange(50, 100, 10);
        fmt_printf("Random number between 50 and 100 with step 10: %d\n", random);
    }

    random = random_randrange(10, 2, -1);
    if (random == -1) {
        fmt_printf("Invalid range input.\n");
    }
    else {
        fmt_printf("Value is %d\n", random);
    }

    random = random_randrange(1, 10, 0);
    if (random == -1) {
        fmt_printf("Invalid step input.\n");
    }

    return 0;
}
