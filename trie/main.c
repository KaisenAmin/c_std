#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include "trie.h"

int main(void)
{

    Trie* t = trie_new();
    if (t == NULL) {
        printf("Failed to create trie\n");
        return 1;
    }
    printf("Trie created\n");
    printf("Trie word count: %zu\n", t->word_count);




    return 0;
}
