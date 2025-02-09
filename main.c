#include "bigfloat/bigfloat.h"
#include "fmt/fmt.h"

int main() {
    BigFloat* angle = bigfloat_from_string("12345678901234567890.87654321"); 

    BigFloat* sin_value = bigfloat_trig_sin(angle);
    BigFloat* cos_value = bigfloat_trig_cos(angle);
    BigFloat* tan_value = bigfloat_trig_tan(angle);

    fmt_printf("Sin(angle): %s\n", bigfloat_to_string(sin_value));
    fmt_printf("Cos(angle): %s\n", bigfloat_to_string(cos_value));
    fmt_printf("Tan(angle): %s\n", bigfloat_to_string(tan_value));

    bigfloat_deallocate(angle);
    bigfloat_deallocate(sin_value);
    bigfloat_deallocate(cos_value);
    bigfloat_deallocate(tan_value);

    return 0;
}