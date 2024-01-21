#include "fmt/fmt.h"

int main() {
    fmt_printf("World : %s %s\n", "🌍", "😡");
    fmt_println("Haaaa :", "😊🥴", FMT_END_ARGS);
    fmt_print("😡", "🥶", "😎", "🤩", FMT_END_ARGS);

    return 0;
}