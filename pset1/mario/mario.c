#include <stdio.h>
#include <cs50.h>

void draw(int h);

int main(void)
{
    int height = get_int("Height: ");

    // if input is not between 1 and 8, re-prompt the user
    while (height < 1 || height > 8)
    {
        height = get_int("Height: ");
    }

    draw(height);
}

void draw(int h)
{
    for (int i = 1; i <= h; i++)
    {
        int spaceCount = h - i;

        // draw leading space
        for (int s = 1; s <= spaceCount; s++)
        {
            printf(" ");
        }
        // draw the first half of the bricks
        for (int c = 1; c <= i; c++)
        {
            printf("#");
        }
        // draw the gap
        printf("  ");
        // draw the second half of the bricks
        for (int c = 1; c <= i; c++)
        {
            printf("#");
        }
        // new line
        printf("\n");
    }
}