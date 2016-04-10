#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "opencv2/ml/ml.hpp"
#include "HOGExtractor.h"

using namespace std;
using namespace cv;

#pragma once
class DetEvaluator
{
public:
	DetEvaluator(void);
	~DetEvaluator(void);
	float EvalDetection(Mat res, vector<Mat> gt, vector<Mat>& report);


private:
	Mat Cumsum(const Mat& in);
	float ComputeAP(const Mat& rec, const Mat& prec);
};

