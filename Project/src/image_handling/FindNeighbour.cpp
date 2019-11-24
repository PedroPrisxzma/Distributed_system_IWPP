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

        int x_left_i = listaDeImagens[i].coordinateX;
        int x_right_i = listaDeImagens[i].coordinateX + listaDeImagens[i].edgeX;

        int y_up_i = listaDeImagens[i].coordinateY;
        int y_bot_i = listaDeImagens[i].coordinateY + listaDeImagens[i].edgeY;      

        //cout << " --------------------------- " << endl;
        //cout << "I -> cX:    "  << listaDeImagens[i].coordinateX << " cY: " << listaDeImagens[i].coordinateY << " eX: " << listaDeImagens[i].edgeX << " eY: " << listaDeImagens[i].edgeY<< endl;
        //cout << " --------------------------- " << endl;
        for(j=0;j<size;j++)
        {
            if(i==j) continue;

            int numOfNeighbours = 0;
            int x_left_j = listaDeImagens[j].coordinateX;
            int x_right_j = listaDeImagens[j].coordinateX + listaDeImagens[j].edgeX;
            
            int y_up_j = listaDeImagens[j].coordinateY;
            int y_bot_j = listaDeImagens[j].coordinateY + listaDeImagens[j].edgeY;            

            
            // Check left neighbour
            if(CheckIfNeighbour(x_left_i, x_left_i, x_right_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                //cout << "Left neighbour"  << endl;
                //cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                //cout << endl;
                numOfNeighbours++;
                // vizinhos[i].resize(numOfNeighbours);
                // vizinhos[i][j] = listaDeImagens[j];
                vizinhos[i].push_back(listaDeImagens[j]);
                // Add neighbour
            }

            // Check up neighbour
            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_up_i, y_bot_j, y_bot_j) )
            {
                //cout << "Up neighbour"  << endl;
                //cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                //cout << endl;
                numOfNeighbours++;
                // vizinhos[i].resize(numOfNeighbours);
                // vizinhos[i][j] = listaDeImagens[j];
                vizinhos[i].push_back(listaDeImagens[j]);
                // Add neighbour
            }
            
            // Check right neighbour
            if(CheckIfNeighbour(x_right_i, x_right_i, x_left_j, x_left_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                //cout << "Right neighbour"  << endl;
                //cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                //cout << endl;
                numOfNeighbours++;
                // vizinhos[i].resize(numOfNeighbours);
                // vizinhos[i][j] = listaDeImagens[j];
                vizinhos[i].push_back(listaDeImagens[j]);
                // Add neighbour
            }

            // Check bot neighbour
            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_bot_i, y_bot_i, y_up_j, y_up_j) )
            {
                //cout << "bot neighbour"  << endl;
                //cout << "J -> cX: "  << listaDeImagens[j].coordinateX << " cY: " << listaDeImagens[j].coordinateY << " eX: " << listaDeImagens[j].edgeX << " eY: " << listaDeImagens[j].edgeY<< endl;
                //cout << endl;
                numOfNeighbours++;
                // vizinhos[i].resize(numOfNeighbours);
                // vizinhos[i][j] = listaDeImagens[j];
                vizinhos[i].push_back(listaDeImagens[j]);
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

void printVector(vector<BoundBox> v, string title)
{
	cout << title<< endl;
	int size = v.size();
	cout<< "	";
	for(int i =0; i<size;i++)
	{
		cout<< v[i].rank << " ";
	}
	cout<<endl;

}

void removeDiagnals(vector<BoundBox>&leftNeighbour, vector<BoundBox>&topNeighbour, vector<BoundBox>&rightNeighbour, vector<BoundBox>&botNeighbour)
{	
	//printVector(leftNeighbour, "left");
	//printVector(topNeighbour, "top");
	//printVector(rightNeighbour, "right");
	//printVector(botNeighbour, "bot");

	vector<int> leftIndexesToRemove;
	vector<int> topIndexesToRemove;
	vector<int> rightIndexesToRemove;
	vector<int> botIndexesToRemove;

	// Remove top left and bottom left
	int left_size = leftNeighbour.size();
	for(int i =0; i < left_size; i++)
	{
		int top_size = topNeighbour.size();
		for(int j =0; j < top_size; j++)
		{
			if(leftNeighbour[i].rank == topNeighbour[j].rank)
			{
			 	topIndexesToRemove.push_back(j);
				leftIndexesToRemove.push_back(i);
				break;
			}
		}

		int bot_size = botNeighbour.size();
		for(int h =0; h < bot_size; h++)
		{
			if(leftNeighbour[i].rank == botNeighbour[h].rank)
			{
			 	botIndexesToRemove.push_back(h);
				leftIndexesToRemove.push_back(i);
				break;
			}
		}
	}

	// Remove top right and bottom right 
	int right_size = rightNeighbour.size();
	for(int i =0; i < right_size; i++)
	{
		int top_size = topNeighbour.size();
		for(int j =0; j < top_size; j++)
		{
			if(rightNeighbour[i].rank == topNeighbour[j].rank)
			{
			 	topIndexesToRemove.push_back(j);
				rightIndexesToRemove.push_back(i);
				break;
			}
		}

		int bot_size = botNeighbour.size();
		for(int h =0; h < bot_size; h++)
		{
			if(rightNeighbour[i].rank == botNeighbour[h].rank)
			{
			 	botIndexesToRemove.push_back(h);
				rightIndexesToRemove.push_back(i);
				break;
			}
		}
	}

	// Remove from highest to lowest index
	left_size = leftIndexesToRemove.size();
	sort(leftIndexesToRemove.begin(), leftIndexesToRemove.end(), greater<int>()); 
	for(int i =0; i < left_size; i++)
	{
		//cout << "Left " << leftNeighbour[leftIndexesToRemove[i]] << endl;
		
		leftNeighbour.erase(leftNeighbour.begin() + leftIndexesToRemove[i]);
	}

	int top_size = topIndexesToRemove.size();
	sort(topIndexesToRemove.begin(), topIndexesToRemove.end(), greater<int>()); 
	for(int i =0; i < top_size; i++)
	{
		//cout << "Top "<< topNeighbour[topIndexesToRemove[i]] << endl;

		topNeighbour.erase(topNeighbour.begin() + topIndexesToRemove[i]);
	}

	right_size = rightIndexesToRemove.size();
	sort(rightIndexesToRemove.begin(), rightIndexesToRemove.end(), greater<int>()); 
	for(int i =0; i < right_size; i++)
	{
		//cout << "Right "<< rightNeighbour[rightIndexesToRemove[i]] << endl;

		rightNeighbour.erase(rightNeighbour.begin() + rightIndexesToRemove[i]);
	}

	int bot_size = botIndexesToRemove.size();
	sort(botIndexesToRemove.begin(), botIndexesToRemove.end(), greater<int>()); 
	for(int i =0; i < bot_size; i++)
	{
		//cout << "Bot "<< botNeighbour[botIndexesToRemove[i]] << endl;

		botNeighbour.erase(botNeighbour.begin() + botIndexesToRemove[i]);
	}

}
