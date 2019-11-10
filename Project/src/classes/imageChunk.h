#ifndef IMAGECHUNK_H
#define IMAGECHUNK_H

#include <opencv2/opencv.hpp>
using namespace cv;

#include <memory>
#include <iostream>
using namespace std;

#include "vertices.h"

class ImageChunk
{
    public:
    vector<Vertices> vetorDeVertices;

    public:
    vector<Rect> vetorDeImagens;
    
    ImageChunk();

};

#endif
