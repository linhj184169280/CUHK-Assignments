///////////////////////////////////////////////////////////////////////////
//
// NAME
//  FeatureAlign.h -- image registration using feature matching
//
// SEE ALSO
//  FeatureAlign.h      longer description
//
// Copyright ?Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#include "ImageLib/ImageLib.h"
#include "FeatureAlign.h"
#include "P3Math.h"
#include <math.h>
#include <time.h>
#include <set>

using namespace std;

/******************* TO DO *********************
 * alignImagePair:
 *	INPUT:
 *		f1, f2: source feature sets
 *		matches: correspondences between f1 and f2
 *		m: motion model
 *		f: focal length
 *		width: image width
 *		height: image height
 *		nRANSAC: number of RANSAC iterations
 *		RANSACthresh: RANSAC distance threshold
 *		M: transformation matrix (output)
 *	OUTPUT:
 *		repeat for nRANSAC iterations:
 *			choose a minimal set of feature matches
 *			estimate the transformation implied by these matches
 *			count the number of inliers
 *		for the transformation with the maximum number of inliers,
 *		compute the least squares motion estimate using the inliers,
 *		and store it in M
 */
vector<int> RandomNumArr(const vector<FeatureMatch> &matches)
{
	srand((unsigned)time(NULL));
	vector<int> RNums;
	for (int i = 0; i < 10000; i++)
	{
		RNums.push_back(rand() % matches.size());
	}
	return RNums;
}

int RandomInliersSelect(const vector<int>& RNums, int offPos,
	const vector<FeatureMatch>& matches,
	vector<int>& inliers,
	set<int>& visitedPair)
{
	inliers.clear();
	int fpos1, fpos2;
	int i;
	for (i = offPos; i < (int)RNums.size(); i += 2)
	{
		fpos1 = RNums[i];
		fpos2 = RNums[i + 1];

		set<int>::iterator iter = visitedPair.find(fpos1 * 1000000 + fpos2);
		if ((iter != visitedPair.end()) || (fpos1 == fpos2) ||
			(matches[fpos1].id < 0) || (matches[fpos2].id < 0))
		{
			continue;
		}
		else
		{
			inliers.push_back(fpos1);
			inliers.push_back(fpos2);
			visitedPair.insert(fpos1 * 1000000 + fpos2);
			break;
		}
	}

	return i + 2;
}

int alignImagePair(const FeatureSet &f1, const FeatureSet &f2,
			  const vector<FeatureMatch> &matches, MotionModel m,
			  float f, int width, int height,
			  int nRANSAC, double RANSACthresh, CTransform3x3& M, vector<int> &inliers)
{
	// BEGIN TODO
	// write this entire method
	vector< vector<int> > vInliers;
	set<int> visitedPair;
	vector<int> RNums = RandomNumArr(matches);
	int t_offset = 0;

	for (int i = 0; i < nRANSAC; i++)
	{
		vector<int> t_inliers;
		t_offset = RandomInliersSelect(RNums, t_offset, matches, t_inliers, visitedPair);
		if (t_inliers.size() < 2)
			break;

		CTransform3x3 Mtmp;
		leastSquaresFit(f1, f2, matches, m, f, width, height, t_inliers, Mtmp);

		countInliers(f1, f2, matches, m, f, width, height, Mtmp, RANSACthresh, t_inliers);

		vInliers.push_back(t_inliers);
	}

	int tmax = -1, maxPos;
	for (size_t j = 0; j < vInliers.size(); j++)
	{
		if (tmax < (int)vInliers[j].size())
		{
			tmax = vInliers[j].size();
			maxPos = j;
		}
	}

	if (vInliers[maxPos].size() < 2)
	{
		inliers = vInliers[maxPos];
		return 0;
	}

	leastSquaresFit(f1, f2, matches, m, f, width, height, vInliers[maxPos], M);
	inliers = vInliers[maxPos];
	// END TODO

	return 0;
}

/******************* TO DO *********************
 * countInliers:
 *	INPUT:
 *		f1, f2: source feature sets
 *		matches: correspondences between f1 and f2
 *		m: motion model
 *		f: focal length
 *		width: image width
 *		height: image height
 *		M: transformation matrix
 *		RANSACthresh: RANSAC distance threshold
 *		inliers: inlier feature IDs
 *	OUTPUT:
 *		transform the features in f1 by M
 *
 *		count the number of features in f1 for which the transformed
 *		feature is within Euclidean distance RANSACthresh of its match
 *		in f2
 *
 *		store these features IDs in inliers
 *
 *		this method should be similar to evaluateMatch from project 1,
 *		except you are comparing each distance to a threshold instead
 *		of averaging them
 */
int countInliers(const FeatureSet &f1, const FeatureSet &f2,
				 const vector<FeatureMatch> &matches, MotionModel m,
				 float f, int width, int height,
				 CTransform3x3 M, double RANSACthresh, vector<int> &inliers)
{
	inliers.clear();
	int count = 0;

	for (unsigned int i=0; i<f1.size(); i++) {
		// BEGIN TODO
		// determine if the ith feature in f1, when transformed by M,
		// is within RANSACthresh of its match in f2 (if one exists)
		//
		// if so, increment count and append i to inliers
		if (matches[i].id < 0)
			continue;

		CVector3 p1, p2;
		p1[0] = f1[i].x - width / 2.0;	p1[1] = f1[i].y - height / 2.0;	p1[2] = f;
		p2 = M * p1;

		double xNew = p2[0] + width / 2.0;
		double yNew = p2[1] + height / 2.0;
		int f2pos = matches[i].id - 1;
		double dist = sqrt(pow(xNew - f2[f2pos].x, 2) + pow(yNew - f2[f2pos].y, 2));
		if (dist < RANSACthresh)
		{
			inliers.push_back(i);
			count++;
		}
		// END TODO
	}

	return count;
}

/******************* TO DO *********************
 * leastSquaresFit:
 *	INPUT:
 *		f1, f2: source feature sets
 *		matches: correspondences between f1 and f2
 *		m: motion model
 *		f: focal length
 *		width: image width
 *		height: image height
 *		inliers: inlier feature IDs
 *		M: transformation matrix (output)
 *	OUTPUT:
 *		compute the transformation from f1 to f2 using only the inliers
 *		and return it in M
 */
int leastSquaresFit(const FeatureSet &f1, const FeatureSet &f2,
					const vector<FeatureMatch> &matches, MotionModel m,
					float f, int width, int height,
					const vector<int> &inliers, CTransform3x3& M)
{
	// BEGIN TODO
	// write this entire method
	CTransform3x3 Mat, t_Mat;
	CVector3 p1, p2;

	//Initial the Mat
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			Mat[i][j] = t_Mat[i][j] = 0;
		}

	for (size_t i = 0; i < inliers.size(); i++)
	{
		int f1pos = inliers[i];
		int f2pos = matches[f1pos].id - 1;
		p1[0] = f1[f1pos].x - width / 2.0;	p1[1] = f1[f1pos].y - height / 2.0;	p1[2] = f;
		p2[0] = f2[f2pos].x - width / 2.0;	p2[1] = f2[f2pos].y - height / 2.0;	p2[2] = f;
		t_Mat = p1 * p2;
		Mat = Mat + t_Mat;
	}

	CTransform3x3 u, s, v;
	svd(Mat, u, s, v);

	M = v * u.Transpose();


	// END TODO

	return 0;
}
