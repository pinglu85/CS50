#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Define a new type called BYTE to store a byte of data.
typedef uint8_t BYTE;

int main(int argc, char *argv[])
{
    // Check if input file is provided via command line argument.
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // Store the input file name.
    char *infile = argv[1];

    // Open the input file.
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
    }

    // Divide the file size by the block size which is
    // 512 bytes to calculate the number of blocks.
    fseek(inptr, 0L, SEEK_END);
    long int fileSize = ftell(inptr);
    int blockSize = 512;
    int blockCount = fileSize / blockSize;

    // Reset the file pointer to the start of the file.
    fseek(inptr, 0, SEEK_SET);

    // Initialize isReadingJpeg to false, which is used to
    // track if we are reading a jpeg.
    int isReadingJpeg = 0;

    // Set the number of jpegs to 0.
    int jpegCount = 0;
    // Output file name.
    char outfile[8];
    // Output file pointer.
    FILE *outptr;

    // Read 512 bytes of the input file at a time,
    // repeat until there is no more block left.
    for (int i = 0; i < blockCount; i++)
    {
        // Dynamically allocate 512 bytes memory.
        BYTE *block = malloc(blockSize * sizeof(BYTE));
        if (block == NULL)
        {
            fprintf(stderr, "Not enough memory.\n");
            fclose(inptr);
            return 2;
        }

        // Read 512 bytes of the input file into to the block array.
        fread(block, sizeof(BYTE), blockSize, inptr);

        // Loop through the block array.
        for (int j = 0; j < blockSize; j++)
        {
            // Check if jpeg signature is encountered.
            if (block[j] == 0xff && block[j + 1] == 0xd8 && block[j + 2] == 0xff &&
                block[j + 3] >= 0xe0 && block[j + 3] <= 0xef)
            {
                // If jpeg signature is encountered and
                // we are currently not reading a jpeg,
                // that means we encounter the first jpeg.
                if (isReadingJpeg == 0)
                {
                    // Set isReadingJpeg to true.
                    isReadingJpeg = 1;

                    // Set the output file name according to the current number of jpegs
                    // and add leading zeros if the file name is not 3 digits.
                    sprintf(outfile, "%03d.jpg", jpegCount);

                    // Open the output file.
                    outptr = fopen(outfile, "w");
                    if (outptr == NULL)
                    {
                        fprintf(stderr, "Could not create %s.\n", outfile);
                        fclose(inptr);
                        return 3;
                    }

                    // Write current bytes to the output file.
                    fwrite(&block[j], sizeof(BYTE), 1, outptr);

                    // Increase the number of jpegs by 1.
                    jpegCount++;
                }
                // Else if jpeg signature is encoutered and we are currently reading
                // a jpeg, that means we have reached the end of a jpeg and encounter
                // a new one.
                else
                {
                    // Close the current output file.
                    fclose(outptr);

                    // Update the output file name according to the current number of jpegs
                    // and add leading zeros if the file name is not 3 digits.
                    sprintf(outfile, "%03d.jpg", jpegCount);

                    // Open a new output file.
                    outptr = fopen(outfile, "w");
                    if (outptr == NULL)
                    {
                        fprintf(stderr, "Could not create %s.\n", outfile);
                        fclose(inptr);
                        return 3;
                    }

                    // Write current bytes to the output file.
                    fwrite(&block[j], sizeof(BYTE), 1, outptr);

                    // Increase the number of jpegs by 1.
                    jpegCount++;
                }
            }
            // Ensure that we continue writing the byte to the ouput
            // file when a jpeg spans contiguous blocks.
            else if (isReadingJpeg == 1)
            {
                fwrite(&block[j], sizeof(BYTE), 1, outptr);
            }
        }

        // Free current allocated memory.
        free(block);
    }

    // Close the infile and outfile.
    fclose(inptr);
    fclose(outptr);
    return 0;
}
