#ifndef __MORPHOLOGICADM__
#define __MORPHOLOGICADM__
#define DllExport //nothing 

#include <opencv2/opencv.hpp>
#include <queue>
#include "../classes/imageChunk.h"

Mat imReconstructAdm(Mat imgblock, Mat mskblock,BoundBox rankVertices, vector<BoundBox> rankNeighbours, int rank, int numeroDeProcessos);

bool stopCondition(int rank, int numeroDeProcessos, int myState);
int checkAllFinished(int myState, int rank, int numDeProcessos);
void alertAllOtherProcesses(int state, int rank, int numeroDeProcessos);

void addReceivedBorderCoordinatesToQueue(int rank, BoundBox rankVertices, BoundBox neighbour, Mat receivedBorder, std::queue<int> &xQueue, std::queue<int> &yQueue, std::queue<int> &borderValues, int side);
void receiveBordersFromNeighbours(int rank, BoundBox rankVertices, vector<vector<BoundBox>> neighbours, std::queue<int> &xQueue, std::queue<int> &yQueue, std::queue<int> &borderValues);

void sendBorderToNeighbours(vector<Mat> previousBorders, vector<Mat> leftTopRightBotBorders, vector<vector<BoundBox>> neighbours, BoundBox rankVertices);
void getIntersection(int coordinate, int edge, int neighbourCoordinate, int neighbourEdge, int &start, int &end);
Mat cutIntersection(Mat borderToCut, BoundBox rankVertice, BoundBox neighbour, int side);

Mat extractBorders(Mat image, int side);
vector<Mat> getBorders(Mat reconstructedImage);
vector<BoundBox> isThereANeighbour(BoundBox rankVertices, vector<BoundBox> rankNeighbours, int side);
void printNeighbours(int rank, vector<vector<BoundBox>> neighbours);



#endif