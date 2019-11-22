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
    //int coordinateX, coordinateY;
    
    std::queue<int> xQueue;
    std::queue<int> yQueue;

    //vector<vector<int>> sendListX(rankNeighbours.size());
    //vector<vector<int>> sendListY(rankNeighbours.size());
    
    //vector<int> reciveListX(rankNeighbours.size());
    //vector<int> reciveListY(rankNeighbours.size());
    
    //int xLeft, xRigth, yTop, yBotton, sendRank;

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
        
        vector<int> neighbours = {leftNeighbour, topNeighbour, rightNeighbour, botNeighbour};
        
        for(int i =0; i < 4; i++)
        {
            cout<< "Rank: " << rank << "  Neighbour["<< i <<"]: " <<neighbours[i]<< endl; 
        }
        
        //int size = rankNeighbours.size();
        //for(int i =0; i < size; i++)
        //{
        //    cout<< "Rank: " << rank << "  rankNeighbour["<< i <<"].rank: " << rankNeighbours[i].rank << endl;
        //    cout<< "    Rank: " << rank << "  rankNeighbour["<< i <<"].coordenadaX: " << rankNeighbours[i].coordinateX << endl;
        //    cout<< "    Rank: " << rank << "  rankNeighbour["<< i <<"].coordenadaY: " << rankNeighbours[i].coordinateY << endl;
        //    cout<< "    Rank: " << rank << "  rankNeighbour["<< i <<"].edgeX: " << rankNeighbours[i].edgeX << endl;
        //    cout<< "    Rank: " << rank << "  rankNeighbour["<< i <<"].edgeY: " << rankNeighbours[i].edgeY << endl;       
        //}

        vector<Mat> previousBorders;

        // Aqui envia a borda para o vizinho, no caso envia a img em formato MAT 
        // (é literalmente os Z pixéis da imagem na borda), vamos tratar eles depois do recv.
        sendBorderToNeighbours(previousBorders, leftTopRightBotBorders, neighbours);

        // Update last sent borders
        int leftTopRightBotBordersLenght = leftTopRightBotBorders.size();
        for(int i =0; i < leftTopRightBotBordersLenght; i++)
        {
            previousBorders.push_back(leftTopRightBotBorders[i]);
        }

        // Receive bordas, 
        if(leftNeighbour != -1)
        {
            int recebimentoBorda;
            Mat leftBorder;
            MPI_Recv(&recebimentoBorda, sizeof(int), MPI_INT, leftNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
             cout << "Left, recebimentoBorda: " << recebimentoBorda << endl;
            if(recebimentoBorda == 1)
            {
                leftBorder = matrcv(leftNeighbour, 0);
                cout << "Rank: " << rank << " received Left border from Rank: "<< leftNeighbour << endl;
                imshow("leftBorder received by "+to_string(rank) + " from Rank: " +to_string(leftNeighbour), leftBorder);
                waitKey();
                // TODO:
                // tratar pixeis, inserindo nas Queues
                // Atentar para posição (valor x e y) que o pixel deve ter na imagem
            }
        }

        if(topNeighbour != -1)
        {
            int recebimentoBorda;
            Mat topBorder;
            MPI_Recv(&recebimentoBorda, sizeof(int), MPI_INT, topNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                topBorder = matrcv(topNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
                // Atentar para posição (valor x e y) que o pixel deve ter na imagem            
            }
        }

        if(rightNeighbour != -1)
        {
            int recebimentoBorda;
            Mat rightBorder;
            MPI_Recv(&recebimentoBorda, sizeof(int), MPI_INT, rightNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                rightBorder = matrcv(rightNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
                // Atentar para posição (valor x e y) que o pixel deve ter na imagem
            }
        }
        
        if(botNeighbour != -1)
        {
            int recebimentoBorda;
            Mat botBorder;
            MPI_Recv(&recebimentoBorda, sizeof(int), MPI_INT, botNeighbour, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if(recebimentoBorda == 1)
            {
                botBorder = matrcv(botNeighbour, 0);
                // TODO:
                // tratar pixeis, inserindo nas Queues
                // Atentar para posição (valor x e y) que o pixel deve ter na imagem
            }
        }

        //TODO:
        // Fazer as chamadas abaixo com os dados atualizados
        
        // Se as filas xQueue e yQueue não estiverem vazias
            // reconstructedImage = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQueue, yQueue);
            // leftTopRightBotBorders = getBorders(reconstructedImage);
            // Notificar rank 0 que não acabou, rank 0 deve então pedir para todos que acabaram 
            // rodarem de novo, pq alguem não acabou.

        // Se estiverem vazias, notificar processo 0, que "Acabei"
            // esperar ser notificado pelo processo 0 que todos acabaram, 
            // ou que devo tentar processar denovo

        if(rank == 0)
        {
            // TODO:
            // Depois de feito os demais TODOs, Administrar fim no rank 0
        }

        //TODO: 
        // Remove break
        break;
    }

    return reconstructedImage;
}

void sendBorderToNeighbours(vector<Mat> previousBorders, vector<Mat> leftTopRightBotBorders, vector<int> neighbours)
{
    int neighboursLength = neighbours.size();
    for(int i=0; i < neighboursLength; i++)
    {
        // Primeiro envio de borda
        if(neighbours[i] != -1 && previousBorders.empty())
        {
            // Envia aviso de envio de borda, um inteiro = 1
            int bordaIndicator = 1;
            MPI_Send(&bordaIndicator, 1, MPI_INT, neighbours[i], 0, MPI_COMM_WORLD);

            // Envia borda
            matsnd(leftTopRightBotBorders[i], neighbours[i], 0);
        }
        
        //Não enviei a borda atual ainda, envia borda
        else if((neighbours[i] != -1) && (!imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i])) )
        {
            // Envia aviso de envio de borda, um inteiro = 1
            int bordaIndicator = 1;
            MPI_Send(&bordaIndicator, 1, MPI_INT, neighbours[i], 0, MPI_COMM_WORLD);

            // Envia borda
            matsnd(leftTopRightBotBorders[i], neighbours[i], 0);
        }

        // Mesma borda de antes, enviar valor indicando que não houve mudança
        else if((neighbours[i] != -1) && (imagesIsEqual(previousBorders[i], leftTopRightBotBorders[i])))
        {
            // Envia aviso de não envio de borda, um inteiro = 0
            MPI_Send(0, 1, MPI_INT, neighbours[i], 0, MPI_COMM_WORLD);
        }
    }
}


vector<Mat> getBorders(Mat reconstructedImage)
{
    vector<Mat> borders(4);
    int bordersLength = borders.size();
    for(int i = 0; i < bordersLength; i++)
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
    
    int rankNeighboursLength = rankNeighbours.size();

    if(side == 0)// left border
    {
        for(int i=0; i < rankNeighboursLength; i++)
        {
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
        for(int i=0; i < rankNeighboursLength; i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
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
        for(int i=0; i < rankNeighboursLength; i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            
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
        for(int i=0; i < rankNeighboursLength; i++)
        {
            int x_left_j = rankNeighbours[i].coordinateX;
            int x_right_j = rankNeighbours[i].coordinateX + rankNeighbours[i].edgeX;
            
            int y_up_j = rankNeighbours[i].coordinateY;

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


