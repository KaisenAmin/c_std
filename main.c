#include "fmt/fmt.h"

int main() {
    fmt_fprintf(stdout, "Emoji example: %s %s %s\n", "😀", "🚀", "🌟");
    fmt_fprint(stdout, "More emojis: ", "🌍", "🌈", "🔥", FMT_END_ARGS);
    
    return 0;
}