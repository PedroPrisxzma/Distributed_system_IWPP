
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

void matsnd(Mat& m, int dest, int image_or_mask){
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

	MPI_Send(buffer,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest, 0,MPI_COMM_WORLD);
	free(buffer);
	// if(image_or_mask == 0)
	// {
	//	 memcpy(&buffer_image[0 * sizeof(int)],(uchar*)&rows,sizeof(int));
	//	 memcpy(&buffer_image[1 * sizeof(int)],(uchar*)&cols,sizeof(int));
	//	 memcpy(&buffer_image[2 * sizeof(int)],(uchar*)&type,sizeof(int));
		
	//	 memcpy(&buffer_image[3*sizeof(int)],m.data,bytes);
		
	//	 MPI_Ssend(&buffer_image,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest, image_or_mask,MPI_COMM_WORLD);
	// }
	// else if(image_or_mask == 1)
	// {
	//	 memcpy(&buffer_mask[0 * sizeof(int)],(uchar*)&rows,sizeof(int));
	//	 memcpy(&buffer_mask[1 * sizeof(int)],(uchar*)&cols,sizeof(int));
	//	 memcpy(&buffer_mask[2 * sizeof(int)],(uchar*)&type,sizeof(int));

	//	 memcpy(&buffer_mask[3*sizeof(int)],m.data,bytes);
		
	//	 MPI_Ssend(&buffer_mask,bytes+3*sizeof(int),MPI_UNSIGNED_CHAR,dest, image_or_mask,MPI_COMM_WORLD);
	// }
	// else
	// {
	//	 cout << "Invalid image_or_mask parameter passed, must be 0 for image and 1 for mask" << endl;
	//	 throw std::exception();	
	// }
}

Mat matrcv(int src, int image_or_mask){
	MPI_Status status;
	int count,rows,cols,type;//,channels;

	buffer = (uchar*) malloc(MAXBYTES);

	MPI_Recv(buffer, MAXBYTES,MPI_UNSIGNED_CHAR,src,0,MPI_COMM_WORLD,&status);
		
	MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
	
	memcpy((uchar*)&rows,buffer+(0 * sizeof(int)), sizeof(int));
	memcpy((uchar*)&cols,buffer+(1 * sizeof(int)), sizeof(int));
	memcpy((uchar*)&type,buffer+(2 * sizeof(int)), sizeof(int));
	Mat received_image = Mat(rows,cols,type,(uchar*)buffer+(3*sizeof(int)));
	received_image = received_image.clone();
	free(buffer);
	return received_image;

	// if(image_or_mask == 0)
	// {
		// MPI_Recv(&buffer_image, sizeof(buffer_image),MPI_UNSIGNED_CHAR,src,image_or_mask,MPI_COMM_WORLD,&status);
		
		// MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);
		
		// memcpy((uchar*)&rows,&buffer_image[0 * sizeof(int)], sizeof(int));
		// memcpy((uchar*)&cols,&buffer_image[1 * sizeof(int)], sizeof(int));
		// memcpy((uchar*)&type,&buffer_image[2 * sizeof(int)], sizeof(int));

		// cout << "matrcv: Count=" << count << endl;
		// cout << "matrcv: rows=" << rows << endl;
		// cout << "matrcv: cols=" << cols << endl;
		// cout << "matrcv: type=" << type << endl;

		// Make the mat
		// Mat received_image = Mat(rows,cols,type,(uchar*)&buffer_image[3*sizeof(int)]);
		//imshow("image "+to_string(image_or_mask), received_image);
		//waitKey();
		// return received_image;
	// }
	// else if(image_or_mask == 1)
	// {
	//	 MPI_Recv(&buffer_mask,sizeof(buffer_mask),MPI_UNSIGNED_CHAR,src,image_or_mask,MPI_COMM_WORLD,&status);
		
	//	 MPI_Get_count(&status,MPI_UNSIGNED_CHAR,&count);

	//	 memcpy((uchar*)&rows,&buffer_mask[0 * sizeof(int)], sizeof(int));
	//	 memcpy((uchar*)&cols,&buffer_mask[1 * sizeof(int)], sizeof(int));
	//	 memcpy((uchar*)&type,&buffer_mask[2 * sizeof(int)], sizeof(int));

	//	 // Make the mat
	//	 Mat received_mask = Mat(rows,cols,type,(uchar*)&buffer_mask[3*sizeof(int)]);
	//	 //imshow("image "+to_string(image_or_mask), received_mask);
	//	 //waitKey();
	//	 return received_mask;
	// }
	// else
	// {
	//	 cout << "Invalid image_or_mask parameter passed, must be 0 for image and 1 for mask" << endl;
	//	 throw std::exception();	
	// }
}
