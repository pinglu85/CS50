from sys import argv
import sqlite3


def main():
    if len(argv) != 2:
        print('Usage: python roster.py house')
        return

    # Connect database
    conn = sqlite3.connect('students.db')
    c = conn.cursor()

    # Query database and print out students
    # for a given house
    produce_rosters(c, argv[1])

    # Save changes
    conn.commit()

    # Close connection
    conn.close()


# Query the students table and print out
# each student's full name and birth year.
def produce_rosters(cur, house):
    query = '''
    --begin-sql
    SELECT
        first,
        middle,
        last,
        birth
    FROM
        students
    WHERE
        house = ?
    ORDER BY
        last,
        first;
    '''
    house = (house,)

    for row in cur.execute(query, house):
        name = f'{row[0]} {row[1]} {row[2]}' if row[1] != None else f'{row[0]} {row[2]}'
        birth = row[3]
        print(f'{name}, born {birth}')


if __name__ == '__main__':
    main()
