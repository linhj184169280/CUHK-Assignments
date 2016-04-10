#include "Detector.h"

Detector::Detector() {
}

Detector::~Detector(void) {
}

void Detector::TrainDetector(const Mat& trainSample, const Mat& trainLabel, 
	const int* featureSizes, const CvSVMParams& params) {
	this->train(trainSample, trainLabel, Mat(), Mat(), params);
	Mat tem1 = TransSV2Detector();
	Mat tmp2(featureSizes[0], featureSizes[1], CV_32FC(featureSizes[2]), tem1.data);
	tmp2.copyTo(detector);
}

Mat Detector::TransSV2Detector() {
	int numOfFeatures = this->var_all;
	int numSupportVectors = this->get_support_vector_count();
	const CvSVMDecisionFunc *dec = this->decision_func;
	Mat detector(1, numOfFeatures, CV_32FC1, Scalar(0));
	for (int i = 0; i < numSupportVectors; ++i) {
		float alpha = *(dec->alpha+i);
		const float* supportVector = this->get_support_vector(i);
		for(int j = 0; j < numOfFeatures; j++) {
			detector.at<float>(0, j) += alpha * *(supportVector+j);
		}
	}
	return -detector;
}

void Detector::DisplayDetection(Mat im, Mat top) {
	for (int i = 0 ; i < top.size[0] ; ++i) {
		rectangle(im, Point(top.at<float>(i, 1), top.at<float>(i, 0)), 
			Point(top.at<float>(i, 3), top.at<float>(i, 2)), 
			Scalar(0, 0, 255), 2);
	}
	namedWindow("Display detection", WINDOW_AUTOSIZE);
	imshow("Display detection", im); 
			
	waitKey(0);
	destroyWindow("Display detection");
}

void Detector::SetDetector(const Mat det) {
	det.copyTo(this->detector);
}

Mat Detector::GetDetector() {
	return detector;
}

/* Detection is for single scale detection
   Input:
           1. im: A grayscale image in height x width.
           2. ext: The pre-defined HOG extractor.
           3. threshold: The constant threshold to control the prediction.
   Output:
           1. bbox: The predicted bounding boxes in this format (n x 5 matrix):
                                    x11 y11 x12 y12 s1
                                    ... ... ... ... ...
                                    xi1 yi1 xi2 yi2 si
                                    ... ... ... ... ...
                                    xn1 yn1 xn2 yn2 sn
                    where n is the number of bounding boxes. For the ith 
                    bounding box, (xi1,yi1) and (xi2, yi2) correspond to its
                    top-left and bottom-right coordinates, and si is the score
                    of convolution. Please note that these coordinates are
                    in the input image im.
*/
Mat Detector::Detection(const Mat& im, HOGExtractor& ext, const float threshold) {

	/* TODO 4: 
       Obtain the HOG descriptor of the input im. And then convolute the linear
	   detector on the HOG descriptor. You may put the score of convolution in 
	   the structure "convolutionScore".
	*/
	Mat HOGDes = ext.ExtractHOG(im);
	Mat convolutionScore(HOGDes.size[0], HOGDes.size[1], CV_32FC1, Scalar(0));

	// Begin TODO 4
	Mat detector = GetDetector();
	vector<cv::Mat> channels_HOGDes(36);
	vector<cv::Mat> channels_detector(36);

	split(HOGDes, channels_HOGDes);
	split(detector, channels_detector);

	vector<cv::Mat>::iterator iter1;
	vector<cv::Mat>::iterator iter2;
	for (iter1 = channels_HOGDes.begin(), iter2 = channels_detector.begin();
		iter1 != channels_HOGDes.end(),iter2!=channels_detector.end(); iter1++, iter2++)
	{
		Mat t_res(HOGDes.size[0], HOGDes.size[1], CV_32FC1, Scalar(0));
		filter2D( (*iter1), t_res, (*iter1).depth(), (*iter2) );
		convolutionScore += t_res;
	}
//	cout << "HoGsize= " << HOGDes.size[0] << ", " << HOGDes.size[1] << endl;
//	cout << "Detectorsize= " << detector.size[0] << ", " << detector.size[1] << endl;
/*	for (int i; i < HOGDes.size[0]; i++)
	{
		for (int j; j < HOGDes.size[0]; j++)
		{
			cout << convolutionScore.at<float>(i,j) << ", ";
		}
		cout << endl;
	}
	cout << endl;*/
	// End TODO 4
	
	/* TODO 5: 
       Select out those positions where the score is above the threshold. Here,
	   the positions are in ConvolutionScore. To output the coordinates of the
	   bounding boxes, please remember to recover the positions to those in the
	   input image. Please put the predicted bounding boxes and their scores in
	   the below structure "bbox".
	*/
	Mat bbox;
	
	// Begin TODO 5
	float cells = ext.GetCells();
	float blocks = ext.GetBlocks();
	float maxScore = -5.0;
	for ( int i = 0; i < convolutionScore.size[0]; i++ )
	{
		for ( int j = 0; j < convolutionScore.size[1]; j++ )
		{
			if (convolutionScore.at<float>(i, j)>maxScore)
			{
				maxScore = convolutionScore.at<float>(i, j);
			}
			if (convolutionScore.at<float>(i, j) > threshold)
			{
				float x1, y1, x2, y2, score;
				score = convolutionScore.at<float>(i, j);
				x1 = (int)((float)i - ((float)detector.size[0] / 2)+0.5) * cells;
				y1 = (int)((float)j - ((float)detector.size[1] / 2) + 0.5) * cells;
				x2 = (int)((float)i + ((float)detector.size[0] / 2) + 0.5) * cells;
				y2 = (int)((float)j + ((float)detector.size[1] / 2) + 0.5) * cells;
				Mat t_bbox = (Mat_<float>(1, 5) << x1, y1, x2, y2, score);

				if (bbox.size[0]!=0)
				{
					vconcat(bbox, t_bbox, bbox);
				}
				else
				{
					bbox = t_bbox;	
					//bbox = Mat(t_bbox);
				}
			}
		}
	}
	cout << "Max Score = " << maxScore << endl;
	// End TODO 5
//	cout << "Here bbox = " <<bbox.rows<<", "<<bbox.cols<< endl;
	return bbox;
}


/* MultiscaleDetection is for multiscale detection
   Input:
           1. im: A grayscale image in height x width.
           2. ext: The pre-defined HOG extractor.
		   3. scales: The scales for resizeing the image.
		   4. numberOfScale: The number of different scales.
           5. threshold: The constant threshold to control the prediction.
   Output:
           1. bbox: The predicted bounding boxes in this format (n x 5 matrix):
                                    x11 y11 x12 y12 s1
                                    ... ... ... ... ...
                                    xi1 yi1 xi2 yi2 si
                                    ... ... ... ... ...
                                    xn1 yn1 xn2 yn2 sn
                    where n is the number of bounding boxes. For the ith 
                    bounding box, (xi1,yi1) and (xi2, yi2) correspond to its
                    top-left and bottom-right coordinates, and si is the score
                    of convolution. Please note that these coordinates are
                    in the input image im.
*/
Mat Detector::MultiscaleDetection(const Mat& im, HOGExtractor& ext, 
	const float* scales, const int numberOfScale, const float* threshold) {

	/* TODO 6: 
       You should firstly resize the input image by scales 
	   and store them in the structure pyra. 
	*/
	vector<Mat> pyra(numberOfScale);

	// Begin TODO 6
	int height = im.rows;
	int width = im.cols;
	int nchannel = im.channels();

	cout << scales << threshold << endl; //Active scales and threshold...Get around the bug...
	for ( int i = 0; i < numberOfScale; i++ )
	{
		int t_height = height * scales[i];
		int t_width = width * scales[i];
		Mat t_im;
		
//		cout << "t_height = " << t_height << " t_width = " << t_width << endl;
		resize( im, t_im, Size(t_width, t_height) );
		pyra[i] = t_im;
	}


	// End TODO 6
	

	/* TODO 7: 
       Perform detection with different scales. Please remember 
	   to transfer the coordinates of bounding box according to 
	   their scales. 
	   You should complete the helper-function  "Detection" and 
	   call it here. All the detected bounding boxes should be 
	   stored in the below structure "bbox".
	*/
	Mat bbox;

	// Begin TODO 7
	int i;
	for (int i=0; i<pyra.size(); i++)
	{
		Mat t_bbox = Detection( pyra[i], ext, threshold[i]);
		if (t_bbox.empty())
		{
			continue;
		}
		int t_row = t_bbox.rows;
		int t_col = t_bbox.cols;
		for (int m = 0; m < t_row; m++)
		{
			for (int n = 0; n < t_col-1; n++)
			{
				t_bbox.at<float>(m, n) = t_bbox.at<float>(m, n) / scales[i];
			}
		}
		if (!bbox.empty())
		{
			vconcat(bbox, t_bbox, bbox);
		}
		else
		{
			bbox = t_bbox;
			//bbox = Mat(t_bbox);
		}
	}
	// End TODO 7

	return bbox;
}

