#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"

int main() {
    random_seed((unsigned int)time_current_time_in_seconds());

    for (int i = 0; i < 10; i++) {
        double random_value = random_gauss(0.0, 1.0);
        fmt_printf("Random Gaussian value: %f\n", random_value);
    }

    return 0;
}
