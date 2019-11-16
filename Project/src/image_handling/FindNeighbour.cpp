#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;
#include "FindNeighbour.h"


BoundBox** FindNeighbours(BoundBox* lista, int size, int connectivity)
{
    int i,j;
    bool intercect;
    vector<int> vizinos;
    for(i=0;i<size;i++)
    {   
        int x_left_up_i = lista[i].coordinateX;
        int x_left_bot_i = lista[i].coordinateX + list[i].edgeY;
        int x_right_up_i = lista[i].coordinateX + list[i].edgeX;
        
        int y_left_up_i = lista[i].coordinateY;
        int y_left_bot_i = lista[i].coordinateY + list[i].edgeY;
        int y_right_up_i = lista[i].coordinateY + list[i].edgeX;
       

        // cout << " --------------------------- " << endl;
        // cout << "I -> cX:    "  << lista[i].coordinateX << " cY: " << lista[i].coordinateY << " eX: " << lista[i].edgeX << " eY: " << lista[i].edgeY<< endl;
        // cout << " --------------------------- " << endl;
        for(j=0;j<size;j++)
        {
            int x_left_up_j = lista[j].coordinateY
            int x_left_bot_j = lista[j].coordinateX;
            int x_right_up_j = lista[j].coordinateX + lista[j].edgeX;
            
            int y_right_up_j = lista[j].coordinateY;
            int y_left_up_j = lista[j].coordinateY;
            int y_right_bot_j = lista[j].coordinateY + lista[i].edgeY;            

            if(i==j) continue;

            // cout << "J -> cX: "  << lista[j].coordinateX << " cY: " << lista[j].coordinateY << " eX: " << lista[j].edgeX << " eY: " << lista[j].edgeY<< endl;
            // cout << endl;

            // Check left neighbour
            if(CheckIfNeighbour())
            {

            }
        
        }

    }

    BoundBox** test;
    return test;
}
                           //x1                 x2                      xa                       xb
bool CheckIfNeighbour( int chunkCoordinate, int chunkCoordinateEnd, int neighbourCoordinate, int neighbourCoordinateEnd)
{   
    // x1 < xa < x2
    if(chunkCoordinate<=neighbourCoordinate && chunkCoordinateEnd>=neighbourCoordinate) return true; 
    
    // x1 < xb < x2
    if(chunkCoordinate<=neighbourCoordinateEnd && chunkCoordinateEnd>=neighbourCoordinateEnd) return true; 
    
    // xa < x1 < xb
    if(neighbourCoordinate<=chunkCoordinate && neighbourCoordinateEnd>=chunkCoordinate) return true; 
    
    // xa < x2 < xb
    if(neighbourCoordinate<=chunkCoordinateEnd && neighbourCoordinateEnd>=chunkCoordinateEnd) return true;
    return false;
}



X
a------------------b
|                  |
|                  |
|                  |
c------------------d