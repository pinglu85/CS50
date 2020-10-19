from sys import argv
import csv
import sqlite3


def main():
    # Ensure proper usage
    if len(argv) != 2:
        print('Usage: python import.py data.csv')
        return

    # Connect database
    conn = sqlite3.connect('students.db')
    c = conn.cursor()

    # Import student data into database
    import_data(c, argv[1])

    # Save changes
    conn.commit()

    # Close the connection
    conn.close()


# Import data into database
def import_data(cur, data_file):
    with open(data_file, 'r') as characters:
        # Create DictReader
        reader = csv.DictReader(characters)

        # Iterate over CSV file
        for row in reader:
            # Prepare for inserting name into database
            name_list = row['name'].split()
            name_list_len = len(name_list)
            first_name = name_list[0]
            middle_name = name_list[1] if name_list_len == 3 else None
            last_name = name_list[2] if name_list_len == 3 else name_list[1]

            birth = int(row['birth'])
            house = row['house']

            # Insert student into students table
            insert = ''' 
            --begin-sql
            INSERT INTO
                students (first, middle, last, house, birth)
            VALUES
                (?, ?, ?, ?, ?);
            '''
            values = (first_name, middle_name, last_name, house, birth)
            cur.execute(insert, values)


if __name__ == '__main__':
    main()
