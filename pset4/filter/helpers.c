#include "helpers.h"

#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    // Loop through every pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Get R, G, B values of jth's pixel
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;
            // Calculate the average of the R, G, B values
            // and update the R, G, B values of the pixel.
            int grey = round((r + g + b) / 3.0);
            image[i][j].rgbtRed = grey;
            image[i][j].rgbtGreen = grey;
            image[i][j].rgbtBlue = grey;
        }
    }
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    // Calculate the middle point of x axis (width) of the image
    int midX = (int)width / 2;
    // Loop through y axis (height) of the image
    for (int i = 0; i < height; i++)
    {
        // Loop through x axis (width) from 0 to the mid point
        for (int j = 0; j < midX; j++)
        {
            // Get R, G, B values of jth's pixel
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;
            // Get jth's opposite pixel
            int _j = width - 1 - j;
            // Get R, G, B values of the opposite pixel
            int _r = image[i][_j].rgbtRed;
            int _g = image[i][_j].rgbtGreen;
            int _b = image[i][_j].rgbtBlue;
            // Swap the R, G, B values between the two pixels
            image[i][j].rgbtRed = _r;
            image[i][j].rgbtGreen = _g;
            image[i][j].rgbtBlue = _b;
            image[i][_j].rgbtRed = r;
            image[i][_j].rgbtGreen = g;
            image[i][_j].rgbtBlue = b;
        }
    }
}

// Handle edge and corner
void handleEdgeAndCorner(int i, int j, int height, int width, RGBTRIPLE *upperLeftPixel, RGBTRIPLE *upperPixel,
                         RGBTRIPLE *upperRightPixel, RGBTRIPLE *leftPixel, RGBTRIPLE *rightPixel,
                         RGBTRIPLE *bottomLeftPixel, RGBTRIPLE *bottomPixel, RGBTRIPLE *bottomRightPixel,
                         RGBTRIPLE image[height][width], int *pixelCount);

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Initialize a matrix to store computed pixel
    RGBTRIPLE tempImage[height][width];
    // Loop through every pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Get R, G, B values of jth's pixel
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;

            // Initialize neighbouring pixels
            // and set their color values to 0.
            RGBTRIPLE upperLeftPixel;
            upperLeftPixel.rgbtRed = 0x00;
            upperLeftPixel.rgbtGreen = 0x00;
            upperLeftPixel.rgbtBlue = 0x00;

            RGBTRIPLE upperPixel;
            upperPixel.rgbtRed = 0x00;
            upperPixel.rgbtGreen = 0x00;
            upperPixel.rgbtBlue = 0x00;

            RGBTRIPLE upperRightPixel;
            upperRightPixel.rgbtRed = 0x00;
            upperRightPixel.rgbtGreen = 0x00;
            upperRightPixel.rgbtBlue = 0x00;

            RGBTRIPLE leftPixel;
            leftPixel.rgbtRed = 0x00;
            leftPixel.rgbtGreen = 0x00;
            leftPixel.rgbtBlue = 0x00;

            RGBTRIPLE rightPixel;
            rightPixel.rgbtRed = 0x00;
            rightPixel.rgbtGreen = 0x00;
            rightPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomLeftPixel;
            bottomLeftPixel.rgbtRed = 0x00;
            bottomLeftPixel.rgbtGreen = 0x00;
            bottomLeftPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomPixel;
            bottomPixel.rgbtRed = 0x00;
            bottomPixel.rgbtGreen = 0x00;
            bottomPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomRightPixel;
            bottomRightPixel.rgbtRed = 0x00;
            bottomRightPixel.rgbtGreen = 0x00;
            bottomRightPixel.rgbtBlue = 0x00;

            int pixelCount = 0;

            // Handle edges and corners
            handleEdgeAndCorner(i, j, height, width, &upperLeftPixel, &upperPixel, &upperRightPixel, &leftPixel,
                                &rightPixel, &bottomLeftPixel, &bottomPixel, &bottomRightPixel, image, &pixelCount);

            // Average the color values of neighbouring values plus the current pixel.
            tempImage[i][j].rgbtRed =
                round((upperLeftPixel.rgbtRed + upperPixel.rgbtRed + upperRightPixel.rgbtRed + leftPixel.rgbtRed + r +
                       rightPixel.rgbtRed + bottomLeftPixel.rgbtRed + bottomPixel.rgbtRed + bottomRightPixel.rgbtRed) /
                      (float)pixelCount);
            tempImage[i][j].rgbtGreen =
                round((upperLeftPixel.rgbtGreen + upperPixel.rgbtGreen + upperRightPixel.rgbtGreen +
                       leftPixel.rgbtGreen + g + rightPixel.rgbtGreen + bottomLeftPixel.rgbtGreen +
                       bottomPixel.rgbtGreen + bottomRightPixel.rgbtGreen) /
                      (float)pixelCount);
            tempImage[i][j].rgbtBlue = round((upperLeftPixel.rgbtBlue + upperPixel.rgbtBlue + upperRightPixel.rgbtBlue +
                                              leftPixel.rgbtBlue + b + rightPixel.rgbtBlue + bottomLeftPixel.rgbtBlue +
                                              bottomPixel.rgbtBlue + bottomRightPixel.rgbtBlue) /
                                             (float)pixelCount);
        }
    }

    // Copy each pixel of temporary image back to the original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = tempImage[i][j].rgbtRed;
            image[i][j].rgbtGreen = tempImage[i][j].rgbtGreen;
            image[i][j].rgbtBlue = tempImage[i][j].rgbtBlue;
        }
    }
}

// Compute Gx and Gy and combine them
int sobelOperator(int upperLeftColor, int leftColor, int bottomLeftColor, int upperColor, int curColor, int bottomColor,
                  int upperRightColor, int rightColor, int bottomRightColor);

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Initialize a matrix to store computed pixel
    RGBTRIPLE tempImage[height][width];
    // Loop through every pixel in the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            // Get R, G, B values of jth's pixel
            int r = image[i][j].rgbtRed;
            int g = image[i][j].rgbtGreen;
            int b = image[i][j].rgbtBlue;

            // Initialize neighbouring pixels
            // and set their color values to 0.
            RGBTRIPLE upperLeftPixel;
            upperLeftPixel.rgbtRed = 0x00;
            upperLeftPixel.rgbtGreen = 0x00;
            upperLeftPixel.rgbtBlue = 0x00;

            RGBTRIPLE upperPixel;
            upperPixel.rgbtRed = 0x00;
            upperPixel.rgbtGreen = 0x00;
            upperPixel.rgbtBlue = 0x00;

            RGBTRIPLE upperRightPixel;
            upperRightPixel.rgbtRed = 0x00;
            upperRightPixel.rgbtGreen = 0x00;
            upperRightPixel.rgbtBlue = 0x00;

            RGBTRIPLE leftPixel;
            leftPixel.rgbtRed = 0x00;
            leftPixel.rgbtGreen = 0x00;
            leftPixel.rgbtBlue = 0x00;

            RGBTRIPLE rightPixel;
            rightPixel.rgbtRed = 0x00;
            rightPixel.rgbtGreen = 0x00;
            rightPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomLeftPixel;
            bottomLeftPixel.rgbtRed = 0x00;
            bottomLeftPixel.rgbtGreen = 0x00;
            bottomLeftPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomPixel;
            bottomPixel.rgbtRed = 0x00;
            bottomPixel.rgbtGreen = 0x00;
            bottomPixel.rgbtBlue = 0x00;

            RGBTRIPLE bottomRightPixel;
            bottomRightPixel.rgbtRed = 0x00;
            bottomRightPixel.rgbtGreen = 0x00;
            bottomRightPixel.rgbtBlue = 0x00;

            // Handle edges and corners
            handleEdgeAndCorner(i, j, height, width, &upperLeftPixel, &upperPixel, &upperRightPixel, &leftPixel,
                                &rightPixel, &bottomLeftPixel, &bottomPixel, &bottomRightPixel, image, 0);

            // Compute channel values using sobel operator
            // and store them to the temporary matrix
            tempImage[i][j].rgbtRed = sobelOperator(upperLeftPixel.rgbtRed, leftPixel.rgbtRed, bottomLeftPixel.rgbtRed,
                                                    upperPixel.rgbtRed, r, bottomPixel.rgbtRed, upperRightPixel.rgbtRed,
                                                    rightPixel.rgbtRed, bottomRightPixel.rgbtRed);

            tempImage[i][j].rgbtGreen = sobelOperator(
                upperLeftPixel.rgbtGreen, leftPixel.rgbtGreen, bottomLeftPixel.rgbtGreen, upperPixel.rgbtGreen, g,
                bottomPixel.rgbtGreen, upperRightPixel.rgbtGreen, rightPixel.rgbtGreen, bottomRightPixel.rgbtGreen);

            tempImage[i][j].rgbtBlue = sobelOperator(
                upperLeftPixel.rgbtBlue, leftPixel.rgbtBlue, bottomLeftPixel.rgbtBlue, upperPixel.rgbtBlue, b,
                bottomPixel.rgbtBlue, upperRightPixel.rgbtBlue, rightPixel.rgbtBlue, bottomRightPixel.rgbtBlue);
        }
    }
    // Copy each pixel of temporary image back to the original image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j].rgbtRed = tempImage[i][j].rgbtRed;
            image[i][j].rgbtGreen = tempImage[i][j].rgbtGreen;
            image[i][j].rgbtBlue = tempImage[i][j].rgbtBlue;
        }
    }
}

// Compute Gx and Gy and combine them
int sobelOperator(int upperLeftColor, int leftColor, int bottomLeftColor, int upperColor, int curColor, int bottomColor,
                  int upperRightColor, int rightColor, int bottomRightColor)
{
    int gx = upperLeftColor * -1 + leftColor * -2 + bottomLeftColor * -1 + upperColor * 0 + curColor * 0 +
             bottomColor * 0 + upperRightColor * 1 + rightColor * 2 + bottomRightColor * 1;

    int gy = upperLeftColor * -1 + leftColor * 0 + bottomLeftColor * 1 + upperColor * -2 + curColor * 0 +
             bottomColor * 2 + upperRightColor * -1 + rightColor * 0 + bottomRightColor * 1;

    int result = round(sqrt(pow(gx, 2) + pow(gy, 2)));
    // Cap the result to 255
    return result > 255 ? 255 : result;
}

// Handle edge and corner
void handleEdgeAndCorner(int i, int j, int height, int width, RGBTRIPLE *upperLeftPixel, RGBTRIPLE *upperPixel,
                         RGBTRIPLE *upperRightPixel, RGBTRIPLE *leftPixel, RGBTRIPLE *rightPixel,
                         RGBTRIPLE *bottomLeftPixel, RGBTRIPLE *bottomPixel, RGBTRIPLE *bottomRightPixel,
                         RGBTRIPLE image[height][width], int *pixelCount)
{
    // If current pixel is at top left corner,
    // reassign pixels right, bottom right and bottom to
    // actual one; set pixelCount to 4.
    if (i == 0 && j == 0)
    {
        *rightPixel = image[i][j + 1];
        *bottomRightPixel = image[i + 1][j + 1];
        *bottomPixel = image[i + 1][j];
        if (pixelCount != 0)
        {
            *pixelCount = 4;
        }
    }
    // If current pixel is at top right corner,
    // reassign pixels left, bottom left and bottom to
    // actual one; set pixelCount to 4.
    else if (i == 0 && j == width - 1)
    {
        *leftPixel = image[i][j - 1];
        *bottomLeftPixel = image[i + 1][j - 1];
        *bottomPixel = image[i + 1][j];
        if (pixelCount != 0)
        {
            *pixelCount = 4;
        }
    }
    // If current pixel is at bottom left corner,
    // reassign pixels upper, upper right and right to
    // actual one; set pixelCount to 4.
    else if (i == height - 1 && j == 0)
    {
        *upperPixel = image[i - 1][j];
        *upperRightPixel = image[i - 1][j + 1];
        *rightPixel = image[i][j + 1];
        if (pixelCount != 0)
        {
            *pixelCount = 4;
        }
    }
    // If current pixel is at bottom right corner,
    // reassign pixels upper, upper left and left to
    // actual one; set pixelCount to 4.
    else if (i == height - 1 && j == width - 1)
    {
        *upperPixel = image[i - 1][j];
        *upperLeftPixel = image[i - 1][j - 1];
        *leftPixel = image[i][j - 1];
        if (pixelCount != 0)
        {
            *pixelCount = 4;
        }
    }
    // If current pixel is at top edge and not at corners,
    // reassign pixels left, right, bottom left, bottom and bottom right
    // to actual one; set pixelCount to 6.
    else if (i == 0)
    {
        *leftPixel = image[i][j - 1];
        *rightPixel = image[i][j + 1];
        *bottomLeftPixel = image[i + 1][j - 1];
        *bottomPixel = image[i + 1][j];
        *bottomRightPixel = image[i + 1][j + 1];
        if (pixelCount != 0)
        {
            *pixelCount = 6;
        }
    }
    // If current pixel is at left edge and not at corners,
    // reassign pixels upper, upper right, right, bottom right and bottom
    // to actual one; set pixelCount to 6.
    else if (j == 0)
    {
        *upperPixel = image[i - 1][j];
        *upperRightPixel = image[i - 1][j + 1];
        *rightPixel = image[i][j + 1];
        *bottomRightPixel = image[i + 1][j + 1];
        *bottomPixel = image[i + 1][j];
        if (pixelCount != 0)
        {
            *pixelCount = 6;
        }
    }
    // If current pixel is at right edge and not at corners,
    // reassign pixels upper, upper left, left, bottem left and bottom
    // to actual one; set pixelCount to 6.
    else if (j == width - 1)
    {
        *upperPixel = image[i - 1][j];
        *upperLeftPixel = image[i - 1][j - 1];
        *leftPixel = image[i][j - 1];
        *bottomLeftPixel = image[i + 1][j - 1];
        *bottomPixel = image[i + 1][j];
        if (pixelCount != 0)
        {
            *pixelCount = 6;
        }
    }
    // If current pixel is at bottom edge and not at corners,
    // reassign pixels upper left, upper, upper right, left and right
    // to actual one; set pixelCount to 6.
    else if (i == height - 1)
    {
        *upperLeftPixel = image[i - 1][j - 1];
        *upperPixel = image[i - 1][j];
        *upperRightPixel = image[i - 1][j + 1];
        *leftPixel = image[i][j - 1];
        *rightPixel = image[i][j + 1];
        if (pixelCount != 0)
        {
            *pixelCount = 6;
        }
    }
    // Otherwise reassign all neighbouring pixels to actual one and
    // set pixelCount to 9.
    else
    {
        *upperLeftPixel = image[i - 1][j - 1];
        *upperPixel = image[i - 1][j];
        *upperRightPixel = image[i - 1][j + 1];
        *leftPixel = image[i][j - 1];
        *rightPixel = image[i][j + 1];
        *bottomLeftPixel = image[i + 1][j - 1];
        *bottomPixel = image[i + 1][j];
        *bottomRightPixel = image[i + 1][j + 1];
        if (pixelCount != 0)
        {
            *pixelCount = 9;
        }
    }
}