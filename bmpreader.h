#include <stdint.h>

typedef struct tagBITMAPFILEHEADER
{
uint16_t bfType;  //specifies the file type
uint32_t bfSize;  //specifies the size in bytes of the bitmap file
uint16_t bfReserved1;  //reserved; must be 0
uint16_t bfReserved2;  //reserved; must be 0
uint32_t bfOffBits;  //species the offset in bytes from the bitmapfileheader to the bitmap bits
}BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
uint32_t biSize;  //specifies the number of bytes required by the struct
uint32_t biWidth;  //specifies width in pixels
uint32_t biHeight;  //species height in pixels
uint16_t biPlanes; //specifies the number of color planes, must be 1
uint16_t biBitCount; //specifies the number of bit per pixel
uint32_t biCompression;//spcifies the type of compression
uint32_t biSizeImage;  //size of image in bytes
uint32_t biXPelsPerMeter;  //number of pixels per meter in x axis
uint32_t biYPelsPerMeter;  //number of pixels per meter in y axis
uint32_t biClrUsed;  //number of colors used by th ebitmap
uint32_t biClrImportant;  //number of colors that are important
}BITMAPINFOHEADER;

unsigned char *LoadBitmapFileProperties(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);