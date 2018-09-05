#include "pr_movement.h"
#include "bmpreader.h"

unsigned char *LoadBitmapFileProperties(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
    FILE *filePtr; //our file pointer
    BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
    unsigned char *bitmapImage;  //store image data
    int imageIdx=0;  //image index counter
    unsigned char tempRGB;  //our swap variable

    //open filename in read binary mode
    filePtr = fopen(filename,"r");
    if (filePtr == NULL)
        return NULL;

    //read the bitmap file header
    fread(&bitmapFileHeader.bfType, 2, 1, filePtr);
    fread(&bitmapFileHeader.bfSize, 4, 1, filePtr);
    fread(&bitmapFileHeader.bfReserved1, 2, 1, filePtr);
    fread(&bitmapFileHeader.bfReserved2, 2, 1, filePtr);
    fread(&bitmapFileHeader.bfOffBits, 4, 1, filePtr);

    //verify that this is a bmp file by check bitmap id
    if (bitmapFileHeader.bfType != 0x4D42)
    {
        fclose(filePtr);
        return NULL;
    }

    //read the bitmap info header
    fread(&bitmapInfoHeader->biSize, 4, 1, filePtr); // small edit. forgot to add the closing bracket at sizeof
    fread(&bitmapInfoHeader->biWidth, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biHeight, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biPlanes, 2, 1, filePtr);
    fread(&bitmapInfoHeader->biBitCount, 2, 1, filePtr);
    fread(&bitmapInfoHeader->biCompression, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biSizeImage, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biXPelsPerMeter, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biYPelsPerMeter, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biClrUsed, 4, 1, filePtr);
    fread(&bitmapInfoHeader->biClrImportant, 4, 1, filePtr);
    
    //close file and return bitmap iamge data
    fclose(filePtr);

    return 0;
}