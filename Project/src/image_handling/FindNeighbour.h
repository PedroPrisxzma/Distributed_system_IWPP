#ifndef __FINDNEIGHBOUR__
#define __FINDNEIGHBOUR__
#include <opencv2/opencv.hpp>
#include "../classes/imageChunk.h"
using namespace cv;


vector<vector<BoundBox>> FindNeighbours(BoundBox* lista, int size, int connectivity);
bool CheckIfNeighbour( int chunkCoordinate, int chunkCoordinateEnd, int neighbourCoordinate, int neighbourCoordinateEnd);
void printVector(vector<BoundBox> v, string title);
void removeDiagnals(vector<BoundBox>&leftNeighbour, vector<BoundBox>&topNeighbour, vector<BoundBox>&rightNeighbour, vector<BoundBox>&botNeighbour);

#endif