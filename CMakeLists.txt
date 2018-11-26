cmake_minimum_required(VERSION 2.8)
project( PRMovement )
add_executable( prmovement main.c pr_movement.c bmpreader.c yuv_rgb.c imgutils.c)
target_link_libraries( prmovement -lm )
