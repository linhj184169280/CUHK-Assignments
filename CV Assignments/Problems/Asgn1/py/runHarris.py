import scipy.io as sio
import numpy as np
import cv2
from matplotlib import pyplot as plt

def harrisdetector(image, k, t):
    # TODO Write harrisdetector function based on the illustration in specification.
    # Return corner points x-coordinates in result[0] and y-coordinates in result[1]
    ptr_x = []
    ptr_y = []
    

    result = [ptr_x, ptr_y]
    return result

if __name__ == '__main__':
    k = 2       # change to your value
    t = 2e6     # change to your value

    I = cv2.imread('./misc/corner_gray.png')

    fr = harrisdetector(I, k, t)

    # Show input, OpenCV is BGR-channel, matplotlib is RGB-channel
    # Or: 
    #   cv2.imshow('output',out)
    #   cv2.waitKey(0)
    plt.imshow(I)
    # plot harris points overlaid on input image
    plt.scatter(x=fr[0], y=fr[1], c='r', s=40) 

    # show
    plt.show()							
