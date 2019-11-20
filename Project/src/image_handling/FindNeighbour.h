#ifndef __FINDNEIGHBOUR__
#define __FINDNEIGHBOUR__
#include <opencv2/opencv.hpp>
#include "../classes/imageChunk.h"
using namespace cv;


vector<vector<BoundBox>> FindNeighbours(BoundBox* lista, int size, int connectivity);
bool CheckIfNeighbour( int chunkCoordinate, int chunkCoordinateEnd, int neighbourCoordinate, int neighbourCoordinateEnd);

#endif