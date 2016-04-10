#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "common.h"

using namespace std;
using namespace cv;

#pragma once
class PostProcessor
{
public:
	PostProcessor(void);
	~PostProcessor(void);
	Mat NonMaxSup(const Mat& boxes, const float overlap);
};

