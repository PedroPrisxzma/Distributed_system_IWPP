#include <stdlib.h>
#include <iostream>
using namespace std; 

#include "vertices.h"

Vertices::Vertices(int left, int right, int top, int bot)
{
    leftSide = left;
    rightSide = right;
    sizex = right-left;
    topSide = top;
    botSide = bot;
    sizey = bot-top;
} 