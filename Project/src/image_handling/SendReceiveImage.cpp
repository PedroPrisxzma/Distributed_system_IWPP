
#include <iostream>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;

const int MAXBYTES = 7*1024 * 1024;
uchar buffer_image[MAXBYTES];
uchar buffer_mask[MAXBYTES];

void matsnd(Mat &m, int dest, int flag)
{
	uchar *buffer;
	int rows = m.rows;
	int cols = m.cols;
	int type = m.type();
	int channels = m.channels();

	// See note at end of answer about "bytes" variable below!!!
	// https://stackoverflow.com/questions/43502254/what-is-an-optimal-way-to-send-opencv-mat-over-mpi
	int bytespersample = 1; // change if using shorts or floats
	int bytes = m.rows * m.cols * channels * bytespersample;

	int blocks = ((bytes + 3 * sizeof(int)) / MAXBYTES) + 1;
	int send_size;

	// cout << "matsnd: rows=" << rows << endl;
	// cout << "matsnd: cols=" << cols << endl;
	// cout << "matsnd: type=" << type << endl;
	// cout << "matsnd: channels=" << channels << endl;
	// cout << "matsnd: bytes=" << bytes << endl;

	if (!m.isContinuous())
	{
		m = m.clone();
	}
	buffer = (uchar *)malloc(bytes + 3 * sizeof(int));

	memcpy(buffer + (0 * sizeof(int)), (uchar *)&rows, sizeof(int));
	memcpy(buffer + (1 * sizeof(int)), (uchar *)&cols, sizeof(int));
	memcpy(buffer + (2 * sizeof(int)), (uchar *)&type, sizeof(int));

	cout << "Send total bytes: "<<bytes<<" in "<<blocks<<" blocks"<<endl;
	MPI_Send(&bytes, 1, MPI_INT, dest, flag, MPI_COMM_WORLD);
	for (int i = 0; i < blocks; i++)
	{
		if ((bytes + 3 * sizeof(int)) < MAXBYTES)
			send_size = bytes;
		else
			send_size = MAXBYTES - 3 * sizeof(int);
		memcpy(buffer + (3 * sizeof(int)), m.data + (i * send_size), send_size);
		cout << "Send : "<<send_size<<" of "<<bytes<<endl;
		MPI_Send(buffer, send_size + 3 * sizeof(int), MPI_UNSIGNED_CHAR, dest, flag, MPI_COMM_WORLD);
		bytes -= send_size;
	}

	free(buffer);
}

Mat matrcv(int src, int flag)
{
	uchar *img_buffer;
	uchar *recive_buffer;
	MPI_Status status;
	int count, rows, cols, type; //,channels;
	int bytes;

	MPI_Recv(&bytes, 1, MPI_INT, src, flag, MPI_COMM_WORLD, &status);
	MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);

	img_buffer = (uchar *)malloc(bytes);
	recive_buffer = (uchar *)malloc(MAXBYTES);
	int blocks = ((bytes + 3 * sizeof(int)) / MAXBYTES) + 1;
	cout << "Recive bytes: "<<bytes<<" in "<<blocks<<" blocks"<<endl;
	
	int recv_size;
	for (int i = 0; i < blocks; i++)
	{
		cout <<"try recive "<< i<<endl;
		MPI_Recv(recive_buffer, MAXBYTES, MPI_UNSIGNED_CHAR, src, flag, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_UNSIGNED_CHAR, &count);
		if (i == 0)
		{
			memcpy((uchar *)&rows, recive_buffer + (0 * sizeof(int)), sizeof(int));
			memcpy((uchar *)&cols, recive_buffer + (1 * sizeof(int)), sizeof(int));
			memcpy((uchar *)&type, recive_buffer + (2 * sizeof(int)), sizeof(int));
		}
		if ((bytes + 3 * sizeof(int)) < MAXBYTES)
			recv_size = bytes;
		else
			recv_size = MAXBYTES - 3 * sizeof(int);
		cout << "Recive : "<<recv_size<<" of "<<bytes<<endl;
		memcpy((uchar *)img_buffer + (i * (MAXBYTES - 3 * sizeof(int))), recive_buffer + (3 * sizeof(int)),recv_size);
		bytes -= recv_size;
	}

	Mat received_image = Mat(rows, cols, type, (uchar *)img_buffer);
	received_image = received_image.clone();

	free(img_buffer);
	free(recive_buffer);
	return received_image;
}
