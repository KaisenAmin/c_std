#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 5; i++) {
        double random = random_random();
        fmt_printf("Random floating-point number between 0 and 1: %f\n", random);
    }

    return 0;
}
