unsigned char* load_frame(char const* filename, int width, int height, int rgb_channels);
void lhe_calculate_block_coordinates (int block_x, int block_y, int width, int height, BasicLheBlock **basic_block, 
                                int total_blocks_width, int total_blocks_height, int theoretical_block_width, 
                                int theoretical_block_height);
int stbi_write_bmp(char const *filename, int w, int h, int comp, const void *data);
