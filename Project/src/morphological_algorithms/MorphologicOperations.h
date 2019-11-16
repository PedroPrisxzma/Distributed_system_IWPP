#ifndef __MORPHOLOGIC__
#define __MORPHOLOGIC__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
// #include <opencv/cv.hpp>


namespace nscale {

// DOES NOT WORK WITH MULTICHANNEL.
template <typename T>
cv::Mat imreconstruct(const cv::Mat& seeds, const cv::Mat& image, int connectivity);

}
#endif