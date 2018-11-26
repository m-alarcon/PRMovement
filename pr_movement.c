#include "pr_movement.h"
#include "bmpreader.h"
#include "yuv_rgb.h"
#include "imgutils.h"
#include "perceptual_relevance_api.h"
#include "math.h"
#include "stdio.h"

//int width, height, rgb_channels, linesize;
//int buff_size;

void init_pr_computation(int width, int height, int rgb_channels)
{

    int pixels_block;

    linesize = width;

    y = malloc(width*height);
    u = malloc(width*height);
    v = malloc(width*height);
    rec_rgb = malloc(width*height*rgb_channels);

    total_blocks_width = HORIZONTAL_BLOCKS;
    pixels_block = width / HORIZONTAL_BLOCKS;
    total_blocks_height = height / pixels_block;

    basic_block = malloc(total_blocks_height * sizeof(BasicLheBlock *));
        
    for (int i=0; i < total_blocks_height; i++)
    {
        basic_block[i] = malloc(total_blocks_width * sizeof(BasicLheBlock));
    }

    pr_x = malloc((total_blocks_height+1) * sizeof(float*));
        
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        pr_x[i] = malloc((total_blocks_width+1) * sizeof(float));
    }
     
    pr_y = malloc((total_blocks_height+1) * sizeof(float*));
        
    for (int i=0; i<total_blocks_height+1; i++)
    {
        pr_y[i] = malloc((total_blocks_width+1) * sizeof(float));
    }

    diffs_x = malloc((total_blocks_height+1) * sizeof(float*));
        
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        diffs_x[i] = malloc((total_blocks_width+1) * sizeof(float));
    }

    diffs_y = malloc((total_blocks_height+1) * sizeof(float*));
        
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        diffs_y[i] = malloc((total_blocks_width+1) * sizeof(float));
    }

    pr_factor = width/128;
    if (pr_factor == 0) pr_factor = 1;

    theoretical_block_width = width / total_blocks_width;
    theoretical_block_height = height / total_blocks_height;       

    for (int x=0; x < total_blocks_width; x++)
    {
        for (int y=0; y < total_blocks_height; y++)
        {
            lhe_calculate_block_coordinates(x, y, width, height, basic_block, total_blocks_width, total_blocks_height, 
                theoretical_block_width, theoretical_block_height);
        }
    }

    //Buffer circular para guardar las PR de frames anteriores
    pr_x_buff = malloc(buff_size * sizeof(float**));
    
    for (int i=0; i < buff_size; i++)
    {
        pr_x_buff[i] = malloc((total_blocks_height+1) * sizeof(float*));    
        for (int j=0; j<total_blocks_height+1; j++) 
        {
            pr_x_buff[i][j] = malloc((total_blocks_width+1) * sizeof(float));
        }
    }

    pr_y_buff = malloc(buff_size * sizeof(float**));
    
    for (int i=0; i < buff_size; i++)
    {
        pr_y_buff[i] = malloc((total_blocks_height+1) * sizeof(float*));    
        for (int j=0; j<total_blocks_height+1; j++) 
        {
            pr_y_buff[i][j] = malloc((total_blocks_width+1) * sizeof(float));
        }
    }
        
    

/*
    (&s->procUV)->width = ((&s->procY)->width - 1)/s->chroma_factor_width + 1;
    (&s->procUV)->height = ((&s->procY)->height - 1)/s->chroma_factor_height + 1;

    (&s->procUV)->theoretical_block_width = (&s->procUV)->width / total_blocks_width;
    (&s->procUV)->theoretical_block_height = (&s->procUV)->height / total_blocks_height;
*/
}

void close_pr_computation()
{

    free(y);
    free(u);
    free(v);
    free(rec_rgb);

    for (int i=0; i < total_blocks_height; i++)
    {
        free(basic_block[i]);
    }

    free (basic_block);
    
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        free (pr_x[i]);
    }    

    free (pr_x);

    for (int i=0; i<total_blocks_height+1; i++)
    {
        free (pr_y[i]);
    }
     
    free (pr_y);
        
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        free (diffs_x[i]);
    }

    free (diffs_x);
        
    for (int i=0; i<total_blocks_height+1; i++) 
    {
        free (diffs_y[i]);
    }

    free (diffs_y);
        
    for (int i=0; i < buff_size; i++)
    {    
        free (pr_x_buff[i]);
    }

    free (pr_x_buff);
    
    for (int i=0; i < buff_size; i++)
    {
        free (pr_y_buff[i]);
    }

    free (pr_y_buff);

}

static void lhe_advanced_compute_pr_lum (float **pr_x, float **pr_y, uint8_t *component_original_data, 
                                       int xini, int xfin, int yini, int yfin, 
                                       int linesize, int block_x, int block_y, 
                                       uint8_t sps_ratio_height, uint8_t sps_ratio_width)
{
         
    //Hops computation.
    int lum_dif, last_lum_dif, pix, dif_pix, y, x, dif;
    int Cx, Cy;
    float PRx, PRy;
    bool lum_sign, last_lum_sign;
    float **perceptual_relevance_x, **perceptual_relevance_y;

    perceptual_relevance_x = pr_x;
    perceptual_relevance_y = pr_y;

    lum_dif=0;
    last_lum_dif=0;
    lum_sign=true;
    last_lum_sign=true;                        
    Cx=0;
    Cy=0;
    PRx = 0;
    PRy = 0;

    for (y=yini+sps_ratio_height/2;y<yfin;y+=sps_ratio_height)
    {   
        pix = y*linesize+xini;
        if (y>0)//Pixel inicial de cada scanline horizontal
        {
            dif=component_original_data[pix]-component_original_data[pix-linesize];
            last_lum_dif=dif;
            if (!(last_lum_sign=(last_lum_dif>=0))) last_lum_dif = -last_lum_dif;

            if (last_lum_dif < QUANT_LUM0) {last_lum_dif = 0;}
            else if (last_lum_dif < QUANT_LUM1) {last_lum_dif = 1;}
            else if (last_lum_dif < QUANT_LUM2) {last_lum_dif = 2;}
            else if (last_lum_dif < QUANT_LUM3) {last_lum_dif = 3;}
            else last_lum_dif = 4;
        }

        for (x=xini;x<xfin;x++) //para poder hacer lum_dif
        {
            dif=0;
            if (x>0) dif=component_original_data[pix]-component_original_data[pix-1];
            lum_dif=dif;
            if (!(lum_sign=(lum_dif>=0))) lum_dif = -lum_dif;
            if (lum_dif < QUANT_LUM0) {lum_dif = 0;}
            else if (lum_dif < QUANT_LUM1) {lum_dif = 1;}
            else if (lum_dif < QUANT_LUM2) {lum_dif = 2;}
            else if (lum_dif < QUANT_LUM3) {lum_dif = 3;}
            else lum_dif = 4;
            if (lum_dif==0) {
                pix++;
                last_lum_dif=lum_dif;
                last_lum_sign=lum_sign;
                continue;
            }

            if ((lum_sign!=last_lum_sign && last_lum_dif!=0) || lum_dif==4) {
                int weight=lum_dif;
                PRx+=weight;
                Cx++;
            }
            last_lum_dif=lum_dif;
            last_lum_sign=lum_sign;
            pix++;
        }
    }

    lum_sign=true;
    lum_dif=0;
    last_lum_sign=true;
    last_lum_dif=0;

    for (x=xini+sps_ratio_width/2;x<xfin;x+=sps_ratio_width)
    {
        pix = yini*linesize+x;    
        if (x>0)
        {
            dif=component_original_data[pix]-component_original_data[pix-1];
            last_lum_dif=dif;
            if (!(last_lum_sign=(last_lum_dif>=0))) last_lum_dif = -last_lum_dif;
            if (last_lum_dif < QUANT_LUM0) {last_lum_dif = 0;}
            else if (last_lum_dif < QUANT_LUM1) {last_lum_dif = 1;}
            else if (last_lum_dif < QUANT_LUM2) {last_lum_dif = 2;}
            else if (last_lum_dif < QUANT_LUM3) {last_lum_dif = 3;}
            else last_lum_dif = 4;
        }
        for (y=yini;y<yfin;y++)
        {  
                                              
            dif=0;
            if (y>0) dif=component_original_data[pix]-component_original_data[pix-linesize];
            lum_dif=dif;
            if (!(lum_sign=(lum_dif>=0))) lum_dif = -lum_dif;
            if (lum_dif < QUANT_LUM0) {lum_dif = 0;}
            else if (lum_dif < QUANT_LUM1) {lum_dif = 1;}
            else if (lum_dif < QUANT_LUM2) {lum_dif = 2;}
            else if (lum_dif < QUANT_LUM3) {lum_dif = 3;}
            else lum_dif = 4;
            if (lum_dif==0) {
                pix += linesize;
                last_lum_dif=lum_dif;
                last_lum_sign=lum_sign;
                continue;
            }

            if ((lum_sign!=last_lum_sign && last_lum_dif!=0) || lum_dif==4) {
                int weight=lum_dif;
                PRy+=weight;
                Cy++;
            }
            last_lum_dif=lum_dif;
            last_lum_sign=lum_sign;
            pix += linesize;
            

        }
    }

    if (PRx>0) {
        PRx=PRx/(Cx*4); 
    }
    if (PRy>0) {
        PRy=PRy/(Cy*4);
    }
    if (PRx>0.5f) PRx=0.5f;
    if (PRy>0.5f) PRy=0.5f;

    //PR HISTOGRAM EXPANSION
    PRx = (PRx-PR_MIN) / PR_DIF;
            
    //PR QUANTIZATION
    if (PRx < PR_QUANT_1) {
        PRx = PR_QUANT_0;
        //proc->pr_quanta_counter[0]++;
    } else if (PRx < PR_QUANT_2) {
        PRx = PR_QUANT_1;
        //proc->pr_quanta_counter[1]++;
    } else if (PRx < PR_QUANT_3) {
        PRx = PR_QUANT_2;
        //proc->pr_quanta_counter[2]++;
    } else if (PRx < PR_QUANT_4) {
        PRx = PR_QUANT_3;
        //proc->pr_quanta_counter[3]++;
    } else {
        PRx = PR_QUANT_5;
        //proc->pr_quanta_counter[4]++;
    }       

    perceptual_relevance_x[block_y][block_x] = PRx;

    //PR HISTOGRAM EXPANSION
    PRy = (PRy-PR_MIN) / PR_DIF;
    
    //PR QUANTIZATION
    if (PRy < PR_QUANT_1) {
        PRy = PR_QUANT_0;
        //proc->pr_quanta_counter[0]++;
    } else if (PRy < PR_QUANT_2) {
        PRy = PR_QUANT_1;
        //proc->pr_quanta_counter[1]++;
    } else if (PRy < PR_QUANT_3) {
        PRy = PR_QUANT_2;
        //proc->pr_quanta_counter[2]++;
    } else if (PRy < PR_QUANT_4) {
        PRy = PR_QUANT_3;
        //proc->pr_quanta_counter[3]++;
    } else {
        PRy = PR_QUANT_5;
        //proc->pr_quanta_counter[4]++;
    }
  
    perceptual_relevance_y[block_y][block_x] = PRy;

}


/**
 * Computes perceptual relevance. 
 * 
 * @param *s LHE Context
 * @param *component_original_data_Y original image data
 * @param linesize rectangle images create a square image in ffmpeg memory. Linesize is width used by ffmpeg in memory
 * @param total_blocks_width total blocks widthwise
 * @param total_blocks_height total blocks heightwise
 */
void lhe_advanced_compute_perceptual_relevance (uint8_t *component_original_data_Y, float **pr_x, float **pr_y) 
{
    uint32_t block_width, block_height, half_block_width, half_block_height;

    block_width = theoretical_block_width;
    block_height = theoretical_block_height;
    half_block_width = (block_width >>1);
    half_block_height = (block_width >>1);

    //#pragma omp parallel for schedule(static)
    for (int block_y=0; block_y<total_blocks_height+1; block_y++)      
    {
        for (int block_x=0; block_x<total_blocks_width+1; block_x++) 
        {   

            int xini, xfin, yini, yfin, xini_pr_block, xfin_pr_block, yini_pr_block, yfin_pr_block;

            //First LHE Block coordinates
            xini = block_x * block_width;
            xfin = xini + block_width;

            yini = block_y * block_height;
            yfin = yini + block_height;
            
            //PR Blocks coordinates 
            xini_pr_block = xini - half_block_width; 
            
            if (xini_pr_block < 0) 
            {
                xini_pr_block = 0;
            }
            
            xfin_pr_block = xfin - half_block_width;
            
            if (block_x == total_blocks_width) 
            {
                xfin_pr_block = width;
            }    
            
            yini_pr_block = yini - half_block_height;
            
            if (yini_pr_block < 0) 
            {
                yini_pr_block = 0;
            }
            
            yfin_pr_block = yfin - half_block_height;
            
            if (block_y == total_blocks_height)
            {
                yfin_pr_block = height;
            }

            //COMPUTE, HISTOGRAM EXPANSION AND QUANTIZATION
            lhe_advanced_compute_pr_lum (pr_x, pr_y, y, xini_pr_block, xfin_pr_block, yini_pr_block, yfin_pr_block, 
                                        linesize, block_x, block_y, pr_factor, pr_factor);
        }
    }
}

void pr_to_movement(int image_position_buffer)
{

    int prev_image_position_buffer;

    prev_image_position_buffer = image_position_buffer - 1;

    if (prev_image_position_buffer < 0)
        prev_image_position_buffer = buff_size - 1;

    //printf("image_position_buffer: %d, prev_image_position_buffer: %d\n", image_position_buffer, prev_image_position_buffer);

    for (int i = 0; i < total_blocks_width + 1; i++){
        for (int j = 0; j < total_blocks_height + 1; j++){
            diffs_x[j][i] = pr_x_buff[image_position_buffer][j][i] - pr_x_buff[prev_image_position_buffer][j][i];
            diffs_y[j][i] = pr_y_buff[image_position_buffer][j][i] - pr_y_buff[prev_image_position_buffer][j][i];
            if (diffs_x[j][i] < 0) diffs_x[j][i] = -diffs_x[j][i];
            if (diffs_y[j][i] < 0) diffs_y[j][i] = -diffs_y[j][i];
            //printf("diffs_x: %f, diffs_y %f\n", diffs_x[j][i], diffs_y[j][i]);
        }
    }

}

float get_image_movement(int mode)
{

    float accum = 0;
    float accum_x = 0, accum_y = 0;

    if (mode == 0){//Average measurement

        for (int i = 0; i < total_blocks_width + 1; i++){
            for (int j = 0; j < total_blocks_height + 1; j++){
                accum += (diffs_x[j][i] + diffs_y[j][i])/2;
            }
        }
        accum /= ((total_blocks_width+1)*(total_blocks_height+1));
    
    } else if (mode == 1) {//Maximum avg measurement PRx or PRy

        for (int i = 0; i < total_blocks_width + 1; i++){
            for (int j = 0; j < total_blocks_height + 1; j++){
                accum_x += diffs_x[j][i];
                accum_y += diffs_y[j][i];
            }
        }

        accum_x /= ((total_blocks_width+1)*(total_blocks_height+1));
        accum_y /= ((total_blocks_width+1)*(total_blocks_height+1));

        if (accum_x > accum_y) 
            accum = accum_x ;
        else 
            accum = accum_y;

    }

    return accum;

}

float get_block_movement(int block_x, int block_y)
{
    
    float accum_y, accum_x, accum;

    accum_x = (diffs_x[block_y][block_x]+diffs_x[block_y][block_x+1]+diffs_x[block_y+1][block_x]+diffs_x[block_y+1][block_x+1])/4;
    accum_y = (diffs_y[block_y][block_x]+diffs_y[block_y][block_x+1]+diffs_y[block_y+1][block_x]+diffs_y[block_y+1][block_x+1])/4;

    if(accum_x > accum_y)
        accum = accum_x;
    else
        accum = accum_y;

    return accum;

}

void paint_block(int block_x, int block_y, int color)
{

    int xini = block_x*theoretical_block_width;
    int xfin = xini+theoretical_block_width;
    int yini = block_y*theoretical_block_height;
    int yfin = yini+theoretical_block_height;

    if (xfin > width-1-theoretical_block_width)
        xfin = width;
    if (yfin > height-1-theoretical_block_height)
        yfin = height;

    for (int xx = xini; xx < xfin; xx++){
        for (int yy = yini; yy < yfin; yy++){
            y[yy*width+xx] = color;
        }
    }
    for (int xx = xini/2; xx < xfin/2; xx++){
        for (int yy = yini/2; yy < yfin/2; yy++){
            u[yy*width+xx] = 128;
            v[yy*width+xx] = 128;
        }
    }
}

void draw_movement_bar(float movement)
{
    
    int bar_height = movement*height;

    for (int xx = width-10; xx < width; xx++){
        for (int yy = height-1; yy > height - bar_height; yy--){
            y[yy*width+xx] = 255;
        }
    }

    for (int xx = width/2-5; xx < width/2; xx++){
        for (int yy = height/2-1; yy > height/2 - bar_height/2; yy--){
            u[yy*width+xx] = 0;
            v[yy*width+xx] = 0;
        }
    }




}

void create_frame(int draw_bar)
{

    float movement_block = 0;
    float movement = 0;
    int color;
    for (int block_y=0; block_y<total_blocks_height; block_y++)      
    {
        for (int block_x=0; block_x<total_blocks_width; block_x++) 
        {
            movement_block = get_block_movement(block_x, block_y);
            color = (int)(movement_block*255);
            if (color > 255) color = 255;
            if (color < 0) color = 0;
            paint_block(block_x, block_y, color);
        }
    }
    if (draw_bar)
    {
        movement = get_image_movement(1);
        draw_movement_bar(movement);
    }

}