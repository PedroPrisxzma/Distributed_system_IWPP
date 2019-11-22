#ifndef __MORPHOLOGICADM__
#define __MORPHOLOGICADM__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
#include <queue>
#include "../classes/imageChunk.h"

Mat imReconstructAdm(Mat imgblock, Mat mskblock,BoundBox rankVertices, vector<BoundBox> rankNeighbours, int rank, int numeroDeProcessos);
Mat extractBorders(Mat image, int side);
int isThereANeighbour(BoundBox rankVertices, vector<BoundBox> rankNeighbours, int side);
bool imagesIsEqual(const cv::Mat mat1, const cv::Mat mat2);


#endif