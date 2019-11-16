#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;
#include "FindNeighbour.h"


vector<vector<BoundBox>> FindNeighbours(BoundBox* listaDeImagens, int size, int connectivity)
{
    int i,j;
    
    // Aloca lista externa de listas de imagens
    ::std::vector<vector<BoundBox>> vizinhos(size);
        
    for(i=0;i<size;i++)
    {   
        vizinhos[i] = vector<BoundBox>(0);

        cout << "PING!!! " << endl;
        int x_left_i = listaDeImagens[i].coordinateX;
        int x_right_i = listaDeImagens[i].coordinateX + listaDeImagens[j].edgeX;
        cout << "PONG??? " << endl;

        int y_up_i = listaDeImagens[i].coordinateY;
        int y_bot_i = listaDeImagens[i].coordinateY + listaDeImagens[i].edgeY;      

        cout << " --------------------------- " << endl;
        cout << "I -> cX:    "  << listaDeImagens[i].coordinateX << " cY: " << listaDeImagens[i].coordinateY << " eX: " << listaDeImagens[i].edgeX << " eY: " << listaDeImagens[i].edgeY<< endl;
        cout << " --------------------------- " << endl;
        for(j=0;j<size;j++)
        {
            int numOfNeighbours = 0;
            int x_left_j = listaDeImagens[j].coordinateX;
            int x_right_j = listaDeImagens[j].coordinateX + listaDeImagens[j].edgeX;
            
            int y_up_j = listaDeImagens[j].coordinateY;
            int y_bot_j = listaDeImagens[j].coordinateY + listaDeImagens[i].edgeY;            

            

            if(i==j) continue;


            // Check left neighbour
            if(CheckIfNeighbour(x_left_i, x_left_i, x_right_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                cout << "Left neighbour"  << endl;
                cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                cout << endl;
                numOfNeighbours++;
                vizinhos[i].resize(numOfNeighbours);
                vizinhos[i][j] = listaDeImagens[j];
                // Add neighbour
            }

            // Check up neighbour
            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_up_i, y_bot_j, y_bot_j) )
            {
                cout << "Up neighbour"  << endl;
                cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                cout << endl;
                numOfNeighbours++;
                vizinhos[i].resize(numOfNeighbours);
                vizinhos[i][j] = listaDeImagens[j];
                // Add neighbour
            }
            
            // Check right neighbour
            if(CheckIfNeighbour(x_right_i, x_right_i, x_left_j, x_left_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                cout << "Right neighbour"  << endl;
                cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                cout << endl;
                numOfNeighbours++;
                vizinhos[i].resize(numOfNeighbours);
                vizinhos[i][j] = listaDeImagens[j];
                // Add neighbour
            }

            // Check bot neighbour
            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_bot_i, y_bot_i, y_up_j, y_up_j) )
            {
                cout << "bot neighbour"  << endl;
                cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                cout << endl;
                numOfNeighbours++;
                vizinhos[i].resize(numOfNeighbours);
                vizinhos[i][j] = listaDeImagens[j];
                // Add neighbour
            }
        }

    }

    return vizinhos;
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