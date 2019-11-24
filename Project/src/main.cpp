#include <stdlib.h>

#include <iostream>
#include <queue>
using namespace std;

#include <omp.h>
#include <mpi.h>

#include <opencv2/opencv.hpp>
using namespace cv;

#include "morphological_algorithms/MorphologicAdministrator.h"
#include "morphological_algorithms/MorphologicOperations.h"
#include "image_handling/ImageSeparator.h"
#include "image_handling/SendReceiveImage.h"
#include "image_handling/FindNeighbour.h"
#include "classes/imageChunk.h"
#include "classes/vertices.h"

int main(int argc, char *argv[])
{
	// **** Testar o arquivo nscale/src/segment/test/src/imreconTest.cpp  ****
	// Para entender a reconstrucao morfologica

	// Separate the image, must be flexible,
	// allowing to separate into same size or diferent sized smaller chunks

	// Store references to the image's corners
	// Neighbours vector BoundBox
	// Send borders interssections

	// Allocate MPI object
	// Send the image to the available machines, to execute the IWPP
	// When hiting a corner must comunicate with it's neighbours

	//Stop condition, possibilities:
	// Either broadcast to all nodes
	// Broadcast to a node (in a ring like fashion) until all have finished

	int numeroDeProcessos, rank;
	int data_size;
	// ImageChunk imageChunk;
	Mat imgblock;
	Mat mskblock;

	// Inicia programacao distribuida
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numeroDeProcessos);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

 /* create a type for struct car */
	const int nitems=5;
	int		  blocklengths[5] = {1,1,1,1,1};
	MPI_Datatype types[5] = {MPI_INT, MPI_INT,MPI_INT, MPI_INT,MPI_INT};
	MPI_Datatype mpi_BoundBox_type;
	MPI_Aint	 offsets[5];

	offsets[0] = offsetof(BoundBox, coordinateX);
	offsets[1] = offsetof(BoundBox, coordinateY);
	offsets[2] = offsetof(BoundBox, edgeX);
	offsets[3] = offsetof(BoundBox, edgeY);
	offsets[4] = offsetof(BoundBox, rank);

	MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_BoundBox_type);
	MPI_Type_commit(&mpi_BoundBox_type);


	vector<BoundBox> rankNeighbours;
	vector<vector<BoundBox>> vizinhos; // Lista de BoundBox listas
	BoundBox *rankVertices;
	rankVertices = (BoundBox *)malloc(sizeof(BoundBox));
	BoundBox *vert_list = (BoundBox *)malloc(numeroDeProcessos * sizeof(BoundBox));

	Mat inputImage;
	if (rank == 0)
	{
		if (argc < 3)
		{
			cout << " Usage: mpiexec -n <process_number> ./main <marker> <mask>" << endl;
			throw std::exception();
		}

		inputImage = image_reader(argv[1]);
		Mat inputMask = image_reader(argv[2]);
		cout << inputImage.size << endl;

		ImageChunk imageBlocks = separate_image(inputImage, inputMask, numeroDeProcessos);


		for (int i = 0; i < numeroDeProcessos; i++)
		{
			vert_list[i].coordinateX = imageBlocks.vetorDeVertices[i].coordinateX;
			vert_list[i].coordinateY = imageBlocks.vetorDeVertices[i].coordinateY;
			vert_list[i].edgeX = imageBlocks.vetorDeVertices[i].edgeX;
			vert_list[i].edgeY = imageBlocks.vetorDeVertices[i].edgeY;
			vert_list[i].rank = i;

			cout << "Rank: "<<vert_list[i].rank<<endl;
			cout << "Coordinate x: "<<vert_list[i].coordinateX<<" y: "<<vert_list[i].coordinateY<<endl;
			cout << "Size x: "<<vert_list[i].edgeX<<" y: "<<vert_list[i].edgeY<<endl;

		}

		// Acha vizinhos dos Chunks de imagens
		vizinhos = FindNeighbours(vert_list, numeroDeProcessos, 4);
		// cout << "---------------------------------" << endl;
		//for (int i = 0; i < numeroDeProcessos; i++)
		//{
			// cout << "Vizinhos de: " << i << endl;
			// cout << "I -> cX: " << vert_list[i].coordinateX << " cY: " << vert_list[i].coordinateY << " eX: " << vert_list[i].edgeX << " eY: " << vert_list[i].edgeY << endl;
			// cout << " --------------------------- " << vizinhos[i].size() << endl;

		//	int currentVizinhosLenght = vizinhos[i].size();
		//	for (int j = 0; j < currentVizinhosLenght; j++)
		//	{
				// cout << "J -> cX: " << vizinhos[i][j].coordinateX << " cY: " << vizinhos[i][j].coordinateY << " eX: " << vizinhos[i][j].edgeX << " eY: " << vizinhos[i][j].edgeY << endl;
				// cout << endl;
		//	}
		//}

		for (int i = 1; i < numeroDeProcessos; i++)
		{
			MPI_Send(&vert_list[i], sizeof(BoundBox), MPI_BYTE, i, 0, MPI_COMM_WORLD);

			// Envia o vetor de BoundBoxes
			// MPI_Send(&vert_list, 5*numeroDeProcessos, MPI_INT, i, 0, MPI_COMM_WORLD);
			
		  data_size = vizinhos[i].size();
			MPI_Send(&data_size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			MPI_Send(vizinhos[i].data(), data_size, mpi_BoundBox_type, i, 0, MPI_COMM_WORLD);
			// MPI_Send(&vizinhos[i], data_size, mpi_BoundBox_type, i, 0, MPI_COMM_WORLD);

			// Envia as imagens
			matsnd(imageBlocks.vetorDeImagens[i], i, 0);
			matsnd(imageBlocks.vetorDeMascaras[i], i, 1);
		}

		memcpy(rankVertices, &vert_list[0], sizeof(BoundBox));
		//cout << "size rank 0 x: " << rankVertices->edgeX << " y: " << rankVertices->edgeY << endl;
		rankNeighbours = vizinhos[0];
		imgblock = Mat(imageBlocks.vetorDeImagens[0]).clone();
		mskblock = Mat(imageBlocks.vetorDeMascaras[0]).clone();

		// free(vert_list);
	}
	else
	{
		MPI_Recv(rankVertices, sizeof(BoundBox), MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Recebe o vetor de BoundBoxes
		MPI_Recv(&data_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		rankNeighbours.resize(data_size);

		MPI_Recv(&rankNeighbours[0], data_size, mpi_BoundBox_type, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		// Recebe as imagens
		imgblock = matrcv(0, 0);
		mskblock = matrcv(0, 1);
	}

	//cout << "Rank: " << rank << ":" << rankVertices->rank << " vertices x: " << rankVertices->coordinateX << 
	//" y: " << rankVertices->coordinateY << " ex: " << rankVertices->edgeX << " ey: " << rankVertices->edgeY <<  endl;

	// int size = rankNeighbours.size();
	// for (int i = 0; i < size; i++)
	// {
	// 	cout << "Rank: " << rank << "  rankNeighbour[" << i << "].rank: " << rankNeighbours[i].rank << endl;
	// 	cout << "	Rank: " << rank << "  rankNeighbour[" << i << "].coordenadaX: " << rankNeighbours[i].coordinateX << endl;
	// 	cout << "	Rank: " << rank << "  rankNeighbour[" << i << "].coordenadaY: " << rankNeighbours[i].coordinateY << endl;
	// 	cout << "	Rank: " << rank << "  rankNeighbour[" << i << "].edgeX: " << rankNeighbours[i].edgeX << endl;
	// 	cout << "	Rank: " << rank << "  rankNeighbour[" << i << "].edgeY: " << rankNeighbours[i].edgeY << endl;
	// }

	// imshow("image final "+to_string(rank), imgblock);
	// waitKey();

	// imshow("mask final "+to_string(rank), mskblock);
	// waitKey();

	//////////////////////////////////////////////////////////////////////////////////////
	// Morphological alg

	Mat recon = imReconstructAdm(imgblock, mskblock, *rankVertices, rankNeighbours, rank, numeroDeProcessos);

	//imshow("imgblock image "+to_string(rank), imgblock);
	// imshow("recon image "+to_string(rank), recon);
	// waitKey();

	if (rank == 0)
	{
		Mat output(inputImage.size(), inputImage.type());
		for (int i = 1; i < numeroDeProcessos; i++)
		{
			Mat recive =  matrcv(i,5);
			cout << "Copy to x: "<<vert_list[i].coordinateX<<" y: "<<vert_list[i].coordinateY<<endl;
			recive.copyTo(output(cv::Rect(vert_list[i].coordinateX,vert_list[i].coordinateY,recive.cols, recive.rows)));
		// imshow("reconstruct image FINAL", output);
		// waitKey();
		}

		cout << "Copy to x: "<<vert_list[0].coordinateX<<" y: "<<vert_list[0].coordinateY<<endl;
		recon.copyTo(output(cv::Rect(0,0,recon.cols, recon.rows)));

		imshow("reconstruct image FINAL", output);
		waitKey();
	}
	else
	{
		matsnd(recon,0,5);
	}
	
	
	// Finaliza programacao distribuida
	MPI_Finalize();
	return 0;
}

// Perguntar:
// Ao separar os blocos da imagem, devem ser referencias ou copias?
// Ao terminar, devo juntar os blocos processados numa imagem?

// ---> tem que ser cópia, referencia não funciona. Ao terminar, tem que juntar de volta a imagem.