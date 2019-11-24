#include <algorithm>
#include <queue>
#include <limits>
#include <list>
#include <omp.h>
#include <iostream>
#include <mpi.h>

#include "../classes/imageChunk.h"
#include "../image_handling/FindNeighbour.h"
#include "../image_handling/SendReceiveImage.h"
#include "../image_handling/CompareImages.h"
#include "MorphologicOperations.h"
#include "MorphologicAdministrator.h"

using namespace cv;

// A ideia é essa função chamar a imreconstruct, aqui podemos ficar "presos" até o programa acabar e
// fazer o fluxo de enviar borda para os vizinhos.
// Tem que terminar a função
Mat imReconstructAdm(Mat imgblock, Mat mskblock, BoundBox rankVertices, vector<BoundBox> rankNeighbours, int rank, int numeroDeProcessos)
{

	std::queue<int> xQueue;
	std::queue<int> yQueue;
	std::queue<int>  borderValues;


	Mat reconstructedImage;

	reconstructedImage = nscale::imreconstruct<unsigned char>(rank, borderValues, imgblock, mskblock, 4, xQueue, yQueue);

	// Extract first time processing borders
	vector<Mat> leftTopRightBotBorders = getBorders(reconstructedImage);
	//0 is the left border, 1 is the top border, 2 is the right border and 3 is the bottom border
	
	// Keeps track of the last borders that were sent.
	vector<Mat> previousBorders;
	int connectivity = 4;
	while (true)
	{	

		vector<BoundBox> leftNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 0);
		vector<BoundBox> topNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 1);
		vector<BoundBox> rightNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 2);
		vector<BoundBox> botNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 3);

		if(connectivity == 4)
		{
		//	removeDiagnals(leftNeighbour, topNeighbour, rightNeighbour, botNeighbour);
		}
		
		else if(connectivity == 8)
		{
			//TODO:
			// Tratar connectividade de 8 pixeis vizinhos
		}

		vector<vector<BoundBox>> neighbours = {leftNeighbour, topNeighbour, rightNeighbour, botNeighbour};
		
		if(false)
		{
			printNeighbours(rank, neighbours);
		}
		
		// Aqui envia a borda para o vizinho, no caso envia a img em formato MAT
		// (é literalmente os Z pixéis da imagem na borda), vamos tratar eles depois do recv.
		if(true)
			sendBorderToNeighbours(previousBorders, leftTopRightBotBorders, neighbours, rankVertices);
		
		// resets previous borders
		previousBorders.clear();

		// Update last sent borders
		int leftTopRightBotBordersLenght = leftTopRightBotBorders.size();
		for (int i = 0; i < leftTopRightBotBordersLenght; i++)
		{
			previousBorders.push_back(leftTopRightBotBorders[i]);
		}
		
		receiveBordersFromNeighbours(rank, rankVertices, neighbours, xQueue, yQueue, borderValues);		

		int myState = xQueue.empty() ? 1 : -1;

		//cout << rank << ": MEU ESTADO É " << myState << endl;
		
		// Se as filas xQueue e yQueue não estiverem vazias, continua processos
		if(myState == -1)
		{
			reconstructedImage = nscale::imreconstruct<unsigned char>(rank, borderValues, reconstructedImage, mskblock, 4, xQueue, yQueue);
			leftTopRightBotBorders = getBorders(reconstructedImage);
		}
		
		// On Stop condition, what it does:
			// Notificar rank 0 que não acabou, rank 0 deve então pedir para todos que acabaram
			// rodarem de novo, pq alguem não acabou.

			// Se estiverem vazias, notificar processo 0, que "Acabei"
			// esperar ser notificado pelo processo 0 que todos acabaram,
			// ou que devo tentar processar denovo
		if(stopCondition(rank, numeroDeProcessos, myState))
		{
			break;
		}

	}

	return reconstructedImage;
}



///////////////////////////////////////////////////////////////////////////////////
// Check Stop Condition
//////////////////////////////////////////////////////////////////////////////////
bool stopCondition(int rank, int numeroDeProcessos, int myState)
{
	bool shouldStop;

	if (rank == 0)
	{
		int finished = checkAllFinished(myState, rank, numeroDeProcessos);
		
		alertAllOtherProcesses(finished, rank, numeroDeProcessos);
		
		shouldStop = finished == 1 ? true : false;
		return shouldStop;
	}

	else
	{
		MPI_Send(&myState, 1, MPI_INT, 0, 42, MPI_COMM_WORLD);
		
		int finished; // 1 todos terminaram | -1 alguem nao terminou, tenho que rodar de novo
		MPI_Recv(&finished, 1, MPI_INT, 0, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		shouldStop = finished == 1 ? true : false;
		return shouldStop;
	}
	
}

int checkAllFinished(int myState, int rank, int numeroDeProcessos)
{
	vector<int> finished(numeroDeProcessos, 0);
	for(int i = 0; i < numeroDeProcessos; i++)
	{
		if(i == rank)
		{
			finished[i] = myState;
		}
		else
		{
			int other_process_state;
			MPI_Recv(&other_process_state, 1, MPI_INT, i, 42, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			finished[i] = other_process_state;
		}
	}
	
	int shouldStop = 1;
	for(int i=0; i<numeroDeProcessos; i++)
	{
		if(finished[i] == 0)
		{
			cout << "0 value in finished vector index " << i << " this is an error" << endl;
			cout << "One of the processes was not correctly marked as finished or not finished" << endl;
			throw std::exception();
		}

		else if(finished[i] == -1)
		{
			shouldStop = -1;
		}
	}

	return shouldStop;
}

void  alertAllOtherProcesses(int state, int rank, int numeroDeProcessos)
{
	for(int i=0; i < numeroDeProcessos; i++)
	{
		if(i != rank)
		{
			MPI_Send(&state, 1, MPI_INT, i, 42, MPI_COMM_WORLD);
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Receive Borders from neighbours
///////////////////////////////////////////////////////////////////////////////////
void addReceivedBorderCoordinatesToQueue(int rank, BoundBox rankVertices, BoundBox neighbour, Mat receivedBorder, std::queue<int> &xQueue, std::queue<int> &yQueue, std::queue<int> &borderValues, int side)
{
	// Iinitial pixel is used to determine where on the image the propagated border fits.

	// left
	if(side == 0)
	{
		int initialPixel = rankVertices.coordinateY > neighbour.coordinateY ? rankVertices.coordinateY : neighbour.coordinateY;
		initialPixel = initialPixel - rankVertices.coordinateY;
		
		for(int pointY = 0; pointY < receivedBorder.rows; pointY++)
		{
			for(int pointX = 0; pointX < receivedBorder.cols; pointX++)
			{
				int pixelValue = (int)receivedBorder.at<uchar>(pointY, pointX);
				//cout << "pixel" << "(" << pointX << ", "<< pointY << ")"<< endl;
				//cout << "  Value: " << pixelValue << endl;

				if(pixelValue == 255)
				{
					//cout << rank <<":	ADD top point (" << pointX << "," << pointY <<")" << endl;
					//cout << "	Value:"<< pixelValue << endl;
					xQueue.push(pointX);
					yQueue.push(pointY + initialPixel);
					borderValues.push(pixelValue);

				}
			}
		}
	}

	// top
	else if(side == 1)
	{
		int initialPixel = rankVertices.coordinateX > neighbour.coordinateX ? rankVertices.coordinateX : neighbour.coordinateX;
		initialPixel = initialPixel - rankVertices.coordinateX;

		for(int pointY = 0; pointY < receivedBorder.rows; pointY++)
		{
			for(int pointX = 0; pointX < receivedBorder.cols; pointX++)
			{
				int pixelValue = (int)receivedBorder.at<uchar>(pointY, pointX);
				//cout << "pixel" << "(" << pointX << ", "<< pointY << ")"<< endl;
				//cout << "  Value: " << pixelValue << endl;

				if(pixelValue == 255)
				{
					//cout << rank <<":	ADD top point (" << pointX << "," << pointY <<")" << endl;
					//cout << "	Value:"<< pixelValue << endl;
					xQueue.push(pointX + initialPixel);
					yQueue.push(pointY);
					borderValues.push(pixelValue);

				}
			}
		}
	}

	// right
	else if(side == 2)
	{
		int initialPixel = rankVertices.coordinateY > neighbour.coordinateY ? rankVertices.coordinateY : neighbour.coordinateY;
		initialPixel = initialPixel - rankVertices.coordinateY;
		
		for(int pointY = 0; pointY < receivedBorder.rows; pointY++)
		{
			for(int pointX = 0; pointX < receivedBorder.cols; pointX++)
			{
				int pixelValue = (int)receivedBorder.at<uchar>(pointY, pointX);
				//cout << "pixel" << "(" << pointX + rankVertices.edgeX<< ", "<< pointY << ")"<< endl;
				//cout << "  Value: " << pixelValue << endl;

				if(pixelValue == 255)
				{
					//cout << rank <<":	ADD top point (" << pointX + rankVertices.edgeX << "," << pointY <<")" << endl;
					//cout << "	Value:"<< pixelValue << endl;
					xQueue.push(pointX + rankVertices.edgeX);
					yQueue.push(pointY + initialPixel);
					borderValues.push(pixelValue);

				}
			}
		}
	}

	// bot
	else if(side == 3)
	{
		int initialPixel = rankVertices.coordinateX > neighbour.coordinateX ? rankVertices.coordinateX : neighbour.coordinateX;
		initialPixel = initialPixel - rankVertices.coordinateX;
		
		for(int pointY = 0; pointY < receivedBorder.rows; pointY++)
		{
			for(int pointX = 0; pointX < receivedBorder.cols; pointX++)
			{
				int pixelValue = (int)receivedBorder.at<uchar>(pointY, pointX);
				//cout << "pixel" << "(" << pointX << ", "<< pointY << ")"<< endl;
				//cout << "  Value: " << pixelValue << endl;

				if(pixelValue == 255)
				{
					//cout << rank <<":	ADD bot point (" << pointX << "," << pointY+ rankVertices.edgeY <<")" << endl;
					//cout << "	Value:"<< pixelValue << endl;
					xQueue.push(pointX + initialPixel);
					yQueue.push(pointY + rankVertices.edgeY);
					borderValues.push(pixelValue);

				}
			}
		}

	}

	else
	{
		cout << "side value of " << side << " was passed." << endl;
		cout << " addReceivedBorderCoordinatesToQueue Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
		throw std::exception();
	}

}


void receiveBordersFromNeighbours(int rank, BoundBox rankVertices, vector<vector<BoundBox>> neighbours, std::queue<int> &xQueue, std::queue<int> &yQueue, std::queue<int> &borderValues)
{
	int lengthOfNeighbours = neighbours.size();
	//int bordaIndicator;
	for (int i = 0; i < lengthOfNeighbours; i++)
	{
		int listOfSideOfNeighboursLength = neighbours[i].size(); 
		for(int j = 0; j <listOfSideOfNeighboursLength; j++)
		{
			int recebimentoBorda;
			Mat receivedBorder;
			MPI_Recv(&recebimentoBorda, 1, MPI_INT, neighbours[i][j].rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//cout << rank <<": recebimentoBorda = " << recebimentoBorda << endl;
			if (recebimentoBorda == 1)
			{
				receivedBorder = matrcv(neighbours[i][j].rank, 0);
				//cout << "Rank: " << rank << " received border from Rank: " << neighbours[i][j].rank << endl;
			
				addReceivedBorderCoordinatesToQueue(rank, rankVertices, neighbours[i][j], receivedBorder, xQueue, yQueue, borderValues, i);
				
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////
// Send Borders to neighbours
///////////////////////////////////////////////////////////////////////////////////
void sendBorderToNeighbours(vector<Mat> previousBorders, vector<Mat> leftTopRightBotBorders, vector<vector<BoundBox>> neighbours, BoundBox rankVertices)
{
	int lengthOfNeighbours = neighbours.size();
	int bordaIndicator;
	for (int i = 0; i < lengthOfNeighbours; i++)
	{
		int listOfSideOfNeighboursLength = neighbours[i].size(); 
		for(int j = 0; j <listOfSideOfNeighboursLength; j++)
		{
			Mat border = cutIntersection(leftTopRightBotBorders[i], rankVertices, neighbours[i][j], i);

			// Primeiro envio de borda
			if (previousBorders.empty())
			{
				// Envia aviso de envio de borda, um inteiro = 1
				//cout << "Condição 0, Try to send to " << neighbours[i] << endl;
				bordaIndicator = 1;
				MPI_Send(&bordaIndicator, 1, MPI_INT, neighbours[i][j].rank, 0, MPI_COMM_WORLD);
			
				// Envia borda
				matsnd(border, neighbours[i][j].rank, 0);
				//cout << "*** Im here ***" << endl;
			}
			//Não enviei a borda atual ainda, envia borda
			else if (!imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i]))
			{
				//cout << "Condição 1, Try to send to " << neighbours[i] << endl;
				// Envia aviso de envio de borda, um inteiro = 1
				bordaIndicator = 1;
				MPI_Send(&bordaIndicator, 1, MPI_INT, neighbours[i][j].rank, 0, MPI_COMM_WORLD);
				// Envia borda
				matsnd(border, neighbours[i][j].rank, 0);
			}
			// Mesma borda de antes, enviar valor indicando que não houve mudança
			else if (imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i]))
			{
				//cout << "Condição 2, Try to send to " << neighbours[i] << endl;
				// Envia aviso de não envio de borda, um inteiro = 0
				bordaIndicator = 0;
				MPI_Send(&bordaIndicator, 1, MPI_INT, neighbours[i][j].rank, 0, MPI_COMM_WORLD);
			}
			
		}
	}
}

// Find start and end of an intersection on the border
void getIntersection(int coordinate, int edge, int neighbourCoordinate, int neighbourEdge, int &start, int &size)
{
	int endCoordinate = coordinate + edge;
	int endNeighbourCoordinate = neighbourCoordinate + neighbourEdge;

	//cout << "finding intersection between: " << coordinate << " to " << endCoordinate <<endl;
	//cout << "	and " << neighbourCoordinate << " to " << endNeighbourCoordinate <<	endl;
	
	
	start = coordinate > neighbourCoordinate ? coordinate : neighbourCoordinate;
	int end = endCoordinate < endNeighbourCoordinate ? endCoordinate : endNeighbourCoordinate;

	// Maps the originals to a start point at 0, to use on the border image
	size = end - start;
	start = start - coordinate;

	//cout << "		Conclusion: " << start << " to " << end - coordinate << " size: " << size <<endl;
}

// Get intersection exemple
/*
|   0
| | 1  -> 1 - 0 = 1
| | 2
| | 3
| | 4  -> 4 - 0 = 4 
  | 5

|   0
| | 1  -> 1 - 0 = 1
| | 2
| | 3  -> 3 - 0 = 3
| 	4


on the border, cut from 1 to 4.
*/

// Cut out the intersection from the border to send to the neighnour
Mat cutIntersection(Mat borderToCut, BoundBox rankVertice, BoundBox neighbour, int side)
{
	//cout << "border being cut: " << endl;
	//for(int i =0; i < borderToCut.cols; i++)
	//{
	//	for(int j =0; j < borderToCut.rows; j++)
	//	{
	//		cout << "(" << i << ", " << j << ") ";
	//	}
	//	cout << endl;
	//}
	//cout << " "  << borderToCut << endl << endl;

	Mat cutBorder;
	
	if (side == 0) // left border
	{
		int start, size;
		getIntersection(rankVertice.coordinateY, rankVertice.edgeY, neighbour.coordinateY, neighbour.edgeY, start, size);
	//	cout<< "Left: " << "Start "<< start << " End: " << end << " end-start: " << end - start  <<" vs " <<  borderToCut.rows << endl;
		cutBorder = borderToCut(Rect(0, start, 1, size));
	}

	else if (side == 1) // top border
	{
		int start, size;
		getIntersection(rankVertice.coordinateX, rankVertice.edgeX, neighbour.coordinateX, neighbour.edgeX, start, size);
	//	cout << "Top: "<< "Start "<< start << " End: " << end << " end-start: " << end - start  <<" vs " <<  borderToCut.cols << endl;
		cutBorder = borderToCut(Rect(start, 0, size, 1));
	}

	else if (side == 2) // right border
	{
		int start, size;
		getIntersection(rankVertice.coordinateY, rankVertice.edgeY, neighbour.coordinateY, neighbour.edgeY, start, size);
	//	cout << "Right: "<< "Start "<< start << " End: " << end << " end-start: " << end - start  <<" vs " <<  borderToCut.rows << endl;
		cutBorder = borderToCut(Rect(0, start, 1, size));

	}

	else if (side == 3) // bot border
	{
		int start, size;
		getIntersection(rankVertice.coordinateX, rankVertice.edgeX, neighbour.coordinateX, neighbour.edgeX, start, size);
	//	cout << "Bot: "<< "Start "<< start << " End: " << end << " end-start: " << end - start  <<" vs " <<  borderToCut.cols << endl;
		cutBorder = borderToCut(Rect(start, 0, size, 1));
	}

	else
	{
		cout << " CutIntersection Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
		throw std::exception();
	}
	
	return cutBorder;
}


///////////////////////////////////////////////////////////////////////////////////
// Border extraction functions
///////////////////////////////////////////////////////////////////////////////////
vector<Mat> getBorders(Mat reconstructedImage)
{
	vector<Mat> borders(4);
	int bordersLength = borders.size();
	for (int i = 0; i < bordersLength; i++)
	{
		borders[i] = extractBorders(reconstructedImage, i);
	}

	return borders;
}

Mat extractBorders(Mat image, int side)
{
	Mat border;

	if (side == 0) // left border
	{
		border = image(Rect(0, 0, 1, image.rows));
	}

	else if (side == 1) // top border
	{
		border = image(Rect(0, 0, image.cols, 1));
	}

	else if (side == 2) // right border
	{
		border = image(Rect(image.cols - 1, 0, 1, image.rows));
	}

	else if (side == 3) // bot border
	{
		border = image(Rect(0, image.rows - 1, image.cols, 1));
	}

	else
	{
		cout << " extractBorders Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
		throw std::exception();
	}

	return border;
}


///////////////////////////////////////////////////////////////////////////////////
// Find all neighbours on one side, choose side with the int side parameter
///////////////////////////////////////////////////////////////////////////////////
vector<BoundBox> isThereANeighbour(BoundBox rankVertices, vector<BoundBox> rankNeighbours, int side)
{
	int x_left_i = rankVertices.coordinateX;
	int x_right_i = rankVertices.coordinateX + rankVertices.edgeX;
	int y_up_i = rankVertices.coordinateY;
	int y_bot_i = rankVertices.coordinateY + rankVertices.edgeY;

	int rankNeighboursLength = rankNeighbours.size();

	vector<BoundBox> leftNeighbours;
	vector<BoundBox> topNeighbours;
	vector<BoundBox> rightNeighbours;
	vector<BoundBox> botNeighbours;



	if (side == 0) // left border
	{
		for (int i = 0; i < rankNeighboursLength; i++)
		{
			int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;

			int y_up_j = rankNeighbours[i].coordinateY;
			int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;

			if (CheckIfNeighbour(x_left_i, x_left_i, x_right_j, x_right_j) &&
				CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j))
			{
				leftNeighbours.push_back(rankNeighbours[i]);
			}
		}
		
		return leftNeighbours;
	}

	else if (side == 1) // top border
	{
		for (int i = 0; i < rankNeighboursLength; i++)
		{
			int x_left_j = rankNeighbours[i].coordinateX;
			int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;

			int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;

			if (CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
				CheckIfNeighbour(y_up_i, y_up_i, y_bot_j, y_bot_j))
			{
				topNeighbours.push_back(rankNeighbours[i]);
			}
		}

		return topNeighbours;
	}

	else if (side == 2) // right border
	{
		for (int i = 0; i < rankNeighboursLength; i++)
		{
			int x_left_j = rankNeighbours[i].coordinateX;

			int y_up_j = rankNeighbours[i].coordinateY;
			int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;

			if (CheckIfNeighbour(x_right_i, x_right_i, x_left_j, x_left_j) &&
				CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j))
			{
				rightNeighbours.push_back(rankNeighbours[i]);
			}
		}

		return rightNeighbours;	
	}

	else if (side == 3) // bot border
	{
		for (int i = 0; i < rankNeighboursLength; i++)
		{
			int x_left_j = rankNeighbours[i].coordinateX;
			int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;

			int y_up_j = rankNeighbours[i].coordinateY;

			if (CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
				CheckIfNeighbour(y_bot_i, y_bot_i, y_up_j, y_up_j))
			{
				botNeighbours.push_back(rankNeighbours[i]);
			}
		}

		return botNeighbours;	
	}

	else
	{
		cout << " isThereANeighbour Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
		throw std::exception();
	}
}


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
// Aux function, use to print neighbours, rank indicates current node/process
void printNeighbours(int rank, vector<vector<BoundBox>> neighbours)
{
	for (int i = 0; i < 4; i++)
	{
		cout << "Rank: " << rank << "  Neighbour[" << i << "]: " << endl;
		int n_size = neighbours[i].size();
		vector<string> lados = {"left", "top", "right", "bottom"};
		
		cout << "	" << lados[i] << " vizinho(s):" << endl;
		for(int h = 0; h < n_size; h++)
		{
			cout << "		 " <<  "neighbours[" << lados[i] << "][" << h << "].rank: " <<
				neighbours[i][h].rank << endl;
			
			cout << "		 " <<  "neighbours[" << lados[i] << "][" << h << "].coordinateX: " <<
				neighbours[i][h].coordinateX << endl;
			
			cout << "		 " <<  "neighbours[" << lados[i] << "][" << h << "].coordinateY: " <<
				neighbours[i][h].coordinateY << endl;
			
			cout << "		 " <<  "neighbours[" << lados[i] << "][" << h << "].edgeX: " <<
				neighbours[i][h].edgeX << endl;
			
			cout << "		 " <<  "neighbours[" << lados[i] << "][" << h << "]edgeY: " <<
				neighbours[i][h].edgeY << endl;
			
		}
	}
}