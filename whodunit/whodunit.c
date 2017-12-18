#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Usage: ./whodunit infile outfile\n");
        return 1;
    }

    // Allocate filename pointers to set in memory

    char *inputFile = argv[1];
    char *outputFile = argv[2];

    // Open the file from the computer
    FILE *inptr = fopen(inputFile, "r");
    // Check existence of file
    if(inptr == NULL){
        fprintf(stderr, "Could not open %s.\n", inputFile);
        return 2;
    }

    // Allocate memory for output
    FILE *outptr = fopen(outputFile, "w");
    // Check if able to write
    if(outptr == NULL){
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outputFile);
        return 3;
    }

    // Get the file header to verify that it is a bitmap file
    // Declare instance of BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    // Read the header bits from the first position???
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // Get the info header to extract info from the file
    BITMAPINFOHEADER bi;
    // Read Info header bits starting from the last pos of the read???
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if(bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || bi.biBitCount != 24 || bi.biCompression != 0){
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported File Format.\n");
        return 4;
    }

    // Start writing some headers from memory to bmp
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // Writing Bitmap information from memory to bmp
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Padding
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over the height of the bitmap pixels

    for(int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++){

        // Iterate over BGR triples (Bitmap places them in reverse)
        for(int j = 0; j < bi.biWidth; j++){
            // Declare a buffer
            RGBTRIPLE triple;
            // Store temporary data from color space into the buffer
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // Assertion 1, remove all red pixels from the triple
            if(triple.rgbtRed == 255){
                triple.rgbtRed = 0;
            }

            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        // Skip over padding from the reading pointer of the original bitmap
        fseek(inptr, padding, SEEK_CUR);

        //add padding for when the bits of the scanline arn't divisible by 4
        for(int k = 0; k < padding; k++){
            fputc(0x00, outptr);
        }

    }

    // Close files to save memory or fclose also writes data from memory to hard disk
    fclose(outptr);
    fclose(inptr);

    return 0;
}