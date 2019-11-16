#include <stdlib.h>
#include <fstream>
#include <exception>
#include <memory>
#include <iostream>
using namespace std;
#include <mpi.h>

#include "../classes/vertices.h"
#include "../classes/imageChunk.h"

#include "ImageSeparator.h"
#include <opencv2/opencv.hpp>
using namespace cv;


// Read in the image
Mat image_reader(char *filename)
{
    
    Mat image;
    // image = imread(filename, IMREAD_COLOR);
    image = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    if (!image.data) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        throw std::exception();
    }

    //namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
    //imshow("Display window", image);                // Show our image inside it.

    //waitKey(0);

    return image;
}

// Separate the image into smaller chunks
ImageChunk separate_image(Mat image, int numProcessos)
{
    int width = image.cols;
    int height = image.rows;
    // int GRID_SIZE = 100;

    Vertices vertice(0, width, 0, height);

    ImageChunk vetorDeBlocos;

    //cout << "Image Details" << endl;
    //printf("Width: %d\nHeight: %d\n", width, height);

    slice_image(image, vertice, &vetorDeBlocos, numProcessos);

    // imshow("image", image);
    // waitKey();

    return vetorDeBlocos;
}

void slice_image(Mat image, Vertices vertices, ImageChunk *vetorDeBlocos, int numProcessos)
{
    //TODO:
    // Mudar para conter no máximo um número de divisões determinado pelo número de nós no sistema
    int factor, numProcessos_1, numProcessos_2;

    if (numProcessos == 0 || vertices.edgeX == 0 || vertices.edgeY == 0 )
        return;
    else if (numProcessos == 1)
    {
        Mat img_slice = image(Rect(vertices.coordinateX, vertices.coordinateY, vertices.edgeX, vertices.edgeY));
        vetorDeBlocos->vetorDeImagens.push_back(img_slice);
        vetorDeBlocos->vetorDeVertices.push_back(vertices);

        
        Rect grid_rect(vertices.coordinateX, vertices.coordinateY, vertices.edgeX, vertices.edgeY);
        // cout << vetorDeBlocos->vetorDeVertices.size()     << endl;
        // cout << vetorDeBlocos->vetorDeImagens.size()     << endl;
        cout << grid_rect << endl;
        
        // rectangle(image, grid_rect, Scalar(rand() % 256, rand() % 256, rand() % 256), 1);
        // imshow("image", image);
        // imshow(format("grid(%d,%d)-(%d,%d)", vertices.coordinateX,vertices.coordinateY, vertices.rightSide, vertices.coordinateY2), image(grid_rect));
        // waitKey();
        
        return;
    }
    else if (numProcessos == 2)
    {
        factor = 2;
        numProcessos_1 = 1;
        numProcessos_2 = 1;
    }
    else
    {
        factor = 2 + (rand() % (numProcessos - 1));
        numProcessos_1 = numProcessos/factor;
        numProcessos_2 = numProcessos - numProcessos_1;
    }
        //cout <<"..facor: "<< factor << endl;
        //cout <<"..numProcessos: "<< numProcessos << endl;
        //cout <<"..numProcessos_1: "<< numProcessos_1 << endl;
        //cout <<"..numProcessos_2: "<< numProcessos_2 << endl;

    if (vertices.edgeX < vertices.edgeY)
    {
        //cout << "valor de divisao:" << factor << endl
             //<< "TamanhoY " << vertices.edgeY << endl;

        int horizontal_cut = vertices.edgeY / factor;
        Vertices v1(vertices.coordinateX, vertices.coordinateX2, vertices.coordinateY, vertices.coordinateY + horizontal_cut);
        Vertices v2(vertices.coordinateX, vertices.coordinateX2, vertices.coordinateY + horizontal_cut, vertices.coordinateY2);
        slice_image(image, v1, vetorDeBlocos, numProcessos_1);
        slice_image(image, v2, vetorDeBlocos, numProcessos_2);
    }
    else
    {
        //cout << "valor de divisao:" << factor << endl
             //<< "TamanhoX " << vertices.edgeX << endl;

        int vertical_cut = vertices.edgeX / (factor);
        Vertices v1(vertices.coordinateX, vertices.coordinateX + vertical_cut, vertices.coordinateY, vertices.coordinateY2);
        Vertices v2(vertices.coordinateX + vertical_cut, vertices.coordinateX2, vertices.coordinateY, vertices.coordinateY2);
        slice_image(image, v1, vetorDeBlocos, numProcessos_1);
        slice_image(image, v2, vetorDeBlocos, numProcessos_2);
    }
}