#ifndef VERTICES_H
#define VERTICES_H

class Vertices
{
    public:
    int leftSide, rightSide, sizex, topSide, botSide, sizey;

    Vertices(int,int,int,int);
    int mySize();
};

#endif