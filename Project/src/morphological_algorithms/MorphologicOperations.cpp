#include <algorithm>
#include <queue>
#include <limits>
#include <list>
#include <omp.h>
#include <iostream>
#include <mpi.h>

#include "MorphologicOperations.h"
using namespace cv;

namespace nscale
{

template <typename T>
inline void propagate(const Mat &image, Mat &output, std::queue<int> &xQ, std::queue<int> &yQ,
					  int x, int y, T *iPtr, T *oPtr, const T &pval)
{

	T qval = oPtr[x];
	T ival = iPtr[x];
	if ((qval < pval) && (ival != qval))
	{
		oPtr[x] = min(pval, ival);
		xQ.push(x);
		yQ.push(y);
	}
}

template <typename T>
Mat imreconstruct(const Mat &seeds, const Mat &image, int connectivity, std::queue<int> &xQ, std::queue<int> &yQ)
{
	// Mat imreconstruct(const Mat& seeds, const Mat& image, int connectivity) {
	CV_Assert(image.channels() == 1);
	CV_Assert(seeds.channels() == 1);
	// imshow("image", seeds);
	// imshow("image", image);
	// waitKey();

	Mat output(seeds.size() + Size(2, 2), seeds.type());
	copyMakeBorder(seeds, output, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	Mat input(image.size() + Size(2, 2), image.type());
	copyMakeBorder(image, input, 1, 1, 1, 1, BORDER_CONSTANT, 0);
	// imshow("image", output);
	// imshow("image", input);
	// waitKey();

	T pval, preval;
	int xminus, xplus, yminus, yplus;
	int maxx = output.cols - 1;
	int maxy = output.rows - 1;
	// std::queue<int> xQ;
	// std::queue<int> yQ;
	std::queue<int> xSend;
	std::queue<int> ySend;
	T *oPtr;
	T *oPtrMinus;
	T *oPtrPlus;
	T *iPtr;
	T *iPtrPlus;
	T *iPtrMinus;

	//	uint64_t t1 = cci::common::event::timestampInUS();

	int count = 0;
	if (xQ.empty())
	{
		// raster scan
		for (int y = 1; y < maxy; ++y)
		{

			oPtr = output.ptr<T>(y);
			oPtrMinus = output.ptr<T>(y - 1);
			iPtr = input.ptr<T>(y);

			preval = oPtr[0];
			for (int x = 1; x < maxx; ++x)
			{
				xminus = x - 1;
				xplus = x + 1;
				pval = oPtr[x];

				// walk through the neighbor pixels, left and up (N+(p)) only
				pval = max(pval, max(preval, oPtrMinus[x]));

				if (connectivity == 8)
				{
					pval = max(pval, max(oPtrMinus[xplus], oPtrMinus[xminus]));
				}
				preval = min(pval, iPtr[x]);
				oPtr[x] = preval;
			}
		}

		// anti-raster scan
		for (int y = maxy - 1; y > 0; --y)
		{
			oPtr = output.ptr<T>(y);
			oPtrPlus = output.ptr<T>(y + 1);
			oPtrMinus = output.ptr<T>(y - 1);
			iPtr = input.ptr<T>(y);
			iPtrPlus = input.ptr<T>(y + 1);

			preval = oPtr[maxx];
			for (int x = maxx - 1; x > 0; --x)
			{
				xminus = x - 1;
				xplus = x + 1;

				pval = oPtr[x];

				// walk through the neighbor pixels, right and down (N-(p)) only
				pval = max(pval, max(preval, oPtrPlus[x]));

				if (connectivity == 8)
				{
					pval = max(pval, max(oPtrPlus[xplus], oPtrPlus[xminus]));
				}

				preval = min(pval, iPtr[x]);
				oPtr[x] = preval;

				// capture the seeds
				// walk through the neighbor pixels, right and down (N-(p)) only
				pval = oPtr[x];

				if ((oPtr[xplus] < min(pval, iPtr[xplus])) ||
					(oPtrPlus[x] < min(pval, iPtrPlus[x])))
				{
					xQ.push(x);
					yQ.push(y);
					++count;
					continue;
				}

				if (connectivity == 8)
				{
					if ((oPtrPlus[xplus] < min(pval, iPtrPlus[xplus])) ||
						(oPtrPlus[xminus] < min(pval, iPtrPlus[xminus])))
					{
						xQ.push(x);
						yQ.push(y);
						++count;
						continue;
					}
				}
			}
		}
	}

	//	uint64_t t2 = cci::common::event::timestampInUS();
	//	std::cout << "	scan time = " << t2-t1 << "ms for " << count << " queue entries."<< std::endl;

	// now process the queue.
	//	T qval, ival;
	int x, y;
	count = 0;
	while (!(xQ.empty()))
	{

		++count;
		x = xQ.front();
		y = yQ.front();
		xQ.pop();
		yQ.pop();
		xminus = x - 1;
		xplus = x + 1;
		yminus = y - 1;
		yplus = y + 1;

		oPtr = output.ptr<T>(y);
		oPtrPlus = output.ptr<T>(yplus);
		oPtrMinus = output.ptr<T>(yminus);
		iPtr = input.ptr<T>(y);
		iPtrPlus = input.ptr<T>(yplus);
		iPtrMinus = input.ptr<T>(yminus);

		pval = oPtr[x];

		// look at the 4 connected components
		if (y > 0)
		{
			if (yminus == 0)
			{
				// std::cout << "ADD X: " << x << " Y: " << yminus << std::endl;
				xSend.push(x);
				ySend.push(yminus);
			}
			propagate<T>(input, output, xQ, yQ, x, yminus, iPtrMinus, oPtrMinus, pval);
		}

		if (y < maxy)
		{
			if (yplus == maxy)
			{
				// std::cout << "ADD X: " << x << " Y: " << yplus << std::endl;
				xSend.push(x);
				ySend.push(yplus);
			}
			propagate<T>(input, output, xQ, yQ, x, yplus, iPtrPlus, oPtrPlus, pval);
		}

		if (x > 0)
		{
			if (xminus == 0)
			{
				// std::cout << "ADD X: " << xminus << " Y: " << y << std::endl;
				xSend.push(xminus);
				ySend.push(y);
			}
			propagate<T>(input, output, xQ, yQ, xminus, y, iPtr, oPtr, pval);
		}

		if (x < maxx)
		{
			if (xplus == maxx)
			{
				// std::cout << "ADD X: " << xplus << " Y: " << y << std::endl;
				xSend.push(xplus);
				ySend.push(y);
			}
			propagate<T>(input, output, xQ, yQ, xplus, y, iPtr, oPtr, pval);
		}

		// now 8 connected
		if (connectivity == 8)
		{

			if (y > 0)
			{
				if (x > 0)
				{
					propagate<T>(input, output, xQ, yQ, xminus, yminus, iPtrMinus, oPtrMinus, pval);
				}
				if (x < maxx)
				{
					propagate<T>(input, output, xQ, yQ, xplus, yminus, iPtrMinus, oPtrMinus, pval);
				}
			}
			if (y < maxy)
			{
				if (x > 0)
				{
					propagate<T>(input, output, xQ, yQ, xminus, yplus, iPtrPlus, oPtrPlus, pval);
				}
				if (x < maxx)
				{
					propagate<T>(input, output, xQ, yQ, xplus, yplus, iPtrPlus, oPtrPlus, pval);
				}
			}
		}
	}

	//	uint64_t t3 = cci::common::event::timestampInUS();
	//	std::cout << "	queue time = " << t3-t2 << "ms for " << count << " queue entries "<< std::endl;

	//	std::cout <<  count << " queue entries "<< std::endl;
	int Qsize = xSend.size();
	int i;
	for (i = 0; i < Qsize; i++)
	{
		x = xSend.front();
		x = (x == 0) ? -1 : x;
		y = ySend.front();
		y = (y == 0) ? -1 : y;

		std::cout << "ADD X: " << x << " Y: " << y << std::endl;

		xQ.push(x);
		yQ.push(y);
		xSend.pop();
		ySend.pop();
	}

	return output(Range(1, maxy), Range(1, maxx));
}

//template Mat imreconstructGeorge<unsigned char>(const Mat& seeds, const Mat& image, int connectivity);

// template DllExport Mat imreconstruct<unsigned char>(const Mat& seeds, const Mat& image, int connectivity);
template DllExport Mat imreconstruct<unsigned char>(const Mat &seeds, const Mat &image, int connectivity, std::queue<int> &xQ, std::queue<int> &yQ);

// template DllExport Mat imreconstruct<unsigned short int>(const Mat& seeds, const Mat& image, int connectivity);
template DllExport Mat imreconstruct<unsigned short int>(const Mat &seeds, const Mat &image, int connectivity, std::queue<int> &xQ, std::queue<int> &yQ);

// template DllExport Mat imreconstruct<float>(const Mat& seeds, const Mat& image, int connectivity);
template DllExport Mat imreconstruct<float>(const Mat &seeds, const Mat &image, int connectivity, std::queue<int> &xQ, std::queue<int> &yQ);
} // namespace nscale