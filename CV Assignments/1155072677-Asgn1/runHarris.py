import scipy.io as sio
import numpy as np
import cv2
from matplotlib import pyplot as plt

def harrisdetector(image, k, t):
    # TODO Write harrisdetector function based on the illustration in specification.
    # Return corner points x-coordinates in result[0] and y-coordinates in result[1]
    ptr_x = []
    ptr_y = []
    
    Im= np.zeros((image.shape[0],image.shape[1],1),dtype=np.float32)
    Im[:,:,0] = image[:,:,0]*0.114+image[:,:,1]*0.587+image[:,:,2]*0.299
    Im.astype(np.float)
    Im = np.pad(Im, ((k, k), (k, k), (0,0)),'edge') 
    
    Weight = np.ones((2*k+1,2*k+1),dtype=np.float)/9
    horrizonKer = np.array([[1.0,0.0,-1.0]],dtype=np.float)
    verticalKer = np.array([[1.0],[0.0],[-1.0]])
    image_Blur = cv2.filter2D(Im ,-1, Weight)
    
    I_x = cv2.filter2D(image_Blur, -1, horrizonKer)
    I_y = cv2.filter2D(image_Blur, -1, verticalKer)
    
    I_xx = cv2.filter2D(I_x, -1, horrizonKer)
    I_yy = cv2.filter2D(I_y, -1, verticalKer)
    I_xy = cv2.filter2D(I_x, -1, verticalKer)
    
    I_xx = I_xx[k : -k, k : -k]
    I_yy = I_yy[k : -k, k : -k]   
    I_xy = I_xy[k : -k, k : -k]   
    
    for i in xrange(image.shape[0]):
        for j in xrange(image.shape[1]):
            A = np.zeros((2,2,1),dtype=np.float)
            A[0,0,0] = I_xx[i,j]
            A[0,1,0] = I_xy[i,j]
            A[1,0,0] = I_xy[i,j]
            A[1,1,0] = I_yy[i,j]
            
            a,eigens,vectors = cv2.eigen(A,True)
            if eigens[0,0]>t and eigens[1,0]>t:
                ptr_x.append(j)
                ptr_y.append(i)

    result = [ptr_x, ptr_y]
    return result

if __name__ == '__main__':
    k = 1       # change to your value
    t = 2e1     # change to your value

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
