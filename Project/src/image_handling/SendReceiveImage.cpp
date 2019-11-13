
#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;


const int MAXBYTES=8*1024*1024;
uchar buffer[MAXBYTES];

void matsnd(Mat& m,int dest){
    int rows  = m.rows;
    int cols  = m.cols;
    int type  = m.type();
    int channels = m.channels();
    memcpy(&buffer[0 * sizeof(int)],(uchar*)&rows,sizeof(int));
    memcpy(&buffer[1 * sizeof(int)],(uchar*)&cols,sizeof(int));
    memcpy(&buffer[2 * sizeof(int)],(uchar*)&type,sizeof(int));
    // See note at end of answer about "bytes" variable below!!!
    // https://stackoverflow.com/questions/43502254/what-is-an-optimal-way-to-send-opencv-mat-over-mpi
    int bytespersample=1; // change if using shorts or floats
    int bytes=m.rows*m.cols*channels*bytespersample;
    cout << "matsnd: rows=" << rows << endl;
    cout << "matsnd: cols=" << cols << endl;
    cout << "matsnd: type=" << type << endl;
    cout << "matsnd: channels=" << channels << endl;
    cout << "matsnd: bytes=" << bytes << endl;  
    if(!m.isContinuous())
    { 
       m = m.clone();
    }
    memcpy(&buffer[3*sizeof(int)],m.data,bytes);
    MPI_Send(&buffer,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest,0,MPI_COMM_WORLD);
}

Mat matrcv(int src){
    MPI_Status status;
    int count,rows,cols,type;//,channels;    
    MPI_Recv(&buffer,sizeof(buffer),MPI_UNSIGNED_CHAR,src,0,MPI_COMM_WORLD,&status);
    MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
    memcpy((uchar*)&rows,&buffer[0 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&cols,&buffer[1 * sizeof(int)], sizeof(int));
    memcpy((uchar*)&type,&buffer[2 * sizeof(int)], sizeof(int));

    cout << "matrcv: Count=" << count << endl;
    cout << "matrcv: rows=" << rows << endl;
    cout << "matrcv: cols=" << cols << endl;
    cout << "matrcv: type=" << type << endl;

    // Make the mat
    Mat received= Mat(rows,cols,type,(uchar*)&buffer[3*sizeof(int)]);
    return received;
}
