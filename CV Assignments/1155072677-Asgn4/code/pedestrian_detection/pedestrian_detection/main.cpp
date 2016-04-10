#include <iostream>
#include <vector>
#include "HOGExtractor.h"
#include "Detector.h"
#include "PostProcessor.h"
#include "DetEvaluator.h"
#include "common.h"

using namespace std;
using namespace cv;

int main() {
	/* Step 0: Configuration of parameters
	*/
	const int bins = 9; // The number of bin to 9.
	const int cells = 8; // The size of cell to cover 8 x 8 pixels.
	const int blocks = 2;	// The size of block to contain 2 x 2  cells.
	const int featureSizes[3] = {15, 7, 36}; // The size of the training feature.
	const int numberOfPositive = 2416; // The number of positive samples.
	const int numberOfNegative = 6090; // The number of negative samples.
	const int numberOfValidation = 40;

	CvSVMParams params; // The parameters of SVM
    params.svm_type = CvSVM::C_SVC;
    params.kernel_type = CvSVM::LINEAR;
    params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER, 500000, 1e-10);

	const float scales[] = { 0.21, 0.27, 0.3}; // Multiple scales
//	const float threshold[] = {3.0, 3.5, 4.5}; // The threshold for each scales
	const float threshold[] = { 1.5 ,1.55, 1.6 }; // The threshold for each scales
	const bool drawBox = true; // Switch to draw the bounding boxes.
	const float overlap = 0.2; // The overlap ration for NMS

	/* Step 1: Extract HOGs of all training samples
	*/
	cout << "Step 1: Extract HOGs of all training samples..." << endl;
	HOGExtractor ext(bins, cells, blocks); // Initialize a HOG extractor.
	Mat trainSample;
	Mat trainLabel;
	char buf[6];
	if (!fopen("..\\Descriptor\\traindata.yml", "r")) { // Delete the descriptor files if you want to extract new ones
		Mat positiveDescriptor(numberOfPositive, featureSizes[0]*featureSizes[1]*featureSizes[2], 
			CV_32FC1, Scalar(0));
		Mat positiveLabel = Mat::ones(numberOfPositive, 1, CV_32FC1);
		Mat negativeDescriptor(numberOfNegative, featureSizes[0]*featureSizes[1]*featureSizes[2], 
			CV_32FC1, Scalar(0));
		Mat negativeLabel = -Mat::ones(numberOfNegative, 1, CV_32FC1);
		for (int i = 0; i < numberOfPositive; ++i) { // positive samples
			cout << "Positive HOG descriptor: " << i+1 << "\\" 
				<< numberOfPositive << endl;
			sprintf(buf, "%.6d", i+1);
			Mat im = imread("..\\Dataset\\Train\\pos\\"+string(buf)+".png", 
				CV_LOAD_IMAGE_GRAYSCALE);
			Mat tmp = ext.ExtractHOG(im);
			Mat feature(1, featureSizes[0]*featureSizes[1]*featureSizes[2], CV_32FC1, tmp.data);
			feature.copyTo(positiveDescriptor.row(i));
		}
		for (int i = 0; i < numberOfNegative; ++i) { // negative samples
			cout << "Negative HOG descriptor: " << i+1 << "\\" 
				<< numberOfNegative << endl;
			sprintf(buf, "%.6d", i+1);
			Mat im = imread("..\\Dataset\\Train\\neg\\"+string(buf)+".png", 
				CV_LOAD_IMAGE_GRAYSCALE);
			Mat tmp = ext.ExtractHOG(im);
			Mat feature(1, featureSizes[0]*featureSizes[1]*featureSizes[2], CV_32FC1, tmp.data);
			feature.copyTo(negativeDescriptor.row(i));
		}
		vconcat(positiveDescriptor, negativeDescriptor, trainSample);
		vconcat(positiveLabel, negativeLabel, trainLabel);
		FileStorage fs("..\\Descriptor\\traindata.yml", FileStorage::WRITE);
		fs << "trainSample" << trainSample << "trainLabel" << trainLabel;
		fs.release();
	} else {
		FileStorage fs("..\\Descriptor\\traindata.yml", FileStorage::READ);
		fs["trainSample"] >> trainSample;
		fs["trainLabel"] >> trainLabel;
		fs.release();
	}
	cout << "done!" << endl;


	/* Step 2: Train Linear Detector with SVM
	*/
	cout << "Step 2: Train Linear Detector with SVM..." << endl;
	Detector dt;
	if (!fopen("..\\Detector\\detector.yml", "rb")) { // Delete the detector file if you want to train a new one
		dt.TrainDetector(trainSample, trainLabel, featureSizes, params);
		Mat detector = dt.GetDetector();
		Mat saveDet(1, featureSizes[0]*featureSizes[1]*featureSizes[2], CV_32FC1, detector.data);
		FileStorage fs("..\\Detector\\detector.yml", FileStorage::WRITE);
		fs << "detector" << saveDet;
		fs.release();
	} else {
		FileStorage fs("..\\Detector\\detector.yml", FileStorage::READ);
		Mat loadDet;
		fs["detector"] >> loadDet;
		Mat detector(featureSizes[0], featureSizes[1], CV_32FC(featureSizes[2]), loadDet.data);
		dt.SetDetector(detector);
		fs.release();
	}
	cout << "done!" << endl;

	/* Step 3: Perform detection
	*/
	cout << "Step 3: Perform detection..." << endl;
	PostProcessor pro;
	Mat res;
	for (int i = 0; i < numberOfValidation; ++i) {
		cout << "Detect " << i+1 << "\\" << numberOfValidation << endl;
		sprintf(buf, "%.5d", i+1);
		Mat im = imread("..\\Dataset\\Validation\\"+string(buf)+".png", 
			CV_LOAD_IMAGE_GRAYSCALE);

//		cout << "scales[" << 0 << "] = " << scales[0] << endl;
//		cout << "threshold[" << 0 << "] = " << threshold[0] << endl;

		Mat bbox = dt.MultiscaleDetection(im, ext, scales, 
			sizeof(scales)/sizeof(float), threshold);
		Mat top = pro.NonMaxSup(bbox, overlap);
		Mat ids = i*Mat::ones(top.size[0], 1, CV_32FC1); // image id
		if (top.dims != 0) {
			Mat temp;
			hconcat(ids, top, temp); // attach the image id
			if (res.dims == 0) {
				res = temp;
			} else {
				vconcat(res, temp, res);
			}
		}

		if (drawBox == true) {
			im = imread("..\\Dataset\\Validation\\"+string(buf)+".png", 
				CV_LOAD_IMAGE_COLOR);
			dt.DisplayDetection(im, top);
		}
	}
	cout << "done!" << endl;

	/* Step 4: Evaluation the performance of detection
	*/
	cout << "Step 4: Evaluation the performance of detection..." << endl;
	vector<Mat> gt;
	for (int i = 0; i < numberOfValidation; ++i) {
		sprintf(buf, "%.5d", i+1);
		FileStorage fs("..\\Dataset\\Validation\\"+string(buf)+".yml", FileStorage::READ);

		cout << "..\\Dataset\\Validation\\" + string(buf) + ".yml" << endl;
		

		Mat groundtruth;
		fs["groundtruth"] >> groundtruth;



		fs.release();
		Mat detFlag = Mat::zeros(groundtruth.size[0], 1, CV_32FC1);

		cout <<"GT(r,c): "<< groundtruth.rows <<", "<< groundtruth.cols << endl;
		cout << "detFlag(r,c): " << detFlag.rows << ", " << detFlag.cols << endl;

		hconcat(groundtruth, detFlag, groundtruth);
		gt.push_back(groundtruth);
	}
	DetEvaluator eva;
	vector<Mat> report;
	float ap = eva.EvalDetection(res, gt, report);
	cout << "AP: " << ap << endl;
	FILE * pFile = fopen ("..\\Report\\report.txt", "wb");
	fwrite (&ap, sizeof(float), 1, pFile);
	fwrite (report[0].data, sizeof(float), report[0].total(), pFile);
	fwrite (report[1].data, sizeof(float), report[1].total(), pFile);
	fclose (pFile);
	cout << "done!" << endl;
   	return 0;
}