#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <math.h>

int readability(string str);

int main(void)
{
    // Prompt user for text
    string text = get_string("Text: ");
    int indexNum = readability(text);

    // Print out different reading level based on indexNum
    if (indexNum < 1)
    {
        printf("Before Grade 1\n");
    }
    else if (indexNum > 16)
    {
        printf("Grade 16+\n");
    }
    else
    {
        printf("Grade %d\n", indexNum);
    }

}

// Compute the reading level
int readability(string str)
{
    int letterCount = 0;
    int wordCount = 0;
    int sentenceCount = 0;

    int strLen = strlen(str);

    // Loop through the string
    for (int i = 0; i < strLen; i++)
    {
        // if str[i] is an alphabetic character, increase letterCount by 1
        if ((str[i] >= 'a' && str[i] <= 'z')
            || (str[i] >= 'A' && str[i] <= 'Z'))
        {
            letterCount++;
        }
        // if str[i] is space, increase letterCount by 1
        else if (str[i] == ' ')
        {
            wordCount++;
        }
        // if str[i] is period or exclamation or question mark,
        // increase sentenceCount by 1.
        else if (str[i] == '.' || str[i] == '!' || str[i] == '?')
        {
            sentenceCount++;
        }
    }

    // Because we use space to count words, we need to add 1 to wordCount
    wordCount++;

    // Calculate letters per 100 words
    float letterPerWord = (float)letterCount / (float)wordCount * 100.0;
    // Round letterPerWord to 2 decimal places
    letterPerWord = roundf(letterPerWord * 100.0) / 100.0;

    // Calculate sentences per 100 words
    float sentencePerWord = (float)sentenceCount / (float)wordCount * 100.0;
    // Round sentencePerWOrd to 2 decimal places
    sentencePerWord = roundf(sentencePerWord * 100.0) / 100.0;

    // Coleman-Liau formula: index = 0.0588 * L - 0.296 * S - 15.8
    float index = 0.0588 * letterPerWord - 0.296 * sentencePerWord - 15.8;

    // Round index to integer and return it
    return (int)round(index);
}