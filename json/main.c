#include "json.h"

int main(int argc, char **argv)
{
    void *j = json_read_from_file(argv[1]);
    json_print(j, 4);
    json_deallocate(j);
}
