import scipy.io as sio
import numpy as np
import cv2
from matplotlib import pyplot as plt

def unSharpmask(im, K, A):
    # TODO Write "unsharp masking" function based on the illustration in specification.
    # Return sharpened result image
    result =
    return result


if __name__ == '__main__':
    im = cv2.imread('./misc/lena_gray.bmp')
    A = 2
    k = np.array([[-1, -1, -1], [-1, 8, -1], [-1, -1, -1]]) # generate Laplacian operator

    result = unSharpmask(im, k, A)
    # Show result, OpenCV is BGR-channel, matplotlib is RGB-channel
    # Or: 
    #   cv2.imshow('output',result)
    #   cv2.waitKey(0)
    plt.imshow(result);
    plt.show()