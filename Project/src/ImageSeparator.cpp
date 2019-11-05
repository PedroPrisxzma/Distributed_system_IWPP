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
Mat image_reader(char *filename)
{
    
    Mat image;
    image = imread(filename, CV_LOAD_IMAGE_COLOR);

    if (!image.data) // Check for invalid input
    {
        cout << "Could not open or find the image" << std::endl;
        throw std::exception();
    }

    namedWindow("Display window", WINDOW_AUTOSIZE); // Create a window for display.
    imshow("Display window", image);                // Show our image inside it.

    waitKey(0);

    return image;
}

// Separate the image into smaller chunks
void separate_image(Mat image, int n_proc)
{
    int width = image.cols;
    int height = image.rows;
    // int GRID_SIZE = 100;

    Vertices vertice(0, width, 0, height);

    vector<Rect> mCells;

    cout << "Image Details" << endl;
    printf("Width: %d\nHeight: %d\n", width, height);

    slice_image(image, vertice, mCells, n_proc);

    imshow("image", image);
    waitKey();
}

void slice_image(Mat image, Vertices vertices, vector<Rect> mCells, int n_proc)
{
    //TODO:
    // Mudar para conter no máximo um número de divisões determinado pelo número de nós no sistema
    int factor, n_proc_1, n_proc_2;

    if (n_proc == 0 || vertices.sizex == 0 || vertices.sizey == 0 )
        return;
    else if (n_proc == 1)
    {
        Rect grid_rect(vertices.leftSide, vertices.topSide, vertices.sizex, vertices.sizey);
        cout << grid_rect << endl;
        mCells.push_back(grid_rect);
        rectangle(image, grid_rect, Scalar(rand() % 256, rand() % 256, rand() % 256), 1);
        // imshow("image", image);
        // imshow(format("grid(%d,%d)-(%d,%d)", vertices.leftSide,vertices.topSide, vertices.rightSide, vertices.botSide), image(grid_rect));
        // waitKey();
        return;
    }
    else if (n_proc == 2)
    {
        factor = 2;
        n_proc_1 = 1;
        n_proc_2 = 1;
    }
    else
    {
        factor = 2 + (rand() % (n_proc - 1));
        n_proc_1 = n_proc/factor;
        n_proc_2 = n_proc - n_proc_1;
    }
        cout <<"..f: "<< factor << endl;
        cout <<"..p: "<< n_proc << endl;
        cout <<"..p1: "<< n_proc_1 << endl;
        cout <<"..p2: "<< n_proc_2 << endl;

    if (vertices.sizex < vertices.sizey)
    {
        cout << "valor de divisao:" << factor << endl
             << "TamanhoY " << vertices.sizey << endl;

        int horizontal_cut = vertices.sizey / factor;
        Vertices v1(vertices.leftSide, vertices.rightSide, vertices.topSide, vertices.topSide + horizontal_cut);
        Vertices v2(vertices.leftSide, vertices.rightSide, vertices.topSide + horizontal_cut, vertices.botSide);
        slice_image(image, v1, mCells, n_proc_1);
        slice_image(image, v2, mCells, n_proc_2);
    }
    else
    {
        cout << "valor de divisao:" << factor << endl
             << "TamanhoX " << vertices.sizex << endl;

        int vertical_cut = vertices.sizex / (factor);
        Vertices v1(vertices.leftSide, vertices.leftSide + vertical_cut, vertices.topSide, vertices.botSide);
        Vertices v2(vertices.leftSide + vertical_cut, vertices.rightSide, vertices.topSide, vertices.botSide);
        slice_image(image, v1, mCells, n_proc_1);
        slice_image(image, v2, mCells, n_proc_2);
    }
}
