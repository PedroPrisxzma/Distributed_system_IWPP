#ifndef __MORPHOLOGIC__
#define __MORPHOLOGIC__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
#include <queue>
#include "../classes/vertices.h"
// #include <opencv/cv.hpp>


namespace nscale {

// DOES NOT WORK WITH MULTICHANNEL.
template <typename T>
// cv::Mat imreconstruct(const cv::Mat& seeds, const cv::Mat& image, int connectivity);
cv::Mat imreconstruct(int rank, std::queue<int> &borderValues, const cv::Mat& seeds, const cv::Mat& image, int connectivity,std::queue<int> &xQ, std::queue<int> &yQ);

}
#endif