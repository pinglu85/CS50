#include <stdio.h>

// Function prototype. 
// In C, functions need to be declared before using.
// Use Function prototype to declare functions after using.
void cough(void);

int main(void) 
{
    for (int i = 0; i < 3; i++) 
    {
        cough();    
    }    
}

void cough(void) 
{
    printf("cough\n");
}
