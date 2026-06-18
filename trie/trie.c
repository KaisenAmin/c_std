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
    t->word_count = 0;
    t->trie_root = calloc(1, sizeof(TrieNode));
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        t->trie_root->children[i] = NULL;
    }
    t->trie_root->is_word = true;
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


    if (!trie_node_insert(t->trie_root, str)) {
        printf("Failed to insert word\n");
        return false;
    }

    t->word_count++;
    return true;

}

bool trie_node_insert(TrieNode* t, const char* str)
{
    TrieNode* curr = t;
    while (*str != '\0') {
        // Path doesn't exist, make new node
        if (curr->children[*str - 'a'] == NULL) {
            curr->children[*str - 'a'] = new_trie_node(*str);
        }
        // Go to next node and next character.
        curr = curr->children[*str - 'a'];
        str++;

    }
    // Mark current node word.
    curr->is_word = true;

    return true;
}


bool trie_search(const Trie* t, const char* str)
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
    return (curr->is_word && curr != NULL);
}

// How many words added to Trie.
size_t trie_count(const Trie* t)
{
    return t->word_count;
}

static void trie_node_free(TrieNode* t)
{
    if (t == NULL)
        return;

    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        if (t->children[i] != NULL) {
            trie_node_free(t->children[i]);
        }
    }
    free(t);
}

void trie_free(Trie* t)
{
    if (t == NULL)
        return;

    trie_node_free(t->trie_root);
    free(t);
}

// Remove entry for trie. Returns true on success.
bool trie_node_remove(TrieNode **curr, const char *str)
{

    if (*curr == NULL)
        return false;

    if (*str) {
        // recurse for node corresponding to the next children in the string.
        if (*curr != NULL && (*curr)->children[*str - 'a'] != NULL &&
            trie_node_remove(&((*curr)->children[*str - 'a']), str + 1) &&
            (*curr)->is_word == false)
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
    if (*str == '\0' && (*curr)->is_word) {
        if (!has_children(*curr)) {
            free(*curr);
            (*curr) = NULL;
            return true;
        }
        else {
            // Mark current node as non-leaf. DONT delete
            (*curr)->is_word = false;
        }
    }
    return false;
}


TrieNode * new_trie_node(const char c)
{

    TrieNode *n = calloc(1, sizeof(TrieNode));
    if (n == NULL)
        return NULL;

    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        n->children[i] = NULL;
    }

    n->is_word = false;
    n->data = c;
    return n;
}


bool has_children(const TrieNode* t)
{
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        if (t->children[i] != NULL)
            return true;
    }
    return false;
}


static void trie_node_print(const TrieNode *t)
{
    if (t == NULL) {
        printf("trie node is null\n");
        return;
    }
    const TrieNode *tmp = t;
    printf("%c -> ", tmp->data);
    for (size_t i = 0; i < ALPHA_SIZE; i++) {
        if (tmp->children[i] != NULL) {
            trie_node_print(t->children[i]);
        }
    }
}


void trie_print(const Trie *t)
{
    printf("Printing trie\n");
    if (t == NULL) {
        printf("trie is null\n");
        return;
    }

    if (t->trie_root == NULL) {
        printf("Trie empty\n");
        return;
    }

    printf("Trie word count: %zu\n", t->word_count);
    trie_node_print(t->trie_root);
    printf("\n");
}

