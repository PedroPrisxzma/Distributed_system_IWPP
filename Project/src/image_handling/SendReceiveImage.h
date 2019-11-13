#ifndef __SENDRECEIVEIMG__
#define __SENDRECEIVEIMG__

#include <opencv2/opencv.hpp>
using namespace cv;

void matsnd(Mat& m,int dest);
Mat matrcv(int src);

#endif