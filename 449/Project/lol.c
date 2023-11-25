#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//****define structs
#pragma pack(1)
typedef struct
{
    uint16_t format;
    uint32_t width;
    uint32_t height;
    uint32_t numOfColors;
    uint32_t paletteOffset;
    uint32_t imageOffset;
} CBMHeader;

typedef struct
{
    uint8_t red;
    uint8_t green;
    uint8_t blue;
} RGBColor;

typedef struct
{
    uint8_t format[2];
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMPHeader;

typedef struct
{
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t numOfColor;
    uint16_t numOfBits;
    uint32_t compression;
    uint32_t imageSize;
    int32_t horizontal;
    int32_t vertical;
    uint32_t colorsPalette;
    uint32_t importantColors;
} DIBHeader;
#pragma pack(4)

//***convert the CBM to BMP function
void convertCBMtoBMP(const char *cbmFilename, const char *bmpFilename)
{

    // open the CBM file to read, print error if null
    FILE *cbmFile = fopen(cbmFilename, "r");
    if (cbmFile == NULL)
    {
        fprintf(stderr, "Error opening CBM file: %s\n", cbmFilename);
        return;
    }

    // create array for palette + read headers
    CBMHeader cbmHeader;
    BMPHeader bmpHeader;
    DIBHeader dibHeader;
    int numColors = 16;
    RGBColor *color;

    color = (RGBColor *)malloc(numColors * sizeof(RGBColor));

    fread(&cbmHeader, sizeof(CBMHeader), 1, cbmFile);

    // check if the format is supported
    if (cbmHeader.format != 0x7449)
    {
        printf("The format is not supported.\n");
        fclose(cbmFile);
        return;
    }

    fseek(cbmFile, cbmHeader.paletteOffset, SEEK_SET);
    fread(color, sizeof(RGBColor), cbmHeader.numOfColors, cbmFile);

    // calculate the image size
    uint32_t bmpImageSize = (cbmHeader.width * cbmHeader.height * 3) + sizeof(dibHeader) + sizeof(bmpHeader);

    // calculate the size of bmp
    uint32_t bmpSize = sizeof(BMPHeader) + sizeof(DIBHeader);

    bmpHeader.format[0] = 'B';
    bmpHeader.format[1] = 'M';
    bmpHeader.size = 101;
    bmpHeader.reserved1 = 0;
    bmpHeader.reserved2 = 0;
    dibHeader.size = sizeof(DIBHeader);
    dibHeader.width = cbmHeader.width;
    dibHeader.height = cbmHeader.height;
    dibHeader.numOfColor = 1;
    dibHeader.numOfBits = 24;
    dibHeader.compression = 0;
    dibHeader.imageSize = bmpImageSize;
    dibHeader.horizontal = 0;
    dibHeader.vertical = 0;
    dibHeader.colorsPalette = 0;
    dibHeader.importantColors = 0;

    // open BMP file for writing
    FILE *bmpFile = fopen(bmpFilename, "w");
    if (bmpFile == NULL)
    {
        fprintf(stderr, "Error opening BMP file: %s\n", bmpFilename);
        fclose(cbmFile);
        return;
    }

    // write BMP header and DIB header to BMP file
    fwrite(&bmpHeader, sizeof(BMPHeader), 1, bmpFile);
    fwrite(&dibHeader, sizeof(DIBHeader), 1, bmpFile);

    // calculate padding
    int rowSize = cbmHeader.width * sizeof(RGBColor);
    int paddingSize = (4 - (rowSize % 4)) % 4;
    uint8_t paddingByte = 0;

    // create an array to store the pixel data from the CBM file
    uint8_t *pixelData = (uint8_t *)malloc(bmpImageSize);
    fread(pixelData, 1, bmpImageSize, cbmFile);

    // iterate through and get the pixel index and look up the color
    for (int y = cbmHeader.height - 1; y >= 0; y--)
    {

        for (int x = 0; x < cbmHeader.width; x++)
        {
            uint8_t paletteIndex = pixelData[y * cbmHeader.width + x];
            RGBColor pixelColor = color[paletteIndex];

            // convert RGB to BGR, then write the pixel to BMP
            RGBColor reverseColor;
            reverseColor.red = pixelColor.blue;
            reverseColor.green = pixelColor.green;
            reverseColor.blue = pixelColor.red;
            fwrite(&reverseColor, sizeof(RGBColor), 1, bmpFile);
        }

        // iterate, padd row if needed
        for (int i = 0; i < paddingSize; i++)
        {
            fwrite(&paddingByte, 1, 1, bmpFile);
        }
    }

    // close files
    free(pixelData);
    fclose(bmpFile);

    // //////////////////////////

    // bmpFile = fopen(bmpFilename, "rb+");
    // if (bmpFile == NULL)
    // {
    //     fprintf(stderr, "Error opening BMP file for header update: %s\n", bmpFilename);
    //     fclose(bmpFile);
    //     return;
    // }

    // // Update BMP and DIB headers with correct values
    // bmpHeader.size = sizeof(BMPHeader) + sizeof(DIBHeader) + bmpImageSize;
    // dibHeader.width = 1200;
    // dibHeader.height = cbmHeader.height;

    // // Seek to the beginning and write updated headers
    // fseek(bmpFile, 0, SEEK_SET);
    // fwrite(&bmpHeader, sizeof(BMPHeader), 1, bmpFile);
    // fwrite(&dibHeader, sizeof(DIBHeader), 1, bmpFile);

    // // Close the BMP file
    // fclose(bmpFile);
    /////////////////////////////
    ///////////////

        /////////////////
    fclose(cbmFile);

    printf("CBM file \"%s\" converted to BMP file \"%s\"\n", cbmFilename, bmpFilename);
}
////////////////////////////

// function to print CBM
int main(int argc, char *argv[])
{
    if (argc<2 | argc> 4)
    {
        fprintf(stderr, "Error: arguments were found");
        return -1;
    }

    char *action = argv[1];
    char *filenameCBM = argv[2];
    char *filenameBMP = argv[3];

    if (strcmp(action, "--info") == 0)
    {
        FILE *cbmFile = fopen(filenameCBM, "r");
        if (cbmFile == NULL)
        {
            printf("Error opening file: %s\n", filenameCBM);
            return -1;
        }

        CBMHeader cbmHeader;
        int numColors = 256;
        RGBColor *color;
        // array for the colors
        color = (RGBColor *)malloc(numColors * sizeof(RGBColor));

        fread(&cbmHeader, sizeof(CBMHeader), 1, cbmFile);

        // Check if the format is 0x7449
        if (cbmHeader.format != 0x7449)
        {
            printf("The format is not supported.\n");
            fclose(cbmFile);
            return -1;
        }
        fseek(cbmFile, cbmHeader.paletteOffset, SEEK_SET);
        fread(color, sizeof(RGBColor), cbmHeader.numOfColors, cbmFile);

        // print header information
        printf("=== CBM Header ===\n");
        printf("Magic: 0x%04X\n", cbmHeader.format);
        printf("Width: %d\n", cbmHeader.width);
        printf("Height: %d\n", cbmHeader.height);
        printf("Number of colors: %d\n", cbmHeader.numOfColors);
        printf("Color array offset: %d\n", cbmHeader.paletteOffset);
        printf("Image array offset: %d\n", cbmHeader.imageOffset);

        // print palette
        printf("\n=== Palette (R, G, B) ===\n");
        for (int i = 0; i < cbmHeader.numOfColors; i++)
        {
            printf("Color %d: (%d, %d, %d)\n", i, color[i].red, color[i].green, color[i].blue);
        }
    }
    else if (strcmp(action, "--bmp-info") == 0)
    {
        char *fileExtension = strrchr(filenameCBM, '.');
        if (fileExtension != NULL)
        {
            if (strcmp(fileExtension, ".cbm") == 0)
            {

                // Add your CBM file processing code here
                FILE *bmpFile = fopen(filenameCBM, "r");
                if (bmpFile == NULL)
                {
                    printf("Error opening file: %s\n", filenameCBM);
                    return 1;
                }

                CBMHeader cbmHeader;
                BMPHeader bmpHeader;
                DIBHeader dibHeader;

                fread(&cbmHeader, sizeof(CBMHeader), 1, bmpFile);

                fseek(bmpFile, 0, SEEK_SET);
                fread(&bmpHeader, sizeof(BMPHeader), 1, bmpFile);
                fread(&dibHeader, sizeof(DIBHeader), 1, bmpFile);

                bmpHeader.reserved1 = 0;
                bmpHeader.reserved2 = 0;
                bmpHeader.format[0] = 'B';
                bmpHeader.format[1] = 'M';
                dibHeader.numOfColor = 1;
                dibHeader.numOfBits = 24;
                dibHeader.compression = 0;
                dibHeader.imageSize = 0;
                dibHeader.horizontal = 0;
                dibHeader.vertical = 0;
                dibHeader.colorsPalette = 0;
                dibHeader.importantColors = 0;

                int wid = cbmHeader.width * 3;
                int pad = 0;
                while ((wid % 4) != 0)
                {
                    wid++;
                    pad++;
                }

                // print bmp information
                printf("=== BMP Header ===\n");
                printf("Type: %c%c\n", bmpHeader.format[0], bmpHeader.format[1]);
                printf("Size: %lu\n", (cbmHeader.width * cbmHeader.height * 3) + sizeof(DIBHeader) + sizeof(BMPHeader) + (pad * cbmHeader.height));
                printf("Reserved 1: %d\n", bmpHeader.reserved1);
                printf("Reserved 2: %d\n", bmpHeader.reserved2);
                printf("Image offset: %lu\n", sizeof(DIBHeader) + sizeof(BMPHeader));

                // print DIB header information
                printf("\n=== DIB Header ===\n");
                printf("Size: %lu\n", sizeof(dibHeader));
                printf("Width: %d\n", cbmHeader.width);
                printf("Height: %d\n", cbmHeader.height);
                printf("# color planes: %d\n", dibHeader.numOfColor);
                printf("# bits per pixel: %d\n", dibHeader.numOfBits);
                printf("Compression scheme: %d\n", dibHeader.compression);
                printf("Image size: %d\n", dibHeader.imageSize);
                printf("Horizontal resolution: %d\n", dibHeader.horizontal);
                printf("Vertical resolution: %d\n", dibHeader.vertical);
                printf("# colors in palette: %d\n", dibHeader.colorsPalette);
                printf("# important colors: %d\n", dibHeader.importantColors);

                fclose(bmpFile);
            }
            else if (strcmp(fileExtension, ".bmp") == 0)
            {

                // Add your BMP file processing code here
                FILE *bmpFile = fopen(filenameCBM, "r");
                if (bmpFile == NULL)
                {
                    printf("Error opening file: %s\n", filenameCBM);
                    return 1;
                }

                CBMHeader cbmHeader;
                BMPHeader bmpHeader;
                DIBHeader dibHeader;

                fread(&cbmHeader, sizeof(CBMHeader), 1, bmpFile);

                fseek(bmpFile, 0, SEEK_SET);
                fread(&bmpHeader, sizeof(BMPHeader), 1, bmpFile);
                fread(&dibHeader, sizeof(DIBHeader), 1, bmpFile);

                bmpHeader.reserved1 = 0;
                bmpHeader.reserved2 = 0;
                bmpHeader.format[0] = 'B';
                bmpHeader.format[1] = 'M';
                dibHeader.numOfColor = 1;
                dibHeader.numOfBits = 24;
                dibHeader.compression = 0;
                dibHeader.imageSize = 0;
                dibHeader.horizontal = 0;
                dibHeader.vertical = 0;
                dibHeader.colorsPalette = 0;
                dibHeader.importantColors = 0;

                int wid = cbmHeader.width * 3;
                int pad = 0;
                while ((wid % 4) != 0)
                {
                    wid++;
                    pad++;
                }

                // print bmp information
                printf("=== BMP Header ===\n");
                printf("Type: %c%c\n", bmpHeader.format[0], bmpHeader.format[1]);
                printf("Size: %lu\n", (dibHeader.width * dibHeader.height * 3) + sizeof(DIBHeader) + sizeof(BMPHeader) + (pad * dibHeader.height));
                printf("Reserved 1: %d\n", bmpHeader.reserved1);
                printf("Reserved 2: %d\n", bmpHeader.reserved2);
                printf("Image offset: %lu\n", sizeof(DIBHeader) + sizeof(BMPHeader));

                // print DIB header information
                printf("\n=== DIB Header ===\n");
                printf("Size: %lu\n", sizeof(dibHeader));
                printf("Width: %d\n", dibHeader.width);
                printf("Height: %d\n", dibHeader.height);
                printf("# color planes: %d\n", dibHeader.numOfColor);
                printf("# bits per pixel: %d\n", dibHeader.numOfBits);
                printf("Compression scheme: %d\n", dibHeader.compression);
                printf("Image size: %d\n", dibHeader.imageSize);
                printf("Horizontal resolution: %d\n", dibHeader.horizontal);
                printf("Vertical resolution: %d\n", dibHeader.vertical);
                printf("# colors in palette: %d\n", dibHeader.colorsPalette);
                printf("# important colors: %d\n", dibHeader.importantColors);

                fclose(bmpFile);
            }
            else
            {
                printf("Unsupported file format: %s\n", filenameCBM);
            }
        }
        else if (strcmp(action, "--convert") == 0)
        {
            convertCBMtoBMP(filenameCBM, filenameBMP);
        }

        return 0;
    }
}
