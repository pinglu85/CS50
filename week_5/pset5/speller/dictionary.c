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
}
node;

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

    // CHeck if the word is a common one.
    char *commonWords[] = {"a", "the", "at", "my", "and", "to", "from", "each", "do"};
    int numberOfCommonWords = 9;
    for (int i = 0; i < 9; i++)
    {
        if (strcmp(lowerCased, commonWords[i]) == 0)
        {
            return true;
        }
    }

    // Hash the word.
    unsigned int hashValue = hash(lowerCased);
    // Calculate the hash table index of the word.
    unsigned int hashTableIndex = hashValue % N;

    // Compare the word with the one in hash table, if matched return true.
    for (node *n = table[hashTableIndex]; n != NULL; n = n->next)
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
    return hashVal;
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

    char wordInDict[LENGTH + 1];
    int wordIndex = 0;

    // Get each word in the dictionary.
    for (int c = fgetc(dict); c != EOF; c = fgetc(dict))
    {
        // If current character is alphabetical or apostroph,
        // append it to the wordInDict.
        if (isalpha(c) || c == '\'')
        {
            wordInDict[wordIndex] = c;
            wordIndex++;
        }
        // We must have found a word.
        else if (wordIndex > 0)
        {
            // Terminate current word.
            wordInDict[wordIndex] = '\0';

            // Hash the word.
            unsigned int hashValue = hash(wordInDict);
            // Calculate the hash table index of the word.
            unsigned int hashTableIndex = hashValue % N;

            // Create a new node and copy the word into the node.
            node *n = malloc(sizeof(node));
            strcpy(n->word, wordInDict);

            // Check hash collision. If there is a collision,
            if (table[hashTableIndex] != NULL)
            {
                // ...insert current node to the front of the linked list.
                n->next = table[hashTableIndex];
            }
            // Otherwise set node's next pointer to NULL.
            else
            {
                n->next = NULL;
            }

            // Store the node to the hash table.
            table[hashTableIndex] = n;

            // Prepare for next word
            wordIndex = 0;
        }

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
