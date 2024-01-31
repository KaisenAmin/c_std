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

    if(!trie_insert(t, "hello"))
        printf("Failed to insert word\n");
    if(!trie_insert(t, "helish"))
        printf("Failed to insert word\n");
    if(!trie_insert(t, "helishness"))
        printf("Failed to insert word\n");
    if(!trie_insert(t, "can"))
        printf("Failed to insert word\n");
    if(!trie_insert(t, "canada"))
        printf("Failed to insert word\n");


    print_trie(t);


    return 0;
}
