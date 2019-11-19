#include <stdlib.h>

#include <iostream>
#include <queue>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "morphological_algorithms/MorphologicOperations.h"
#include "image_handling/ImageSeparator.h"
#include "image_handling/SendReceiveImage.h"
#include "image_handling/FindNeighbour.h"
#include "classes/imageChunk.h"
#include "classes/vertices.h"

int main(int argc, char *argv[])
{
   // **** Testar o arquivo nscale/src/segment/test/src/imreconTest.cpp  ****
    // Para entender a reconstrucao morfologica

    // Separate the image, must be flexible,
    // allowing to separate into same size or diferent sized smaller chunks

    // Store references to the image's corners
    // Neighbours vector boundbox
    // Send borders interssections 

    // Allocate MPI object
    // Send the image to the available machines, to execute the IWPP
    // When hiting a corner must comunicate with it's neighbours


    //Stop condition, possibilities:
    // Either broadcast to all nodes
    // Broadcast to a node (in a ring like fashion) until all have finished
        
    int numeroDeProcessos, rank;
    int data_size;
    // ImageChunk imageChunk;
    Mat imgblock;
    Mat mskblock;

    // Inicia programacao distribuida
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numeroDeProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    BoundBox* vert_list;
    vector<BoundBox> rankNeighbours;
    vector<vector<BoundBox>> vizinhos; // Lista de BoundBox listas       


    if (rank == 0)
    {
        if (argc < 3)
        {
            cout << " Usage: mpiexec -n <process_number> ./main <marker> <mask>" << endl;
            throw std::exception();
        }

        Mat inputImage = image_reader(argv[1]);
        Mat inputMask = image_reader(argv[2]);
        cout << inputImage.size << endl;

        ImageChunk imageBlocks = separate_image(inputImage, inputMask, numeroDeProcessos);

        vert_list = (BoundBox*)malloc(numeroDeProcessos*sizeof(BoundBox));

        for(int i=0; i<numeroDeProcessos; i++)
        {
            vert_list[i].coordinateX = imageBlocks.vetorDeVertices[i].coordinateX; 
            vert_list[i].coordinateY = imageBlocks.vetorDeVertices[i].coordinateY; 
            vert_list[i].edgeX = imageBlocks.vetorDeVertices[i].edgeX; 
            vert_list[i].edgeY = imageBlocks.vetorDeVertices[i].edgeY; 
            vert_list[i].rank = i; 
        }

        // Acha vizinhos dos Chunks de imagens
        vizinhos = FindNeighbours(vert_list, numeroDeProcessos, 4);
        cout << "---------------------------------" << endl;
        for(int i = 0; i < numeroDeProcessos; i++)
        {
            cout << "Vizinhos de: " << i << endl;
            cout << "I -> cX: "  << vert_list[i].coordinateX << " cY: " << vert_list[i].coordinateY << " eX: " << vert_list[i].edgeX << " eY: " << vert_list[i].edgeY<< endl;
            cout << " --------------------------- " << vizinhos[i].size() << endl;
            for(int j = 0; j < vizinhos[i].size(); j++)
            {
                cout << "J -> cX: "  << vizinhos[i][j].coordinateX << " cY: " << vizinhos[i][j].coordinateY << " eX: " << vizinhos[i][j].edgeX << " eY: " << vizinhos[i][j].edgeY<< endl;
                cout << endl;
            }
        }

        for(int i=1; i<numeroDeProcessos; i++)
        {   
            // Envia o vetor de boundBoxes
            // MPI_Send(&vert_list, 5*numeroDeProcessos, MPI_INT, i, 0, MPI_COMM_WORLD);
            data_size = vizinhos[i].size();
            
            MPI_Send(&data_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&vizinhos[i], data_size, MPI_INT, i, 0, MPI_COMM_WORLD);

            //// Envia as imagens
            matsnd(imageBlocks.vetorDeImagens[i], i);
            matsnd(imageBlocks.vetorDeMascaras[i], i);
            
        }

        // Esse free não pode estar aqui
        //free(vert_list);

        rankNeighbours = vizinhos[0];
        imgblock = Mat(imageBlocks.vetorDeImagens[0]).clone();
        mskblock = Mat(imageBlocks.vetorDeMascaras[0]).clone();
    }
    else
    {
        // Recebe o vetor de boundBoxes
        cout << "PING !!" << endl;
        MPI_Recv(&data_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        cout << "PONG ??" << endl;

        rankNeighbours.resize(data_size);
        
        MPI_Recv(&rankNeighbours[0], data_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        
        // Recebe as imagens
        imgblock = matrcv(0);
        mskblock = matrcv(0);
    }
    
    //cout << "rank" << rank << "  " 
    // << vert_list[rank].coordinateX << "  " 
    // << vert_list[rank].coordinateY << "  " 
    // << vert_list[rank].edgeX << "  "   
    // << vert_list[rank].edgeY << endl; 

    imshow("image", imgblock);
    waitKey();
    // imshow("image", mskblock);
    // waitKey();
        
    //cout << imageChunk.vetorDeVertices << endl << imageChunk.vetorDeImagens << endl;
        

    //////////////////////////////////////////////////////////////////////////////////////
	// Morphological alg
    std::queue<int> xQ;
    std::queue<int> yQ;
    // Mat recon = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4);
    Mat recon = nscale::imreconstruct<unsigned char>(imgblock, mskblock, 4, xQ, yQ);
    
    int qSize = xQ.size();
    cout << qSize << " elements of rank: " << rank << endl;
    for (int q = 0; q < qSize; q++)
    {
        cout << "X: " << xQ.front() << " Y: " << yQ.front() << endl;
        xQ.pop();
        yQ.pop();
    }
    
    
    // imshow("image", recon);
    // waitKey();
    // Finaliza programacao distribuida
    MPI_Finalize();
    return 0;
}


// Perguntar:
    // Ao separar os blocos da imagem, devem ser referencias ou copias?
    // Ao terminar, devo juntar os blocos processados numa imagem?

    // ---> tem que ser cópia, referencia não funciona. Ao terminar, tem que juntar de volta a imagem.