///////////////////////////////////////////////////////////////////////////
//
// NAME
//  BlendImages.cpp -- blend together a set of overlapping images
//
// DESCRIPTION
//  This routine takes a collection of images aligned more or less horizontally
//  and stitches together a mosaic.
//
//  The images can be blended together any way you like, but I would recommend
//  using a soft halfway blend of the kind Steve presented in the first lecture.
//
// SEE ALSO
//  BlendImages.h       longer description of parameters
//
// Copyright ?Richard Szeliski, 2001.  See Copyright.h for more details
//
///////////////////////////////////////////////////////////////////////////

#include "ImageLib/ImageLib.h"
#include "BlendImages.h"
#include "WarpSpherical.h"
#include <math.h>


#ifndef M_PI 
#define M_PI    3.1415926536f
#endif // M_PI


/******************* TO DO *********************
 * SetImageAlpha:
 *	INPUT:
 *		img: an image to be added to the final panorama
 *		blendRadius: radius of the blending function
 *	OUTPUT:
 *		set the alpha values of img
 *		pixels near the center of img should have higher alpha
 *		use blendRadius to determine how alpha decreases
 */
static void SetImageAlpha(CImg<float>& img, float blendRadius)
{
	// *** BEGIN TODO ***
	// fill in this routine..
	

	// *** END TODO ***
}

/******************* TO DO *********************
 * AccumulateBlend:
 *	INPUT:
 *		img: a new image to be added to acc
 *		acc: portion of the accumulated image where img is to be added
 *		blendRadius: radius of the blending function
 *	OUTPUT:
 *		add a weighted copy of img to the subimage specified in acc
 *		the first 3 band of acc records the weighted sum of pixel colors
 *		the fourth band of acc records the sum of weight
 */
static void AccumulateBlend(CImg<float>& img, CImg<float>& acc, float blendRadius)
{
	// *** BEGIN TODO ***
	// fill in this routine..
	
	// *** END TODO ***
}

/******************* TO DO *********************
 * NormalizeBlend:
 *	INPUT:
 *		acc: input image whose alpha channel (4th channel) contains
 *		     normalizing weight values
 *		img: where output image will be stored
 *	OUTPUT:
 *		normalize r,g,b values (first 3 channels) of acc and store it into img
 */
static void NormalizeBlend(CImg<float>& acc, CImg<float>& img)
{
	// *** BEGIN TODO ***
	// fill in this routine..
	

	// *** END TODO ***
}

/******************* TO DO *********************
 * BlendImages:
 *	INPUT:
 *		ipv: list of input images and their global positions in the mosaic
 *		f: focal length
 *		blendRadius: radius of the blending function
 *	OUTPUT:
 *		create & return final mosaic by blending all images
 */
CImg<float> BlendImages(CImagePositionV& ipv, float f, float blendRadius)
{
    // Assume all the images are of the same shape (for now)
    CImg<float>& img0 = ipv[0].img;
    //CShape sh        = img0.Shape();
    int width        = img0.width();
    int height       = img0.height();
	int nBands       = img0.spectrum();
    int dim[2]       = {width, height};

	int nTheta = (int) (2*M_PI*f + 0.5);
	int nPhi = (int) (M_PI*f + 0.5);

    // Create a floating point accumulation image
    //CShape mShape(nTheta, nPhi, nBands);
    CImg<float> accumulator(nTheta, nPhi, 1, nBands, 0);

    //accumulator.ClearPixels();

    // Add in all of the images
	for (unsigned int i = 0; i < ipv.size(); i++)
    {
        // Warp the image into spherical coordinates
        CTransform3x3 M = ipv[i].position;
        CImg<float>& src = ipv[i].img;
		SetImageAlpha(src, blendRadius);
		CImg<float> uv = WarpSphericalField(CShape(src.width(), src.height(), src.spectrum()), CShape(nTheta, nPhi, nBands), f, M);
		CImg<float>& dst = src.warp(uv);
        // Perform the accumulation
        AccumulateBlend(dst, accumulator, blendRadius);
		uv.clear();
		dst.clear();
    }
    // Normalize the results
    CImg<float> compImage(nTheta, nPhi, 1, nBands, 0);
    NormalizeBlend(accumulator, compImage);

    return compImage;
}

