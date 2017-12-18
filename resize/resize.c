#include <stdlib.h>
#include <stdio.h>

#include "bmp.h"

int main(int argc, char *argv[]){
    // Ensure proper usage
    if(argc != 4){
        fprintf(stderr, "Usage: ./resize n infile outfile\n");
        return 1;
    }

    // Convert factor to proper datatype and remember file name locations
    int n = atoi(argv[1]);
    char *inputFileName = argv[2];
    char *outputFileName = argv[3];

    // Range of factor n = 1 - 100;

    if(n < 1 || n > 100){
        fprintf(stderr, "n must be a positive integer not greater than 100\n");
        return 1;
    }

    // Open the file
    FILE *inptr = fopen(inputFileName, "r");
    if(inptr == NULL){
        fprintf(stderr, "Could not open %s.\n", inputFileName);
        return 2;
    }

    // Prepare memory for file writing
    FILE *outptr = fopen(outputFileName, "w");
    if(outptr == NULL){
        fprintf(stderr, "Could not create %s.\n", outputFileName);
        return 3;
    }

    // Read header info from bitmap input
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Ensure that the input is a 24bit uncompressed bitmap
    if(bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || bi.biBitCount != 24 || bi.biCompression != 0){
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    // Redirect the current reading pointer to beginning
    fseek(inptr, 0, SEEK_SET);

    // Prepare some new header files to be written to the output depending on n
    BITMAPFILEHEADER nbf;
    fread(&nbf, sizeof(BITMAPFILEHEADER), 1, inptr);

    BITMAPINFOHEADER nbi;
    fread(&nbi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // Set new width and height to the new resized bitmap
    nbi.biWidth *= n;
    nbi.biHeight *= n;

    int resizedPadding = (4 - (nbi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    nbi.biSizeImage = ((nbi.biWidth * sizeof(RGBTRIPLE)) + resizedPadding) * abs(nbi.biHeight);
    nbf.bfSize = nbf.bfOffBits + nbi.biSizeImage;

    // Write modified header to output
    fwrite(&nbf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&nbi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // Determine padding of original bitmap
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // Iterate over the original height
    for(int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++ ){
        // Iterate over pixels in scanline
        for(int j = 0; j < bi.biWidth; j++){
            // Temporary Buffer for the pixel (3 bytes)
            RGBTRIPLE triple;
            // Read pixel from memory
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // If factor is specified, stretch the horizontal scanline
            for(int k = 0; k < n; k++){
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }
        }

        // Add the padding for the new bitmap, if any
        for(int l = 0; l < resizedPadding; l++){
            fputc(0x00, outptr);
        }

        // Check if factor is specified more than 1
        if(n > 1){
            for(int k = 0; k < (n - 1); k++){
                // Rewind the pointer and copy again to new scanline
                fseek(inptr, -1 * (bi.biWidth * sizeof(RGBTRIPLE)), SEEK_CUR);

                for(int l = 0; l < bi.biWidth; l++){
                    // Again add another row
                    RGBTRIPLE triple;
                    fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
                    for(int m = 0; m < n; m++){
                        fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                    }
                }

                // Again, Add padding for the new row, if any
                for(int o = 0; o < resizedPadding; o++){
                    fputc(0x00, outptr);
                }
            }
        }

        // Skip over the padding of the input bitmap when it is done adding rows assuming that factor > 1
        fseek(inptr, padding, SEEK_CUR);
    }

    // Assuming that everything is finished

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // Successful execution
    return 0;
}