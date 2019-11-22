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

    MPI_Request request;
    MPI_Status status;
    int coordinateX, coordinateY;
    
    std::queue<int> xQueue;
    std::queue<int> yQueue;

    vector<vector<int>> sendListX(rankNeighbours.size());
    vector<vector<int>> sendListY(rankNeighbours.size());
    
    vector<int> reciveListX(rankNeighbours.size());
    vector<int> reciveListY(rankNeighbours.size());
    
    int xLeft, xRigth, yTop, yBotton, sendRank;

    // sendListX.resize(numeroDeProcessos);
    // sendListY.resize(numeroDeProcessos);

    Mat reconstructedImage;
    
    reconstructedImage = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);
    
    // Extract first time processing borders
    vector<Mat> leftTopRightBotBorders = getBorders(reconstructedImage);
    //0 is the left border, 1 is the top border, 2 is the right border and 3 is the bottom border
    
    while (true)
    {
        int leftNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 0);
        int topNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 1);
        int rightNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 2);
        int botNeighbour = isThereANeighbour(rankVertices, rankNeighbours, 3);
        
        vector<int> neighbours = [leftNeighbour, topNeighbour, rightNeighbour, botNeighbour];
        
        vector<Mat> previousBorders;

        if(rank == 0)
        {
            // TODO:
            // Depois de feito os demais TODOs, Administrar fim no rank 0
        }

        // Aqui envia a borda para o vizinho, no caso envia a img em formato MAT 
        // (é literalmente os Z pixéis da imagem na borda), vamos tratar eles depois do recv.
        sendBorderToNeighbours(previousBorders, leftTopRightBotBorders, neighbours);

        // Update last sent borders
        for(int i =0; i < leftTopRightBotBorders.size(); i++)
        {
            previousBorders.push_back(leftTopRightBotBorders[i])
        }

        // Receive bordas, 
        if(leftNeighbour != -1)
        {
            int recebimentoBorda;
            Mat leftBorder;
            MPI_Recv(recebimentoBorda, sizeof(int), MPI_INT, leftNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                leftBorder = matrcv(leftNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
            }
        }

        if(topNeighbour != -1)
        {
            int recebimentoBorda;
            Mat topBorder;
            MPI_Recv(recebimentoBorda, sizeof(int), MPI_INT, topNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                leftBorder = matrcv(topNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
            }
        }

        if(rightNeighbour != -1)
        {
            int recebimentoBorda;
            Mat rightBorder;
            MPI_Recv(recebimentoBorda, sizeof(int), MPI_INT, rightNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                leftBorder = matrcv(rightNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
            }
        }
        
        if(botNeighbour != -1)
        {
            int recebimentoBorda;
            Mat botBorder;
            MPI_Recv(recebimentoBorda, sizeof(int), MPI_INT, botNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                leftBorder = matrcv(botNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
            }
        }

        //TODO:
        // Fazer as chamadas abaixo com os dados atualizados
        //reconstructedImage = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);
        //leftTopRightBotBorders = getBorders(reconstructedImage);




//
    //    recon = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);

    //    int queueSize = xQueue.size();
    //    cout << queueSize << " elements of rank: " << rank << endl;
    //    for (int q = 0; q < queueSize; q++)
    //    {
    //        coordinateX = xQueue.front() + rankVertices.coordinateX;
    //        coordinateY = yQueue.front() + rankVertices.coordinateY;
    //        cout << "X: " << coordinateX << " Y: " << coordinateY << endl;
    //        xQueue.pop();
    //        yQueue.pop();
    //        for (int r = 0; r < rankNeighbours.size(); r++)
    //        {
    //            xLeft = rankNeighbours[r].coordinateX;
    //            xRigth = xLeft + rankNeighbours[r].edgeX;
    //            yTop = rankNeighbours[r].coordinateY;
    //            yBotton = yTop + rankNeighbours[r].edgeY;
    //            sendRank = rankNeighbours[r].rank;

    //            if (coordinateX >= xLeft && coordinateX <= xRigth &&
    //                coordinateY >= yTop && coordinateY <= yBotton)
    //            {
    //                sendListX[sendRank].push_back(coordinateX - xLeft);
    //                sendListY[sendRank].push_back(coordinateY - yTop);
    //                cout << "send x: " << sendListX[sendRank].back() << " y: " << sendListY[sendRank].back() << endl;
    //                break;
    //            }
    //        }
    //    }
    //    for (int srank = 0; srank < numeroDeProcessos; srank++)
    //    {
    //        if (srank == rank)
    //            continue;

    //        queueSize = sendListX[srank].size() * sizeof(int);
    //        cout << "queue size send: " << queueSize << " from: " << rank << " to: " << srank << endl;
    //        MPI_Send(&queueSize, 1, MPI_INT, srank, 0, MPI_COMM_WORLD);
    //        if (queueSize > 0)
    //        {
    //            MPI_Send(&sendListX[srank], queueSize, MPI_INT, srank, 0, MPI_COMM_WORLD);
    //            MPI_Send(&sendListY[srank], queueSize, MPI_INT, srank, 0, MPI_COMM_WORLD);
    //        }
    //    }
    //    for (int rrank = 0; rrank < numeroDeProcessos; rrank++)
    //    {
    //        if (rrank == rank)
    //            continue;

    //        MPI_Recv(&queueSize, 1, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //        cout << "queue size recive: " << queueSize << " from: " << rrank << " to: " << rank << endl;
    //        if (queueSize > 0)
    //        {
    //            reciveListX.resize(queueSize);
    //            reciveListY.resize(queueSize);
    //            MPI_Recv(&reciveListX[0], queueSize, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //            MPI_Recv(&reciveListY[0], queueSize, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //            cout << "queue x size recive: " << reciveListX.size() << endl;
    //            for (int i = 0; i < queueSize; i++)
    //            {
    //                cout << " x: " << reciveListX[i] << " y: " << reciveListY[i] << endl;
    //                xQueue.push(reciveListX[i]);
    //                yQueue.push(reciveListY[i]);
    //            }
    //        }
    //    }
    //    if (xQueue.size() == 0)
    //    {
    //        MPI_Wait(&request, &status);
    //        break;
    //    }
//    
    }

    return reconstructedImage;
}

void sendBorderToNeighbours(vector<Mat> previousBorders, vector<Mat> leftTopRightBotBorders, vector<int> neighbours)
{
    for(int i=0; i < neighbours.size(); i++)
    {
        // Primeiro envio de borda
        if(neighbours[i] != -1 && previousBorders.empty())
        {
            // Envia aviso de envio de borda, um inteiro = 1
            MPI_Send(1, sizeof(int), MPI_INT, neighbours[i], 0, MPI_COMM_WORLD);

            // Envia borda
            matsnd(leftTopRightBotBorders[i], neighbours[i], 0);
        }
        
        //Não enviei a borda atual ainda, envia borda
        else if((neighbours[i] != -1) && (!imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i])) )
        {
            // Envia aviso de envio de borda, um inteiro = 1
            MPI_Send(1, sizeof(int), MPI_INT, neighbours[i], 0, MPI_COMM_WORLD);

            // Envia borda
            matsnd(leftTopRightBotBorders[i], neighbours[i], 0);
        }

        // Mesma borda de antes, enviar valor indicando que não houve mudança
        else if((neighbours[i] != -1) && (imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i])))
        {
            // Envia aviso de não envio de borda, um inteiro = 0
            MPI_Send(0, sizeof(int), MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
}


vector<Mat> getBorders(Mat reconstructedImage)
{
    vector<Mat> borders(4);
    for(int i = 0; i < borders.size(); i++)
    {
        borders[i] = extractBorders(reconstructedImage, i);
    }

    return borders;
}

Mat extractBorders(Mat image, int side)
{
    Mat border;

    if(side == 0)// left border
    {
        border = image(Rect(0, 0, 1, image.rows));
    }

    else if(side == 1)// top border
    {
        border = image(Rect(0, 0, image.cols, 1));
    }

    else if(side == 2)// right border
    {
        border = image(Rect(image.cols - 1, 0, 1, image.rows));
    }

    else if(side == 3)// bot border
    {
        border = image(Rect(0, image.rows - 1, image.cols, 1));
    }

    else
    {
        cout << " Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
        throw std::exception();
    }
    
    return border;
}

int isThereANeighbour(BoundBox rankVertices, vector<BoundBox> rankNeighbours, int side)
{
    int x_left_i = rankVertices.coordinateX;
    int x_right_i = rankVertices.coordinateX + rankVertices.edgeX;
    int y_up_i = rankVertices.coordinateY;
    int y_bot_i = rankVertices.coordinateY + rankVertices.edgeY;   
    
    if(side == 0)// left border
    {
        for(int i=0; i < rankNeighbours.size(); i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
            int y_up_j = rankNeighbours[i].coordinateY;
            int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;            

            if(CheckIfNeighbour(x_left_i, x_left_i, x_right_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                return rankNeighbours[i].rank;
            }
        }
        return -1; 
    }

    else if(side == 1)// top border
    {
        for(int i=0; i < rankNeighbours.size(); i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
            int y_up_j = rankNeighbours[i].coordinateY;
            int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;            

            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_up_i, y_up_i, y_bot_j, y_bot_j) )
            {
                return rankNeighbours[i].rank;
            }
        }
        return -1; 
    }

    else if(side == 2)// right border
    {
        for(int i=0; i < rankNeighbours.size(); i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
            int y_up_j = rankNeighbours[i].coordinateY;
            int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;            

            if(CheckIfNeighbour(x_right_i, x_right_i, x_left_j, x_left_j) &&
               CheckIfNeighbour(y_up_i, y_bot_i, y_up_j, y_bot_j) )
            {
                return rankNeighbours[i].rank;
            }
        }
        return -1; 
    }

    else if(side == 3)// bot border
    {
        for(int i=0; i < rankNeighbours.size(); i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
            int y_up_j = rankNeighbours[i].coordinateY;
            int y_bot_j = rankNeighbours[i].coordinateY + rankNeighbours[i].edgeY;            

            if(CheckIfNeighbour(x_left_i, x_right_i, x_left_j, x_right_j) &&
               CheckIfNeighbour(y_bot_i, y_bot_i, y_up_j, y_up_j) )
            {
                return rankNeighbours[i].rank;
            }
        }
        return -1;
    }

    else
    {
        cout << " Invalid side was passed, use 0 for left, 1 for top, 2 for right and 3 for bottom." << endl;
        throw std::exception();
    }
}


