#ifndef __SENDRECEIVEIMG__
#define __SENDRECEIVEIMG__

#include <opencv2/opencv.hpp>
using namespace cv;

void matsnd(Mat& m, int dest, int image_or_mask);
Mat matrcv(int src, int image_or_mask);

#endif