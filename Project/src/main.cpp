#include <stdlib.h>

#include <iostream>
using namespace std;

#include <omp.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "morphological_algorithms/MorphologicOperations.h"
#include "image_handling/ImageSeparator.h"
#include <mpi.h>

int main(int argc, char *argv[])
{
    // Separate the image, must be flexible,
    // allowing to separate into same size or diferent sized smaller chunks

    // Store references to the image's corners

    // Send the image to the available machines, to execute the IWPP
    // When hiting a corner must comunicate with it's neighbours

    //Stop condition, possibilities:
    // Either broadcast to all nodes
    // Broadcast to a node (in a ring like fashion) until all have finished
    int n_procs, rank;

    // Inicia programacao distribuida
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        if (argc < 3)
        {
            cout << " Usage: executable ImageToLoad, no ImageToLoad was provided" << endl;
            throw std::exception();
        }

        Mat inputImage;
        inputImage = image_reader(argv[1]);
        cout << inputImage.size << endl;

        vector<React> imageBlocks = separate_image(inputImage, atoi(argv[2]));
    }

    // Finaliza programacao distribuida
    MPI_Finalize();
    return 0;
}