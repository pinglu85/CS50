// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Number of buckets in hash table
const unsigned int N = 143091;

// Hash table
node *table[N];

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    char lowerCased[LENGTH + 1];
    int wordIndex = 0;

    // Lowercase all letters in word.
    for (const char *c = word; *c != '\0'; c++)
    {
        lowerCased[wordIndex] = tolower(*c);
        wordIndex++;
    }
    // Terminate lowercased word.
    lowerCased[wordIndex] = '\0';

    // Hash the word.
    unsigned int hashValue = hash(lowerCased);

    // Compare the word with the one in hash table, if matched return true.
    for (node *n = table[hashValue]; n != NULL; n = n->next)
    {
        if (strcmp(lowerCased, n->word) == 0)
        {
            return true;
        }
    }

    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // Implement FNV-1a. FNV Hash: http://isthe.com/chongo/tech/comp/fnv/
    const unsigned int OFFSET_BASIS = 2166136261;
    const unsigned int FNV_PRIME = 16777619;

    unsigned int hashVal = OFFSET_BASIS;
    for (const char *c = word; *c != '\0'; c++)
    {
        hashVal = hashVal ^ *c;
        hashVal *= FNV_PRIME;
    }
    // Calculate the index in the hash table.
    return hashVal % N;
}

// Loads dictionary into memory, returning true if successful else false
bool load(const char *dictionary)
{
    // Open the dictionary file.
    FILE *dict = fopen(dictionary, "r");
    if (dict == NULL)
    {
        return false;
    }

    // Read each word in the dictionary and
    // store them into the hash table.
    while (1)
    {
        // Create a new node and copy the word into the node.
        node *n = malloc(sizeof(node));
        if (n == NULL)
        {
            return false;
        }

        // Read a word and store it into the node's word.
        fscanf(dict, "%s", n->word);

        // If reach end of the file, free last node
        // and stop the while loop.
        if (feof(dict))
        {
            free(n);
            break;
        }

        // Hash the word.
        unsigned int hashValue = hash(n->word);

        // Insert current node to the front of the linked list
        // at the corresponding index in the hash table.
        n->next = table[hashValue];

        // Store the node to the hash table.
        table[hashValue] = n;
    }

    // Close the dictionary file.
    fclose(dict);

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    unsigned int numberOfWords = 0;

    // Loop through the hash table.
    for (int i = 0; i < N; i++)
    {
        node *n = table[i];

        // If current ith pointer in the table points to a linked list,
        // which means there is at least one word stored at current index,
        // traverse the linked list until we reach the end of the list.
        while (n != NULL)
        {
            // Increase number of words by 1.
            numberOfWords++;
            n = n->next;
        }
    }
    return numberOfWords;
}

// Unloads dictionary from memory, returning true if successful else false
bool unload(void)
{
    // Free the linked list in the hash table
    for (int i = 0; i < N; i++)
    {
        node *n = table[i];
        while (n != NULL)
        {
            // Create a temporary pointer pointing to
            // the next node.
            node *tmp = n->next;
            // Free current node.
            free(n);
            // Point n to the rest of the list.
            n = tmp;
        }
    }
    return true;
}
