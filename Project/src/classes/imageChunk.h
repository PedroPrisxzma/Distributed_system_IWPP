#ifndef IMAGECHUNK_H
#define IMAGECHUNK_H

#include <opencv2/opencv.hpp>
using namespace cv;

#include <memory>
#include <iostream>
using namespace std;

#include "vertices.h"

typedef struct boundBox
{
    int coordinateX, coordinateY, edgeX, edgeY, rank;
} BoundBox; 

class ImageChunk
{
    public:
    vector<Vertices> vetorDeVertices;

    public:
    vector<Mat> vetorDeImagens;
    public:
    vector<Mat> vetorDeMascaras;

    ImageChunk();
    int mySize();

};

#endif
