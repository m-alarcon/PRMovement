#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define DEBUG 0
#define HORIZONTAL_BLOCKS 32

//Perceptual Relevance Params
#define PR_LUM_DIV 4
#define PR_HMAX 4.0
#define PR_MIN 0.2
#define PR_MAX 0.5
#define PR_DIF 0.3 //PR_MAX-PR_MIN
#define PR_QUANT_0 0
#define PR_QUANT_1 0.125
#define PR_QUANT_2 0.25
#define PR_QUANT_3 0.5
#define PR_QUANT_4 0.75
#define PR_QUANT_5 1
#define CORNERS 4
#define SIDE_MIN 2
#define TOP_LEFT_CORNER 0
#define TOP_RIGHT_CORNER 1
#define BOT_LEFT_CORNER 2
#define BOT_RIGHT_CORNER 3
#define QUANT_LUM0 8//4
#define QUANT_LUM1 16//12//8
#define QUANT_LUM2 32//24
#define QUANT_LUM3 80//72

#define BUFF_SIZE 5

typedef struct BasicLheBlock {
    uint32_t block_width;
    uint32_t block_height;
    uint32_t x_ini;
    uint32_t x_fin;
    uint32_t y_ini;
    uint32_t y_fin;
} BasicLheBlock;