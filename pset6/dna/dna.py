from sys import argv
import csv
import re


def main():
    # Ensure proper usage
    if len(argv) != 3:
        print('Usage: python dna.py data.csv sequence.txt')
        return

    # Remember data.csv file name
    data_file = argv[1]
    # Remember sequence.txt file name
    sequence_file = argv[2]

    # Get the STR counts
    STR_counts = count_STRs(data_file, sequence_file)
    # Identify the individual
    result = identify(STR_counts, data_file)
    print(result)


# Compute the STRs counts
def count_STRs(data_file, sequence_file):
    # Dict for storing the STR counts in the DNA sequence
    STR_counts = {}

    # Get all STRs from data.csv
    with open(data_file, 'r') as data_file:
        reader = csv.reader(data_file)
        header = next(reader)
        # ...add each STR as key and 0 as value to dict.
        for field_name in header:
            if field_name != 'name':
                STR_counts[field_name] = 0

    # Read sequence file into string
    with open(sequence_file, 'r') as sequence_file:
        sequence = sequence_file.read()

    # Iterate through STRs in dict
    for key in STR_counts:
        # Find all consecutive repeats of current STR in DNA sequence
        regex = re.compile('({})+'.format(key))
        iter = re.finditer(regex, sequence)

        longest_len = 0
        # Iterate through the result to get the longest length
        for result in iter:
            sequence_len = len(result.group())
            longest_len = sequence_len if sequence_len > longest_len else longest_len
        # Calculate number of STR repeats in the longest sequence
        # and update the count of current STR in dict
        STR_counts[key] = longest_len // len(key)

    # Return dict
    return STR_counts


def identify(STR_counts, data_file):
    # Read data file row by row
    with open(data_file, 'r') as data_file:
        reader = csv.DictReader(data_file)
        for row in reader:
            matched = True
            # Compare each STR's count in dict with the one in current row.
            for key in STR_counts:
                # ...update matched
                matched = matched and int(row[key]) == STR_counts[key]
            # Return row['name'] if all STR counts matched.
            if matched:
                return row['name']

        # Return 'No match'.
        return 'No match'


if __name__ == '__main__':
    main()
