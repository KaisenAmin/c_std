/**
 * @author Joey DeFrancesco
 * @date 10/26/2021
 * @class trie
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "trie.h"

// Create new Trie. Returns new Trie on success
// and NULL on failure.
Trie* trie_new(void)
{
    Trie* t = malloc(sizeof(Trie));
    if (t == NULL)
        return NULL;

    // Number of entries in trie 0.
    t->word_count = 1;
    t->trie_root = calloc(1, sizeof(TrieNode));
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        t->trie_root->children[i] = NULL;
    }
    t->trie_root->is_leaf = true;
    t->trie_root->data = '\0';

    return t;
}

bool trie_insert(Trie* t, const char* str)
{
    if (str == NULL)
        return false;

    if (!strlen(str))
        return false;

    // Make sure we have a valid trie constructed.
    if (t == NULL)
        return false;

    printf("Inserting %s\n", str);
    TrieNode* curr = t->trie_root;
    while (*str != '\0') {
        // Path doesn't exist, make new node
        if (curr->children[*str - 'a'] == NULL) {
            curr->children[*str - 'a'] = new_trie_node(*str);
        }
        // Go to next node and next character.
        curr = curr->children[*str - 'a'];
        str++;

    }
    // Mark current node as leaf
    curr->is_leaf = true;
    t->word_count++;

    return true;
}


bool trie_search(Trie* t, const char* str)
{
    if (t->trie_root == NULL)
        return false;

    TrieNode* curr = t->trie_root;
    while (*str) {
        curr = curr->children[*str - 'a'];
        if (curr == NULL)
            return false;
        str++;
    }
    // If current node is a leaf, we reached end of string, return 1;
    return curr->is_leaf;
}

// How many words added to Trie.
size_t trie_count(Trie* t)
{
    return t->word_count;
}


/* void _trie_free(TrieNode* V */
/* { */
/*     if (n == NULL) */
/*         return; */

/*     for (size_t i = 0; i < ALPHA_SIZE; i++) { */
/*         if (t->children[i] != NULL) { */
/*             _trie_free(t->children[i]); */
/*         } */
/*     } */
/*     free(t); */
/* } */

// Remove entry for trie. Returns true on success.
bool trie_node_remove(TrieNode **curr, const char *str)
{

    if (*curr == NULL)
        return false;

    if (*str) {
        // recurse for node corresponding to the next children in the string.
        if (*curr != NULL && (*curr)->children[*str - 'a'] != NULL &&
            trie_node_remove(&((*curr)->children[*str - 'a']), str + 1) &&
            (*curr)->is_leaf == false)
        {
            if (!has_children(*curr)) {
                free(*curr);
                (*curr) = NULL;
                return true;
            } else {
                return false;
            }
        }
    }

    // If we reach end of string
    if (*str == '\0' && (*curr)->is_leaf) {
        if (!has_children(*curr)) {
            free(*curr);
            (*curr) = NULL;
            return true;
        }
        else {
            // Mark current node as non-leaf. DONT delete
            (*curr)->is_leaf = false;
        }
    }
    return false;
}


TrieNode * new_trie_node(char c)
{

    TrieNode *n = calloc(1, sizeof(TrieNode));
    if (n == NULL)
        return NULL;

    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        n->children[i] = NULL;
    }

    n->is_leaf = false;
    n->data = c;
    return n;
}

bool has_children(TrieNode* t)
{
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        if (t->children[i] != NULL)
            return true;
    }
    return false;
}

void print_trie(Trie *t)
{
    printf("Printing trie\n");
    if (t == NULL) {
        printf("trie is null\n");
        return;
    }

    if (t->trie_root == NULL) {
        printf("trie root is null\n");
        return;
    }

    trie_node_print(t->trie_root);
}

void trie_node_print(TrieNode *t)
{
    if (t == NULL) {
        printf("trie node is null\n");
        return;
    }
    TrieNode *tmp = t;
    printf("%c -> ", tmp->data);
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        if (tmp->children[i] != NULL) {
            trie_node_print(t->children[i]);
        }
    }
}

