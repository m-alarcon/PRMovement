#pragma once

unsigned char *rgb, *y, *u, *v, *rec_rgb;
int pr_factor, total_blocks_width, total_blocks_height, theoretical_block_width, theoretical_block_height, initiated;
BasicLheBlock **basic_block;
float **pr_x, **pr_y, **diffs_x, **diffs_y;
float ***pr_x_buff, ***pr_y_buff;
int width, height, rgb_channels, linesize;
int buff_size;

void init_pr_computation(int width, int height, int rgb_channels);
void close_pr_computation();
void lhe_advanced_compute_perceptual_relevance (uint8_t *component_original_data_Y, float **pr_x, float **pr_y);
void pr_to_movement(int image_position_buffer);
float get_image_movement(int mode);
void create_frame(int draw_bar);
float get_block_movement(int block_x, int block_y);
