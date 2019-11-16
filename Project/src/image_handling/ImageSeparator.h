#ifndef __SEPARATOR__
#define __SEPARATOR__
#include <opencv2/opencv.hpp>
using namespace cv;

#include "../classes/vertices.h"
#include "../classes/imageChunk.h"

ImageChunk separate_image(Mat image, Mat mask, int numProcessos);
Mat image_reader(char *filename);
void slice_image(Mat image, Mat mask, Vertices vertices, ImageChunk *vetorDeBlocos, int numProcessos);


#endif