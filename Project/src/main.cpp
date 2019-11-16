#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "morphological_algorithms/MorphologicOperations.h"
#include "image_handling/ImageSeparator.h"
#include "image_handling/SendReceiveImage.h"
#include "classes/imageChunk.h"
#include "classes/vertices.h"

typedef struct boundBox
{
    int coordinateX, coordinateY, edgeX, edgeY;
} BoundBox; 


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
    // ImageChunk imageChunk;
    Mat imgblock;

    // Inicia programacao distribuida
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numeroDeProcessos);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    BoundBox vert_list[numeroDeProcessos];

    if (rank == 0)
    {
        if (argc < 2)
        {
            cout << " Usage: mpiexec -n <process_number> ./main <image>" << endl;
            throw std::exception();
        }

        Mat inputImage;
        inputImage = image_reader(argv[1]);
        cout << inputImage.size << endl;

        ImageChunk imageBlocks = separate_image(inputImage, numeroDeProcessos);

        for(int i=0; i<numeroDeProcessos; i++)
        {
            vert_list[i].coordinateX = imageBlocks.vetorDeVertices[i].coordinateX; 
            vert_list[i].coordinateY = imageBlocks.vetorDeVertices[i].coordinateY; 
            vert_list[i].edgeX = imageBlocks.vetorDeVertices[i].edgeX; 
            vert_list[i].edgeY = imageBlocks.vetorDeVertices[i].edgeY; 
        }

        for(int i=1; i<numeroDeProcessos; i++)
        {
            // Envia o vetor de boundBoxes
            MPI_Send(&vert_list, 4*numeroDeProcessos, MPI_INT, i, 0, MPI_COMM_WORLD);

            //// Envia as imagens
            matsnd(imageBlocks.vetorDeImagens[i], i);
        }

        imgblock = Mat(imageBlocks.vetorDeImagens[0]).clone();
    }
    else
    {
        // Recebe o vetor de boundBoxes
        MPI_Recv(&vert_list, 4*numeroDeProcessos, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        // Recebe as imagens
        imgblock = matrcv(0);
    }

    cout << "rank" << rank << "  " 
     << vert_list[rank].coordinateX << "  " 
     << vert_list[rank].coordinateY << "  " 
     << vert_list[rank].edgeX << "  "   
     << vert_list[rank].edgeY << endl; 

    imshow("image", imgblock);
    waitKey();

    // TODO: fazer algo com imageChunk 

    //cout << imageChunk.vetorDeVertices << endl << imageChunk.vetorDeImagens << endl;
    

    // Finaliza programacao distribuida
    MPI_Finalize();
    return 0;
}


// Perguntar:
    // Ao separar os blocos da imagem, devem ser referencias ou copias?
    // Ao terminar, devo juntar os blocos processados numa imagem?

    // ---> tem que ser cópia, referencia não funciona. Ao terminar, tem que juntar de volta a imagem.