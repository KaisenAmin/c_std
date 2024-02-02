# CLi Library in C

**Author:** amin tahmasebi
**Release Date:** 2024
**License:** ISC License



## Example 1 : Create a parser the deallocate parser and resource with `cli_parser_create` and `cli_parser_deallocate`

```c
#include "cli/cli.h"
#include "fmt/fmt.h"

int main(int argc, char *argv[]) {
    CliParser *parser = cli_parser_create("MyApp");
    
    if (parser == NULL) {
        fmt_fprintf(stderr, "Failed to create CLI parser\n");
        return -1;
    }


    // Deallocate the CLI parser and all associated resources
    cli_parser_deallocate(parser);
    return 0;
}
```