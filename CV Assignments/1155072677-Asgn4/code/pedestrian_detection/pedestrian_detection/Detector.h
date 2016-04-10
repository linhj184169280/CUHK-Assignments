#include <vector>
#include "opencv2/opencv.hpp"
#include "opencv/cv.h"
#include "opencv2/ml/ml.hpp"
#include "HOGExtractor.h"

using namespace std;
using namespace cv;

#pragma once
class Detector : public CvSVM {
public:
	Detector();
	~Detector(void);
	void TrainDetector(const Mat& trainSample, const Mat& trainLabel, 
		const int* featureSizes, const CvSVMParams& params);
	void SetDetector(const Mat det);
	Mat GetDetector();
	Mat MultiscaleDetection(const Mat& im, HOGExtractor& ext, 
		const float* scales, const int numberOfScale, const float* threshold);
	Mat Detection(const Mat& im, HOGExtractor& ext, const float threshold);
	void DisplayDetection(Mat im, Mat top);

private:
	Mat TransSV2Detector();
	Mat detector;
};

