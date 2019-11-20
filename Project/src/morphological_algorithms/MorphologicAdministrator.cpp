#include <algorithm>
#include <queue>
#include <limits>
#include <list>
#include <omp.h>
#include <iostream>
#include <mpi.h>

#include "../classes/imageChunk.h"
#include "../image_handling/SendReceiveImage.h"
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
    std::queue<int> xQueue;
    std::queue<int> yQueue;
    int coordinateX, coordinateY;
    vector<vector<int>> sendListX(numeroDeProcessos);
    vector<vector<int>> sendListY(numeroDeProcessos);
    vector<int> reciveListX;
    vector<int> reciveListY;
    // sendListX.resize(numeroDeProcessos);
    // sendListY.resize(numeroDeProcessos);

    int xLeft, xRigth, yTop, yBotton, sendRank;
    Mat recon;

    while (true)
    {

        recon = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);

        int queueSize = xQueue.size();
        cout << queueSize << " elements of rank: " << rank << endl;
        for (int q = 0; q < queueSize; q++)
        {
            coordinateX = xQueue.front() + rankVertices.coordinateX;
            coordinateY = yQueue.front() + rankVertices.coordinateY;
            cout << "X: " << coordinateX << " Y: " << coordinateY << endl;
            xQueue.pop();
            yQueue.pop();
            for (int r = 0; r < rankNeighbours.size(); r++)
            {
                xLeft = rankNeighbours[r].coordinateX;
                xRigth = xLeft + rankNeighbours[r].edgeX;
                yTop = rankNeighbours[r].coordinateY;
                yBotton = yTop + rankNeighbours[r].edgeY;
                sendRank = rankNeighbours[r].rank;

                if (coordinateX >= xLeft && coordinateX <= xRigth &&
                    coordinateY >= yTop && coordinateY <= yBotton)
                {
                    sendListX[sendRank].push_back(coordinateX - xLeft);
                    sendListY[sendRank].push_back(coordinateY - yTop);
                    cout << "send x: " << sendListX[sendRank].back() << " y: " << sendListY[sendRank].back() << endl;
                    break;
                }
            }
        }
        for (int srank = 0; srank < numeroDeProcessos; srank++)
        {
            if (srank == rank)
                continue;

            queueSize = sendListX[srank].size() * sizeof(int);
            cout << "queue size send: " << queueSize << " from: " << rank << " to: " << srank << endl;
            MPI_Send(&queueSize, 1, MPI_INT, srank, 0, MPI_COMM_WORLD);
            if (queueSize > 0)
            {
                MPI_Send(&sendListX[srank], queueSize, MPI_INT, srank, 0, MPI_COMM_WORLD);
                MPI_Send(&sendListY[srank], queueSize, MPI_INT, srank, 0, MPI_COMM_WORLD);
            }
        }
        for (int rrank = 0; rrank < numeroDeProcessos; rrank++)
        {
            if (rrank == rank)
                continue;

            MPI_Recv(&queueSize, 1, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            cout << "queue size recive: " << queueSize << " from: " << rrank << " to: " << rank << endl;
            if (queueSize > 0)
            {
                reciveListX.resize(queueSize);
                reciveListY.resize(queueSize);
                MPI_Recv(&reciveListX[0], queueSize, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&reciveListY[0], queueSize, MPI_INT, rrank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                cout << "queue x size recive: " << reciveListX.size() << endl;
                for (int i = 0; i < queueSize; i++)
                {
                    cout << " x: " << reciveListX[i] << " y: " << reciveListY[i] << endl;
                    xQueue.push(reciveListX[i]);
                    yQueue.push(reciveListY[i]);
                }
            }
        }
        if (xQueue.size() == 0)
        {
            MPI_Wait(&request, &status);
            break;
        }
    }

    return recon;
}