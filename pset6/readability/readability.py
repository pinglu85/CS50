from cs50 import get_string


def main():
    # Prompt user for text
    text = get_string('Text: ')
    # Get Coleman-Liau index of the text
    coleman_liau_index = readability(text)

    # Print out different reading level based on coleman_liau_index
    if coleman_liau_index < 1:
        print('Before Grade 1')
    elif coleman_liau_index > 16:
        print('Grade 16+')
    else:
        print(f'Grade {coleman_liau_index}')


# Compute Coleman-Liau index
def readability(str):
    letter_count = 0
    word_count = 0
    sentence_count = 0

    # Loop througth the string
    for c in str:
        # If c is alphabetic,
        if c.isalpha():
            # ...increase letter_count by 1.
            letter_count += 1
        # If c is space,
        elif c == ' ':
            # ...increase word_count by 1.
            word_count += 1
        # If we encounter period or exclamation or question mark,
        elif c == '.' or c == '!' or c == '?':
            # ...increase sentence_count by 1.
            sentence_count += 1

    # Because we use space to count words,
    # we need to count in the last word.
    word_count += 1

    # Calculate letters per 100 words
    letter_per_word = (letter_count / word_count) * 100
    # Round letter_per_word to 2 decimal places
    letter_per_word = round(letter_per_word * 100) / 100

    # Calculate sentences per 100 words
    sentence_per_word = (sentence_count / word_count) * 100
    # Round sentence_per_word to 2 decimal places
    sentence_per_word = round(sentence_per_word * 100) / 100

    # Calulate Coleman-Liau index
    coleman_liau_index = 0.0588 * letter_per_word - 0.296 * sentence_per_word - 15.8

    return round(coleman_liau_index)


if __name__ == '__main__':
    main()