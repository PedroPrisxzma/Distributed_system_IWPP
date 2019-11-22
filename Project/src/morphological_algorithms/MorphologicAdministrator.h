#ifndef __MORPHOLOGICADM__
#define __MORPHOLOGICADM__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
#include <queue>
#include "../classes/imageChunk.h"

Mat imReconstructAdm(Mat imgblock, Mat mskblock,BoundBox rankVertices, vector<BoundBox> rankNeighbours, int rank, int numeroDeProcessos);
void sendBorderToNeighbours(vector<Mat> previousBorders, vector<Mat> leftTopRightBotBorders, vector<int> neighbours);
Mat extractBorders(Mat image, int side);
vector<Mat> getBorders(Mat reconstructedImage);
int isThereANeighbour(BoundBox rankVertices, vector<BoundBox> rankNeighbours, int side);



#endif