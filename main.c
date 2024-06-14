#include "random/random.h"
#include "fmt/fmt.h"
#include "time/time.h"
#include <math.h>

int main() {
    random_seed((unsigned int) time_current_time_in_seconds());

    for (int i = 0; i < 50; i++) {
        double random_value = random_vonmises(0.0, 1.0);

        if (!isnan(random_value))
            fmt_printf("Random von Mises value: %f\n", random_value);
    }

    return 0;
}