# PRMovement
Library with an example to calculate the movement of a video using perceptual relevance metrics

INSTALLATION:
To install this library use the command "cmake ." and then "make" and create a new folder named "output" to the directory.

USING:
To execute the library use "./prmovement [path/name] [extension of images] [numb of images to proccess]"
where <path/name> is the relative path of the images you want to proccess and the name of the images without number and file extension.
An example of this command is:
./prmovement ./video/frame .bmp 1000
where the name of the images in the directory ./video is frame1.bmp, frame2.bmp, frame3.bmp...

EXTRA:
There is an extra feature that enables multicore processing of PR metrics with OpenMP that speed up the library. To use that, ensure you have installed OpenMP on linux, using: sudo apt-get install libgomp1
By default, OpenMP use all the cores available on the processor. To change the number of cores you want to use use the following command:
export OMP_NUM_THREADS=<"number of cores you want to use">
I recommend to use "total cores - 1" in order to improve the performance, or try different configurations until you get the best performance.
