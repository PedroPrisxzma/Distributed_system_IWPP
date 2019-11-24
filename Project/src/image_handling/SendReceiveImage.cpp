
#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;


const int MAXBYTES=8*1024*1024;
uchar buffer_image[MAXBYTES];
uchar buffer_mask[MAXBYTES];
uchar *buffer;

void matsnd(Mat& m, int dest, int flag){
	int rows  = m.rows;
	int cols  = m.cols;
	int type  = m.type();
	int channels = m.channels();
  
	// See note at end of answer about "bytes" variable below!!!
	// https://stackoverflow.com/questions/43502254/what-is-an-optimal-way-to-send-opencv-mat-over-mpi
	int bytespersample=1; // change if using shorts or floats
	int bytes=m.rows*m.cols*channels*bytespersample;

	// cout << "matsnd: rows=" << rows << endl;
	// cout << "matsnd: cols=" << cols << endl;
	// cout << "matsnd: type=" << type << endl;
	// cout << "matsnd: channels=" << channels << endl;
	// cout << "matsnd: bytes=" << bytes << endl;  

	if(!m.isContinuous())
	{ 
	   m = m.clone();
	}
	buffer = (uchar*) malloc(bytes+3*sizeof(int));

	memcpy(buffer+(0 * sizeof(int)),(uchar*)&rows,sizeof(int));
	memcpy(buffer+(1 * sizeof(int)),(uchar*)&cols,sizeof(int));
	memcpy(buffer+(2 * sizeof(int)),(uchar*)&type,sizeof(int));
	memcpy(buffer+(3*sizeof(int)),m.data,bytes);

	MPI_Send(buffer,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest, flag,MPI_COMM_WORLD);
	free(buffer);
	
}

Mat matrcv(int src, int flag){
	MPI_Status status;
	int count,rows,cols,type;//,channels;

	buffer = (uchar*) malloc(MAXBYTES);

	MPI_Recv(buffer, MAXBYTES,MPI_UNSIGNED_CHAR,src,flag,MPI_COMM_WORLD,&status);
		
	MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
	
	memcpy((uchar*)&rows,buffer+(0 * sizeof(int)), sizeof(int));
	memcpy((uchar*)&cols,buffer+(1 * sizeof(int)), sizeof(int));
	memcpy((uchar*)&type,buffer+(2 * sizeof(int)), sizeof(int));
	Mat received_image = Mat(rows,cols,type,(uchar*)buffer+(3*sizeof(int)));
	received_image = received_image.clone();
	free(buffer);
	return received_image;

	
}
