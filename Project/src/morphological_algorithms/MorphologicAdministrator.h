#ifndef __MORPHOLOGICADM__
#define __MORPHOLOGICADM__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
#include <queue>
#include "../classes/imageChunk.h"

Mat imReconstructAdm(Mat imgblock, Mat mskblock, vector<BoundBox> rankNeighbours, int rank);

#endif