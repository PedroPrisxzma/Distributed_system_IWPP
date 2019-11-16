#ifndef __FINDNEIGHBOUR__
#define __FINDNEIGHBOUR__
#include <opencv2/opencv.hpp>
using namespace cv;

typedef struct boundBox
{
    int coordinateX, coordinateY, edgeX, edgeY, rank;
} BoundBox; 


BoundBox** FindNeighbours(BoundBox*, int);
#endif