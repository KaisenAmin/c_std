#include "span/span.h"
#include "fmt/fmt.h"

int main() {
    int array[] = {10, 20, 30, 40, 50};
    Span* span = span_create(array, 5, sizeof(int));

    for (int* ptr = span_begin(span); ptr != span_end(span); ptr = span_increment(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    for (int* ptr = span_rbegin(span); ptr != span_rend(span); ptr = (int*)span_decrement(span, ptr)) {
        if (ptr) {
            fmt_printf("%d ", *ptr);
        }
    }
    fmt_printf("\n");

    span_destroy(span);
    return 0;
}