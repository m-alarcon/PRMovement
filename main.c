#include "pr_movement.h"
#include "bmpreader.h"
#include "yuv_rgb.h"
#include "imgutils.h"
#include "perceptual_relevance_api.h"
#include "math.h"
#include "stdio.h"

int main( int argc, char** argv )
{
    char* imageName = argv[1];
    char* imageExt = argv[2];
    char* frameArg = argv[3];
    int frameNumber;
    char image[100];
    int frame = atoi(frameArg);
    initiated = 0;
    buff_size = BUFF_SIZE;

    struct timeval  tv1, tv2;
    double time = 0;


    for (frameNumber = 1; frameNumber < frame; frameNumber++){

        if (frameArg == NULL){
            frameNumber = 0;
            strcpy(image, imageName);
            strcat(image, imageExt);
        } else {
            //frameNumber = atoi(frameArg);
            sprintf(frameArg,"%i",frameNumber);
            strcpy(image, imageName);
            strcat(image, frameArg);
            strcat(image, imageExt);
        }

        printf("%s\n", image);

        BITMAPINFOHEADER bitmapInfoHeader;

        LoadBitmapFileProperties(image, &bitmapInfoHeader);
        width = bitmapInfoHeader.biWidth;
        height = bitmapInfoHeader.biHeight;
        rgb_channels = bitmapInfoHeader.biBitCount/8;

        if (initiated == 0)
        {
            init_pr_computation(width, height, rgb_channels);
            initiated = 1;
        }

        rgb = load_frame(image, width, height, rgb_channels);

        const size_t y_stride = width + (16-width%16)%16;
        const size_t uv_stride = y_stride;
        const size_t rgb_stride = width*3 +(16-(3*width)%16)%16;
        
        rgb24_yuv420_std(width, height, rgb, rgb_stride, y, u, v, y_stride, uv_stride, YCBCR_601);
        
        int position = (frameNumber-1)%buff_size;

gettimeofday(&tv1, NULL);

        lhe_advanced_compute_perceptual_relevance (y, pr_x_buff[position], pr_y_buff[position]);



        //pr_x_buff[position] = pr_x;
        //pr_y_buff[position] = pr_y;

        pr_to_movement(position);
        float movement = get_image_movement(0);

        fprintf( stderr, "%f\n", movement);

gettimeofday(&tv2, NULL);        

time += (double) (tv2.tv_usec - tv1.tv_usec);
        //printf("MOVEMENT: %f frame: %d\n", movement, frameNumber);

        create_frame(1);

        char frameName[100];
        sprintf(frameName,"./output/output%i.bmp",frameNumber);
        //printf("Frame name: %s\n", frameName);
        yuv420_rgb24_std(width, height, y, u, v, y_stride, uv_stride, rec_rgb, rgb_stride, YCBCR_601);
        stbi_write_bmp(frameName, width, height, rgb_channels, rec_rgb);


    }

printf("Time spent on compute movement: %f useconds\n", time/frame);
    close_pr_computation();

    return 0;
}