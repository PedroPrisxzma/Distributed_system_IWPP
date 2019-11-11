#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "morphological_algorithms/MorphologicOperations.h"
#include "image_handling/ImageSeparator.h"
#include "classes/imageChunk.h"
#include "classes/vertices.h"

typedef struct boundBox
{
    int x, y, sizex, sizey;
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
    Rect *imgblock;

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

        int sizeToSend;
        for(int i=0; i<numeroDeProcessos; i++)
        {
            vert_list[i].x = imageBlocks.vetorDeVertices[i].leftSide; 
            vert_list[i].y = imageBlocks.vetorDeVertices[i].topSide; 
            vert_list[i].sizex = imageBlocks.vetorDeVertices[i].sizex; 
            vert_list[i].sizey = imageBlocks.vetorDeVertices[i].sizey; 
        }
        for(int i=1; i<numeroDeProcessos; i++)
        {
            MPI_Send(&vert_list, 4*numeroDeProcessos, MPI_INT, i, 0, MPI_COMM_WORLD);

            sizeToSend = imageBlocks.vetorDeImagens[i].area();

            // Envia o tamanho dos dados
            MPI_Send(&sizeToSend, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // Envia os dados
            MPI_Send(&imageBlocks.vetorDeImagens[i], sizeToSend, MPI_BYTE, i, 0, MPI_COMM_WORLD);
        }

        imgblock = imageBlocks.vetorDeImagens[0];
    }
    else
    {
        MPI_Recv(&vert_list, 6*numeroDeProcessos, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int sizeToRecive;
        // Recebe o tamanho dos dados
        MPI_Recv(&sizeToRecive, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Recebe os dados
        MPI_Recv(&imgblock, sizeToRecive, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // TODO: fazer algo com imageChunk 

    //cout << imageChunk.vetorDeVertices << endl << imageChunk.vetorDeImagens << endl;
    

    // Finaliza programacao distribuida
    MPI_Finalize();
    return 0;
}