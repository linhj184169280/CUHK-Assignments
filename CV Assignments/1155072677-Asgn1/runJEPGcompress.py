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
    h8_t = h8 if H%8 is 0 else h8+1
    w8_t = w8 if W%8 is 0 else w8+1    
    im_t = np.zeros((h8_t*8,w8_t*8,1),dtype=np.float32)
    im_t[:H,:W,0] = im[:,:,0]*0.114+im[:,:,1]*0.587+im[:,:,2]*0.299
    # TODO Separate the image into blocks, and compress the blocks via quantization DCT coefficients
    DCT_arr = []
    for i in xrange(h8_t):
        list = []
        for j in xrange(w8_t):
            tmp = (cv2.dct(im_t[i*8:(i+1)*8,j*8:(j+1)*8,:])/quantmatrix+0.5)
            tmp = tmp.astype(np.uint8)
            tmp = tmp.astype(np.float32)
            list.append( tmp )
        DCT_arr.append(list)

    # TODO Convert back from DCT domain to RGB image
    for i in xrange(h8_t):
        for j in xrange(w8_t):
            im_t[i*8:(i+1)*8,j*8:(j+1)*8,0] = cv2.idct( DCT_arr[i][j] )
    
    result =np.zeros((h8_t*8,w8_t*8,3),dtype=np.float32)
    result[:,:,0] = result[:,:,1] = result[:,:,2] = im_t[:,:,0]
    result=result.astype(np.uint8)
    return result

if __name__ == '__main__':

    im = cv2.imread('./misc/lena_gray.bmp')
#    im = cv2.imread('/Users/linhuangjing/MyWorkspace/Assignments/Computer Vision/py/misc/lena_gray.bmp')
    im.astype('float')
    
    quantmatrix = sio.loadmat('./misc/quantmatrix.mat')['quantmatrix']
#    quantmatrix = sio.loadmat('/Users/linhuangjing/MyWorkspace/Assignments/Computer Vision/py/misc/quantmatrix.mat')['quantmatrix']

    out = jpegCompress(im, quantmatrix)
    # Show result, OpenCV is BGR-channel, matplotlib is RGB-channel
    # Or: 
#    cv2.imshow('output',out)
#    cv2.waitKey(0)
    plt.imshow(out)
    plt.show()

