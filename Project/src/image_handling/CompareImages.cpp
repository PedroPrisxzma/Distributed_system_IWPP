#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;


bool imagesIsEqual(const cv::Mat mat1, const cv::Mat mat2)
{
    // treat two empty mat as identical as well
    if (mat1.empty() && mat2.empty()) {
        return true;
    }

    // if dimensionality of two mat is not identical, these two mat are not identical
    if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims) {
        return false;
    }
    cv::Mat diff;
    cv::compare(mat1, mat2, diff, cv::CMP_NE);
    int nz = cv::countNonZero(diff);
    return nz==0;
}
// It is important to stand out that the function cv::countNonZero only works
// with cv::Mat of one channel