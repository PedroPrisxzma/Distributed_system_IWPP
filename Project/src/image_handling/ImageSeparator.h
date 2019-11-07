#ifndef __SEPARATOR__
#define __SEPARATOR__
#include <opencv2/opencv.hpp>
using namespace cv;

#include "../classes/vertices.h"

vector<Rect> separate_image(Mat image, int numProcessos);
Mat image_reader(char *filename);
void slice_image(Mat image, Vertices vertices, vector<Rect> mCells, int numProcessos);

#endif