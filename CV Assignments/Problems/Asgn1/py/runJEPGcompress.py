import scipy.io as sio
import numpy as np
import cv2
from matplotlib import pyplot as plt

def jpegCompress(image, quantmatrix):
    '''
        Compress(imagefile, quanmatrix simulates the lossy compression of 
        baseline JPEG, by quantizing the DCT coefficients in 8x8 blocks
    '''
    # Return compressed image in result
    
    H = np.size(image, 0)
    W = np.size(image, 1)

    # Number of 8x8 blocks in the height and width directions
    h8 = H / 8
    w8 = W / 8
    
    # TODO If not an integer number of blocks, pad it with zeros

    # TODO Separate the image into blocks, and compress the blocks via quantization DCT coefficients

    # TODO Convert back from DCT domain to RGB image
    
    
    return result

if __name__ == '__main__':

    im = cv2.imread('./misc/lena_gray.bmp')
    im.astype('float')
    
    quantmatrix = sio.loadmat('./misc/quantmatrix.mat')['quantmatrix']

    out = jpegCompress(im, quantmatrix)

    # Show result, OpenCV is BGR-channel, matplotlib is RGB-channel
    # Or: 
    #   cv2.imshow('output',out)
    #   cv2.waitKey(0)
    plt.imshow(out)
    plt.show()

