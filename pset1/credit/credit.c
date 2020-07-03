#include <stdio.h>
#include <cs50.h>

void validate(long num);

int main(void)
{
  // Prompt user for a credit card number
  long creditCardNum = get_long("Enter a credit card number: ");
  validate(creditCardNum);
}

int luhnAlgorithm(long num);

void validate(long num)
{
  string creditCard;

  // Check if it is an Ameriacan Express card number
  if (num / 10000000000000 == 34 || num / 10000000000000 == 37)
  {
    creditCard = "AMEX";
  }
  // Check if it is a Mastercard card number
  else if (num / 100000000000000 == 51 ||
           num / 100000000000000 == 52 ||
           num / 100000000000000 == 53 ||
           num / 100000000000000 == 54 ||
           num / 100000000000000 == 55)
  {
    creditCard = "MASTERCARD";
  }
  // Check if it is a Visa card number
  else if (num / 1000000000000 == 4 || num / 1000000000000000 == 4)
  {
    creditCard = "VISA";
  }
  // If the card number is neither of them, print "INVALID\n" and return
  else
  {
    printf("INVALID\n");
    return;
  }

  // Check the validity of the credit card number
  if (luhnAlgorithm(num) == 1)
  {
    printf("%s\n", creditCard);
  }
  else
  {
    printf("INVALID\n");
  }
}

int luhnAlgorithm(long num)
{
  int sum = 0;
  int position = 1;

  // Implement luhn's algorithm
  while (num != 0)
  {
    // Get the last digit
    int digit = num % 10;
    // Check if the position of the digit is odd
    if (position % 2 != 0)
    {
      // Add digit to the sum
      sum += digit;
    }
    else
    {
      // When the position of the digit is even, digit = digit * 2
      digit *= 2;
      // Check if digit * 2 is larger than or equal to 10,
      if (digit >= 10)
      {
        // Add its digit to the sum respectively
        sum = sum + digit % 10;
        digit /= 10;
        sum += digit;
      }
      else
      {
        sum += digit;
      }
    }
    // Remove the last digit from the card number
    num /= 10;
    // Increment the digit postion
    position++;
  }
  // If the last digit of the final sum is 0, return 1 else return 0
  return sum % 10 == 0 ? 1 : 0;
}