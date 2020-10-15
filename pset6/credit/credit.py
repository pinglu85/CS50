from cs50 import get_int


def main():
    # Prompt user for a credit card number
    num = get_int('Enter a credit card number: ')
    validate(num)


def validate(num):
    credit_card = ''

    # Check if it is an American Express card number
    if num // 10000000000000 == 34 or num // 10000000000000 == 37:
        credit_card = 'AMEX'
    # Check if it is a Mastercard card number
    elif (num // 100000000000000 == 51 or
          num // 100000000000000 == 52 or
          num // 100000000000000 == 53 or
          num // 100000000000000 == 54 or
          num // 100000000000000 == 55):
        credit_card = 'MASTERCARD'
    # Check if it is a Visa card number
    elif num // 1000000000000 == 4 or num // 1000000000000000 == 4:
        credit_card = 'VISA'
    else:
        print('INVALID')
        return

    # Luhn’s algorithm:
    # check the validity of the credit card number
    if luhn_algorithm(num):
        print(f'{credit_card}')
    else:
        print('INVALID')


def luhn_algorithm(n):
    sum = 0
    position = 1

    # Implement Luhn's algorithm
    while n != 0:
        # Get the last digit
        digit = n % 10
        # If the position of the digit is odd
        if position % 2 != 0:
            # ...add digit to sum
            sum += digit
        else:
            # When the position of the digit is even, digit = digit * 2
            digit *= 2
            # If digit * 2 is greater than or equal to 10,
            if digit >= 10:
                # ...add its digit to sum respectively
                sum += digit % 10
                digit = digit // 10
                sum += digit
            # Otherwise add digit to sum
            else:
                sum += digit
        # Remove the last digit from the card number
        n = n // 10
        # Increase the digit position
        position += 1
    # If the last digit of the final sum is 0, return true else false.
    return sum % 10 == 0


if __name__ == '__main__':
    main()
