#include <stdlib.h>
#include <iostream>
using namespace std; 

#include "vertices.h"

Vertices::Vertices(int coordX, int coordX2, int coordY, int coordY2)
{
    coordinateX = coordX;
    coordinateX2 = coordX2;
    edgeX = coordX2-coordX;
    coordinateY = coordY;
    coordinateY2 = coordY2;
    edgeY = coordY2-coordY;

} 

int Vertices::mySize()
{
    return 6 * sizeof(int);
}