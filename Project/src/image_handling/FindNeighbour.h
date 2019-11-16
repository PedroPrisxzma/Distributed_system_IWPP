#ifndef __FINDNEIGHBOUR__
#define __FINDNEIGHBOUR__
#include <opencv2/opencv.hpp>
using namespace cv;

typedef struct boundBox
{
    int coordinateX, coordinateY, edgeX, edgeY, rank;
} BoundBox; 


vector<vector<BoundBox>> FindNeighbours(BoundBox* lista, int size, int connectivity);
bool CheckIfNeighbour( int chunkCoordinate, int chunkCoordinateEnd, int neighbourCoordinate, int neighbourCoordinateEnd);

#endif