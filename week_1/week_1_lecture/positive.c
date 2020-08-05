#include <cs50.h>
#include <stdio.h>

int get_positive_integer(void);

int main(void) 
{
    int n = get_positive_integer();
    printf("Positive integer: %i\n", n);    
}

int get_positive_integer(void) 
{
    int n;
    do 
    {
        n = get_int("Integer: ");
    }
    while (n < 1);
    return n;    
}
