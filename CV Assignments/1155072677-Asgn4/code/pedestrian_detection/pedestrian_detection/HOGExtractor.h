#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "common.h"

using namespace std;
using namespace cv;

#pragma once
class HOGExtractor {
public:
	HOGExtractor(const int bins, const int cells, const int blocks);
	~HOGExtractor(void);
	Mat ExtractHOG(const Mat& im);
	int GetBins();
	int GetCells();
	int GetBlocks();

private:
	int bins; // The number of bins in histogram.
	int cells; // The number of pixels in a cell.
	int blocks; // The number of cells in a block.
};

