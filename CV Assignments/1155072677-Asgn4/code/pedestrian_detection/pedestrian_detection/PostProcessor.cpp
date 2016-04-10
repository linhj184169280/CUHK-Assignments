#include "PostProcessor.h"


PostProcessor::PostProcessor(void) {
}


PostProcessor::~PostProcessor(void) {
}



/* NonMaxSup is for non-maximum suppression.
   Input:
           boxes: Detection bounding boxes.
		   overlap: Overlap threshold for suppression
   Output:
           pick: The locally maximal bounding boxes of detections.
				 For a selected box Bi, all boxes Bj that are covered by
				 more than overlap are suppressed. Note that 'covered' is
				 is |Bi \cap Bj| / |Bj|, not the PASCAL intersection over 
				 union measure.
*/
Mat PostProcessor::NonMaxSup(const Mat& boxes, const float overlap) {
	if (boxes.dims == 0) {
		return Mat();
	}
	int numberOfBoxes = boxes.size[0];
	Mat x1 = boxes.col(0);
	Mat y1 = boxes.col(1);
	Mat x2 = boxes.col(2);
	Mat y2 = boxes.col(3);
	Mat s = boxes.col(4);
	Mat area(numberOfBoxes, 1, CV_32FC1, Scalar(0));
	for (int i = 0 ; i < numberOfBoxes ; ++i) {
		area.at<float>(i, 0) = (x2.at<float>(i, 0)-x1.at<float>(i, 0)+1) 
			* (y2.at<float>(i, 0)-y1.at<float>(i, 0)+1);
	}

	Mat idx;
	vector<int> I;
	sortIdx(s, idx, CV_SORT_EVERY_COLUMN+CV_SORT_ASCENDING);
	for (int i = 0 ; i < idx.rows ; ++i) {
		I.push_back(idx.at<int>(i, 0));
	}
	vector<int> pickIdx;
	vector<int> suppress;
	while (!I.empty()) {
		int last = I.size() - 1;
		int i = I[last];
		pickIdx.push_back(i);
		suppress.clear();
		suppress.push_back(last);
		for (int pos = 0 ; pos < last ; ++pos) {
			int j = I[pos];
			float xx1 = max(x1.at<float>(i, 0), x1.at<float>(j, 0));
			float yy1 = max(y1.at<float>(i, 0), y1.at<float>(j, 0));
			float xx2 = min(x2.at<float>(i, 0), x2.at<float>(j, 0));
			float yy2 = min(y2.at<float>(i, 0), y2.at<float>(j, 0));
			float h = xx2 - xx1 + 1;
			float w = yy2 - yy1 + 1;
			if (w > 0 && h > 0) {
				float o = w * h / area.at<float>(j, 0);
				if (o > overlap) {
					suppress.push_back(pos);
				}
			}
		}

		vector<int> counterPart;
		for (int idxI = 0 ; idxI < I.size() ; ++idxI) {
			bool flag = false;
			for (int sup = 0 ; sup < suppress.size() ; ++sup) {
				if (idxI == suppress[sup]) {
					flag = true;
					break;
				}
			}
			if (flag == false) {
				counterPart.push_back(I[idxI]);
			}
		}
		I = counterPart;
	}


	Mat pick(pickIdx.size(), 5, CV_32FC1, Scalar(0));
	for (int i = 0 ; i < pickIdx.size(); ++i) {
		boxes.row(pickIdx[i]).copyTo(pick.row(i));
	}

	return pick;
}