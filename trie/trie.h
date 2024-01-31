#ifndef TRIE_H_
#define TRIE_H_

#include <stdbool.h>

// Size of English alphabet, note we assume only lowercase
#define ALPHA_SIZE 26

// TrieNode represents a single node in the trie.
typedef struct TrieNode {
    struct TrieNode *children[ALPHA_SIZE];
    bool is_leaf;
} TrieNode;

// Trie data structure, that stores a set of strings.
// as a prefix tree.
typedef struct Trie {
    struct TrieNode *trie_root;;
    size_t word_count;
} Trie;

// Get new trie node
Trie * trie_new(void);
// Insert into trie
bool trie_insert(Trie* t, const char* str);
// Search for string.
bool trie_search(Trie* t, const char* str);
// Remove entry from trie.
bool trie_node_remove(TrieNode **t, const char* str);
// Return true if node has any children
bool has_children(TrieNode* t);
// Cleanup and free trie.
void trie_free(Trie* t);
// Holds number of words that have been inserted into trie.
size_t trie_count(Trie* t);


// Create new node and allocate memory for it.
TrieNode * new_trie_node(void);

#endif  // TRIE_H_
