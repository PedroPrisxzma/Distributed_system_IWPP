#include <stdlib.h>
#include <fstream>
#include <exception>
#include <memory>
#include <iostream>
using namespace std;
#include <mpi.h>

#include "vertices.h"

#include "ImageSeparator.h"
#include <opencv2/opencv.hpp>
using namespace cv;

// Read in the image
Mat image_reader(int argc, char *argv[])
{
    if( argc != 2)
    {
        cout <<" Usage: executable ImageToLoad, no ImageToLoad was provided" << endl;
        throw std::exception();    
    }

    Mat image;
    image = imread(argv[1], CV_LOAD_IMAGE_COLOR);
    
    
    imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl;
        throw std::exception();    
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    imshow( "Display window", image );                   // Show our image inside it.

    waitKey(0); 

    return image;
}

// Separate the image into smaller chunks
void separate_image(Mat image)
{
    int width = image.cols;
    int height = image.rows;
    int GRID_SIZE = 100;

    Vertices vertice(0,width,0,height);

    vector<Rect> mCells;

    cout << "Image Details" << endl;
    printf("Width: %d\nHeight: %d\n",width, height);

    slice_image(image, vertice, mCells, GRID_SIZE);

    imshow("image", image);
    waitKey();
}

void slice_image(Mat image, Vertices vertices, vector<Rect> mCells, int min_size)
{   
    //TODO:
    // Mudar para conter no máximo um número de divisões determinado pelo número de nós no sistema

    if (vertices.sizex <= min_size && vertices.sizey <= min_size)
    {
        Rect grid_rect(vertices.leftSide, vertices.topSide, vertices.sizex, vertices.sizey);
        cout << grid_rect<< endl;
        mCells.push_back(grid_rect);
        rectangle(image, grid_rect, Scalar(0, 255, 0), 1);
        // imshow("image", image);
        //imshow(format("grid(%d,%d)-(%d,%d)", vertices.leftSide,vertices.topSide, vertices.rightSide, vertices.botSide), image(grid_rect));
        //waitKey();
    }
    else if (vertices.sizex < vertices.sizey)
    {
        int factor =  2 + rand() % (vertices.sizey/10);
        cout << "valor de divisao:" << factor << endl << "TamanhoY " << vertices.sizey << endl;

        int horizontal_cut = vertices.sizey/factor;
        Vertices v1(vertices.leftSide,vertices.rightSide,vertices.topSide,vertices.topSide+horizontal_cut);
        Vertices v2(vertices.leftSide,vertices.rightSide,vertices.topSide+horizontal_cut,vertices.botSide);
        slice_image(image, v1, mCells, min_size);
        slice_image(image, v2, mCells, min_size);
    }
    else
    {
        int factor =  2 + rand() % (vertices.sizex/10);
        cout << "valor de divisao:" << factor << endl << "TamanhoX " << vertices.sizex << endl;

        int vertical_cut = vertices.sizex/(factor);
        Vertices v1(vertices.leftSide,vertices.leftSide+vertical_cut,vertices.topSide,vertices.botSide);
        Vertices v2(vertices.leftSide+vertical_cut,vertices.rightSide,vertices.topSide,vertices.botSide);
        slice_image(image, v1, mCells, min_size);
        slice_image(image, v2, mCells, min_size);
    }
}


