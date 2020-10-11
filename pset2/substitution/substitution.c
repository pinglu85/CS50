#include <stdio.h>
#include <cs50.h>
#include <string.h>
#include <ctype.h>

// Create a lookup array for substitution.
char lookup[25];

int validateKey(string str);
string cipherString(string str);

int main(int argc, string argv[])
{
    // If no command-line argument or
    // more than one argument is provided,
    // print error message and return 1.
    if (argc != 2)
    {
        printf("Please enter the key.\n");
        return 1;
    }

    // Otherwise set key to argv[1].
    string key = argv[1];

    // Validate key. If key is invalid key,
    // print error message and return 1.
    if (validateKey(key) == 0)
    {
        printf("Key must contain 26 alphabetical characters. Each letter can be used only once.\n");
        return 1;
    }

    // If key is valid, prompt user for text.
    string plainText = get_string("plaintext: ");

    // Cipher the text and print out the result
    string cipherText = cipherString(plainText);
    printf("ciphertext: %s\n", cipherText);

    // Return 0
    return 0;
}

// Validate the key
int validateKey(string str)
{
    int strLen = strlen(str);
    // Check if the length of str is equal to 26;
    // if not return 0
    if (strLen != 26)
    {
        return 0;
    }

    // Loop through the str
    for (int i = 0; i < strLen; i++)
    {
        // If char is alphabetic character
        if ((str[i] >= 'A' && str[i] <= 'Z')
            || (str[i] >= 'a' && str[i] <= 'z'))
        {
            // Lowercase char.
            char c = tolower(str[i]);

            // Check if char is duplicated.
            // For j from 0 to i
            for (int j = 0; j < i; j++)
            {
                // If the char is already in lookup array
                // Return 0
                if (lookup[j] == c)
                {
                    return 0;
                }
            }

            // If char is not duplicated, store it in the lookup array
            lookup[i] = c;

        }
        // Otherwise return 0
        else
        {
            return 0;
        }


    }
    // Return 1
    return 1;
}

// Cipher the string
string cipherString(string str)
{

    int strLen = strlen(str);

    // Loop through str
    for (int i = 0; i < strLen; i++)
    {
        // Set keyIndex to 0.
        int keyIndex = 0;
        // If char is alphabetic and uppercased,
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            // Lowercase the char.
            char c = tolower(str[i]);
            // Turn the lowercased char into ascii code, and subtract it by 97
            // so that we can find the corresponding index in the lookup array.
            keyIndex = (int)c - 97;
            // Uppercase the substitution and set current character to the result
            str[i] = toupper(lookup[keyIndex]);
        }
        // Otherwise if char is alphabetic and lowercased
        else if (str[i] >= 'a' && str[i] <= 'z')
        {
            // Turn the lowercased char into ascii code, and subtract it by 97
            // so that we can find the corresponding index in the lookup array.
            keyIndex = (int)str[i] - 97;
            // Set current character to the result
            str[i] = lookup[keyIndex];
        }
    }

    // Return str
    return str;

}