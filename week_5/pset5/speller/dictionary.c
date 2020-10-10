// Implements a dictionary's functionality

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

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

// Number of words in dictionary
unsigned int numOfWordsInDict = 0;

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
    int dict = open(dictionary, O_RDONLY);
    if (dict == -1)
    {
        return false;
    }

    // Prepare for getting dictionary file info
    struct stat dictInfo = {0};
    // Error getting the file size
    if (fstat(dict, &dictInfo) == -1)
    {
        return false;
    }

    // Load the whole dictionary into memory using memory mapping.
    char *map = mmap(0, dictInfo.st_size, PROT_READ, MAP_SHARED, dict, 0);
    // Error mmapping the file
    if (map == MAP_FAILED)
    {
        close(dict);
        return false;
    }
    // Close the dictionary file.
    close(dict);

    char word[LENGTH + 1];
    int wordIndex = 0;

    // Read each word in the dictionary and
    // store them into the hash table.
    for (off_t i = 0; i < dictInfo.st_size; i++)
    {
        // We must found a word.
        if (map[i] == '\n')
        {
            // Terminate current word.
            word[wordIndex] = '\0';
            // Hash current word.
            unsigned int hashValue = hash(word);

            // Create a new node.
            node *n = malloc(sizeof(node));
            if (n == NULL)
            {
                return false;
            }

            // Copy word into the node's word.
            strcpy(n->word, word);

            // Insert current node to the front of the linked list
            // at the corresponding index in the hash table.
            n->next = table[hashValue];

            // Store the node to the hash table.
            table[hashValue] = n;

            // Increase number of words in dictionary by 1.
            numOfWordsInDict++;
            // Prepare for new word.
            wordIndex = 0;
        }
        else
        {
            // Append character to word.
            word[wordIndex] = map[i];
            // Increase wordIndex by 1.
            wordIndex++;
        }
    }

    int err = munmap(map, dictInfo.st_size);
    // UnMapping failed.
    if (err != 0)
    {
        return false;
    }

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return numOfWordsInDict;
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
