#include "pr_movement.h"
#include "stb_image.h"
#include "stb_image_write.h"


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
unsigned char* load_frame(char const* filename, int width, int height, int rgb_channels)
{

    unsigned char *rgb;
    if (DEBUG) printf("ENTER in load frame...\n");

    rgb = stbi_load(filename, &width, &height, &rgb_channels, 0);

    if (DEBUG) 
    {
        printf("Image loaded. width=%d, height=%d\n", width, height);
        stbi_write_bmp("kk.bmp", width, height, rgb_channels, rgb);
        printf("exit from load_frame...\n");
    }
    return rgb;
}

void lhe_calculate_block_coordinates (int block_x, int block_y, int width, int height, BasicLheBlock **basic_block, 
                                int total_blocks_width, int total_blocks_height, int theoretical_block_width, 
                                int theoretical_block_height)
{
    uint32_t xini_Y, xfin_Y, yini_Y, yfin_Y;
    uint32_t xini_UV, xfin_UV, yini_UV, yfin_UV;
    
    //LUMINANCE
    xini_Y = block_x * theoretical_block_width;
    xfin_Y = xini_Y + theoretical_block_width;
      
    yini_Y = block_y * theoretical_block_height;
    yfin_Y = yini_Y + theoretical_block_height ;

    
    //CHROMINANCE UV
    //xini_UV = block_x * procUV->theoretical_block_width;
    //xfin_UV = xini_UV + procUV->theoretical_block_width;
    
    //yini_UV = block_y * procUV->theoretical_block_height;
    //yfin_UV = yini_UV + procUV->theoretical_block_height ;
        
    //LIMITS
    //If width cant be divided by 32, all pixel excess is in the last block
    if (block_x == total_blocks_width -1) 
    {
        xfin_Y = width;
        //xfin_UV = procUV->width;
    }
    
    if (block_y == total_blocks_height -1) 
    {
        yfin_Y = height;
        //yfin_UV = procUV->height;
    }

    basic_block[block_y][block_x].x_ini = xini_Y;
    basic_block[block_y][block_x].x_fin = xfin_Y;
    basic_block[block_y][block_x].y_ini = yini_Y;
    basic_block[block_y][block_x].y_fin = yfin_Y;

    //procUV->basic_block[block_y][block_x].x_ini = xini_UV;
    //procUV->basic_block[block_y][block_x].x_fin = xfin_UV;
    //procUV->basic_block[block_y][block_x].y_ini = yini_UV;
    //procUV->basic_block[block_y][block_x].y_fin = yfin_UV;
    
    //Block length is calculated as fin-ini. I calculated block length because it is possible there are 
    //different block sizes. For example, any image whose width cant be divided by 32(number of blocks 
    //widthwise) will have at least one block that is smaller than the others.
    basic_block[block_y][block_x].block_width = xfin_Y - xini_Y;
    basic_block[block_y][block_x].block_height = yfin_Y - yini_Y;
    //procUV->basic_block[block_y][block_x].block_width = xfin_UV - xini_UV;
    //procUV->basic_block[block_y][block_x].block_height = yfin_UV - yini_UV;

}