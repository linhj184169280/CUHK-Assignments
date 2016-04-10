#include "HOGExtractor.h"


HOGExtractor::HOGExtractor(const int bins, const int cells, const int blocks) {
	this->bins = bins;
	this->cells = cells;
	this->blocks = blocks;
}


HOGExtractor::~HOGExtractor(void) {
}

int HOGExtractor::GetBins() {
	return this->bins;
}
int HOGExtractor::GetCells() {
	return this->cells;
}
int HOGExtractor::GetBlocks() {
	return this->blocks;
}


/* ExtractHOG is for extracting HOG descriptor of an image
   Input:
           im: A grayscale image in height x width.
   Output:
           HOGBlock: The HOG descriptor of the input image.
*/


Mat HOGExtractor::ExtractHOG(const Mat& im) {
	// Pad the im in order to make the height and width the multiplication of
    // the size of cells.
	int height = im.rows;
	int width = im.cols;

//	cout << "height&width= " << height << " " << width << endl;

	int padHeight = height % cells == 0 ? 0 : (cells - height % cells);
    int padWidth = width % cells == 0 ? 0 : (cells - width % cells);
	Mat paddedIm(height+padHeight, width+padWidth, CV_32FC1, Scalar(0));
	Range imRanges[2];
	imRanges[0] = Range(0, height);
	imRanges[1] = Range(0, width);
	im.copyTo(paddedIm(imRanges));
	height = paddedIm.rows;
	width = paddedIm.cols;

	/* TODO 1: 
       Compute the horizontal and vertical gradients for each pixel. Put them 
	   in gradX and gradY respectively. In addition, compute the angles (using
	   atan2) and magnitudes by gradX and gradY, and put them in angle and 
	   magnitude. 
	*/
	Mat hx(1, 3, CV_32FC1, Scalar(0));
	hx.at<float>(0, 0) = -1;
	hx.at<float>(0, 1) = 0;
	hx.at<float>(0, 2) = 1;
	Mat hy = -hx.t();

	Mat gradX(height, width, CV_32FC1, Scalar(0));
	Mat gradY(height, width, CV_32FC1, Scalar(0));
	Mat angle(height, width, CV_32FC1, Scalar(0));
	Mat magnitude(height, width, CV_32FC1, Scalar(0));
	float pi = 3.1416;
	
	// Begin TODO 1
	Mat ker_h = ( Mat_<float>(1,3) <<-1.0, 0, 1.0);
	Mat ker_v = ( Mat_<float>(3,1)<< 1.0, 0, -1.0 );
	filter2D(paddedIm, gradX, paddedIm.depth(), ker_h);
	filter2D(paddedIm, gradY, paddedIm.depth(), ker_v);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			angle.at<float>(i, j) = atan2(gradY.at<float>(i, j), gradX.at<float>(i, j));
			magnitude.at<float>(i, j) = sqrt( pow(gradX.at<float>(i, j), 2) + pow(gradY.at<float>(i, j), 2) );
		}
	}
    // End TODO 1
	

	/* TODO 2:
	   Construct HOG for each cells, and put them in HOGCell. numberOfVerticalCell
	   and numberOfHorizontalCell are the numbers of cells in vertical and 
	   horizontal directions.
	   You should construct the histogram according to the bins. The bins range
	   from -pi to pi in this project, and the interval is given by
	   (2*pi)/bins.
	*/
	int numberOfVerticalCell = height / cells;
	int numberOfHorizontalCell = width / cells;
	Mat HOGCell(numberOfVerticalCell, numberOfHorizontalCell, 
		CV_32FC(bins), Scalar(0));
	float piInterval = 2 * pi / bins;
	// Begin TODO 2
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (int)((angle.at<float>(i, j)+pi) / piInterval);
//			HOGCell.at<Vec<float, 9>>(i / cells, j / cells)[index]+=1.0;		
			float *ptr = HOGCell.ptr<float>(i / cells);
			*(ptr + (j / cells)*bins + index) += magnitude.at<float>(i, j);
		}
	}
	// End TODO 2

	/* TODO 3:
	   Concatenate HOGs of the cells within each blocks and normalize them.
	   The result should be stored in HOGBlock, where numberOfVerticalBlock and
	   numberOfHorizontalBlock are the number of blocks in vertical and
	   horizontal directions.
	*/
	int numberOfVerticalBlock = numberOfVerticalCell - 1;
	int numberOfHorizontalBlock = numberOfHorizontalCell - 1;
	Mat HOGBlock(numberOfVerticalBlock, numberOfHorizontalBlock, 
		CV_32FC(blocks*blocks*bins), Scalar(0));

	// Begin TODO 3
	for (int i = 0; i < numberOfVerticalBlock; i++)
	{
		for (int j = 0; j < numberOfHorizontalBlock; j++)
		{
			cv::Mat t_Mat(1, blocks*blocks*bins, CV_32FC1, Scalar(0));
			cv::Mat norm_Mat(1, blocks*blocks*bins, CV_32FC1, Scalar(0));
			for (int k = 0; k < blocks*blocks*bins; k++)
			{
				//Copy “4” cells to t_Mat
				float *ptr = t_Mat.ptr<float>(0);
				int residual = k % bins;
				int tmp = k / bins;
				int t_h = tmp / blocks;
				int t_w = tmp % blocks;

				float *ptr_cell = HOGCell.ptr<float>(i + t_h);
				*(ptr + k) = *(ptr_cell + (j + t_w)*bins + residual);
					
			}
			normalize(t_Mat, norm_Mat, 1.0, 0.0, cv::NORM_L2);

			float *ptr_block = HOGBlock.ptr<float>(i);
			float *ptr_norm = norm_Mat.ptr<float>(0);
			for (int k = 0; k < blocks*blocks*bins; k++)
			{
				if (*(ptr_norm + k) < 0.2)
					*(ptr_block + j*(blocks*blocks*bins) + k) = *(ptr_norm + k);
				else
					*(ptr_block + j*(blocks*blocks*bins) + k) = 0.2;
			}
			
		}
	}
	
	// End TODO 3

	return HOGBlock;
}