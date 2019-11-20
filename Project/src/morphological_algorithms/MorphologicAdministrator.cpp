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
Mat imReconstructAdm(Mat imgblock, Mat mskblock, vector<BoundBox> rankNeighbours, int rank)
{
    std::queue<int> xQueue;
    std::queue<int> yQueue;

    Mat recon = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);
    
    int queueSize = xQueue.size();
    cout << queueSize << " elements of rank: " << rank << endl;
    for (int q = 0; q < queueSize; q++)
    {
        cout << "X: " << xQueue.front() << " Y: " << yQueue.front() << endl;
        xQueue.pop();
        yQueue.pop();
    }

    return recon;
}