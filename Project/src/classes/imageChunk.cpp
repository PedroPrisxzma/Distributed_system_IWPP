#include <stdlib.h>
#include <iostream>
using namespace std; 

#include <opencv2/opencv.hpp>
using namespace cv;

#include "imageChunk.h"

ImageChunk::ImageChunk()
{
} 

int ImageChunk::mySize()
{
    int vVertices = vetorDeVertices.size() * vetorDeVertices[0].mySize();

    int vImages = vetorDeImagens[0].area(); 

    return vVertices + vImages;
} 