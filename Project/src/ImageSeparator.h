#ifndef __SEPARATOR__
#define __SEPARATOR__
#include <opencv2/opencv.hpp>
using namespace cv;

#include "vertices.h"

void separate_image(Mat image, int n_proc);
Mat image_reader(char *filename);
void slice_image(Mat image, Vertices vertices, vector<Rect> mCells, int n_proc);

#endif