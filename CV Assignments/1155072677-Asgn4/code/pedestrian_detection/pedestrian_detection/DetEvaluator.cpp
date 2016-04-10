#include "DetEvaluator.h"


DetEvaluator::DetEvaluator(void) {
}


DetEvaluator::~DetEvaluator(void) {
}



float DetEvaluator::EvalDetection(Mat res, vector<Mat> gt, vector<Mat>& report) {
	if (res.dims == 0) {
		return 0;
	}

	Mat si;
	sortIdx(res.col(5), si, CV_SORT_EVERY_COLUMN+CV_SORT_DESCENDING); // Sort the scores descendingly.

	int nd = res.size[0];
	Mat tp = Mat::zeros(nd, 1, CV_32FC1);
	Mat fp = Mat::zeros(nd, 1, CV_32FC1);
	
	for (int d = 0 ; d < nd ; ++d) {
		int i = res.at<float>(si.at<int>(d, 0), 0); // image id
		Mat bb = res.row(si.at<int>(d, 0));
		float ovmax = -FLT_MAX;
		float jmax = 0;
		for (int j = 0 ; j < gt[i].size[0] ; ++j) {
			Mat bbgt = gt[i].row(j);
			float bi[4] = {std::max(bb.at<float>(0, 1), bbgt.at<float>(0, 0)),
						   std::max(bb.at<float>(0, 2), bbgt.at<float>(0, 1)),
						   std::min(bb.at<float>(0, 3), bbgt.at<float>(0, 2)),
						   std::min(bb.at<float>(0, 4), bbgt.at<float>(0, 3))};
			float ih = bi[2] - bi[0] + 1;
			float iw = bi[3] - bi[1] + 1;
			if (ih > 0 && iw > 0) {
				float ua = (bb.at<float>(0, 3)-bb.at<float>(0, 1)+1)*(bb.at<float>(0, 4)-bb.at<float>(0, 2)+1)+
						   (bbgt.at<float>(0, 2)-bbgt.at<float>(0, 0)+1)*(bbgt.at<float>(0, 3)-bbgt.at<float>(0, 1)+1)-
						   iw*ih;
				float ov = iw*ih/ua;
				if (ov > ovmax) {
					ovmax = ov;
					jmax = j;
				}
			}
		}
		if (ovmax >= 0.5) {
			if (gt[i].row(jmax).at<float>(0, 4) == 0) {
				tp.at<float>(d, 0) = 1;
				gt[i].row(jmax).at<float>(0, 4) = 1; // relabel the det flag
			} else {
				fp.at<float>(d, 0) = 1;
			}
		} else {
			fp.at<float>(d, 0) = 1;
		}
	}

	float npos = 0;
	for (int i = 0 ; i < gt.size() ; ++i) {
		npos += (float)gt[i].size[0];
	}

	fp = Cumsum(fp);
	tp = Cumsum(tp);
	Mat rec = tp / npos;
	Mat prec = tp;
	for (int i = 0 ; i < prec.size[0] ; ++i) {
		prec.at<float>(i, 0) /= (fp.at<float>(i, 0)+tp.at<float>(i, 0));
	}
	report.push_back(rec);
	report.push_back(prec);
	return ComputeAP(rec, prec);
}

/* Cumsum is for calculating the cumulative of the col-vector in
*/
Mat DetEvaluator::Cumsum(const Mat& in) {
	Mat out(in.size[0], 1, CV_32FC1, Scalar(0));
	for (int i = 0 ; i < in.size[0] ; ++i) {
		if (i == 0) {
			out.at<float>(i, 0) += in.at<float>(i, 0);
		} else {
			out.at<float>(i, 0) += (out.at<float>(i-1, 0)+in.at<float>(i, 0));
		}
	}
	return out;
}

/* ComputeAP is for computing the AP
*/
float DetEvaluator::ComputeAP(const Mat& rec, const Mat& prec) {
	Range rois[2];

	Mat mrec(rec.size[0]+2, 1, CV_32FC1, Scalar(0));
	mrec.at<float>(0, 0) = 0;
	mrec.at<float>(rec.size[0]+1, 0) = 1;
	rois[0] = Range(1, rec.size[0]+1);
	rois[1] = Range(0, 1);
	rec.copyTo(mrec(rois));

	Mat mpre(prec.size[0]+2, 1, CV_32FC1, Scalar(0));
	mpre.at<float>(0, 0) = 0;
	mpre.at<float>(prec.size[0]+1, 0) = 0;
	rois[0] = Range(1, prec.size[0]+1);
	rois[1] = Range(0, 1);
	prec.copyTo(mpre(rois));

	for (int i = mpre.size[0]-2 ; i >= 0 ; --i) {
		mpre.at<float>(i, 0) = std::max(mpre.at<float>(i, 0), mpre.at<float>(i+1, 0));
	}

	vector<int> idx;
	for (int i = 0 ; i < mrec.size[0] - 1 ; ++i) {
		if (mrec.at<float>(i, 0) != mrec.at<float>(i+1, 0)) {
			idx.push_back(i+1);
		}
	}

	float ap = 0;
	for (int i = 0 ; i < idx.size() ; ++i) {
		ap += (mrec.at<float>(idx[i], 0)-mrec.at<float>(idx[i]-1, 0))*mpre.at<float>(idx[i], 0);
	}

	return ap;
}