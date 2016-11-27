import numpy as np
import cv2
from matplotlib import pyplot as plt
import scipy.sparse.linalg as sslg
import scipy.sparse as ss
import flowTools as ft
import scipy.ndimage as af



def estimateHSflowlayer(frame1, frame2, uv, lam=80, maxwarping=10):
    H, W = frame1.shape

    npixels = H * W

    w_mesh, h_mesh = np.meshgrid(range(W), range(H))

    # TODO#3: build differential matrix and Laplacian matrix according to
    # image size
    e = np.ones([npixels,1])
    e2 = np.concatenate((-e,e))
    Dy = ss.spdiags(e2.reshape(2,e2.shape[0]/2), [0,1], npixels, npixels)
    Dx = ss.spdiags(e2.reshape(2,e2.shape[0]/2), [0,H], npixels, npixels)
    L = Dx.transpose().dot(Dx)+Dy.transpose().dot(Dy)
    # please use order 'F' when doing reshape: np.reshape(xx, xxshape, order='F') 

    # Kernel to get gradient
    h = np.array( [[1, -8, 0, 8, -1]], dtype='single' ) / 12
    remap = np.zeros([H, W, 2])
    for i in range(maxwarping):

        # TODO#2: warp image using the flow vector
        # an example is in runFlow.py
        remap[:,:,0] = w_mesh + uv[:,:,0]
        remap[:,:,1] = h_mesh + uv[:,:,1]
        remap = remap.astype('single')
        warped2 = cv2.remap(frame2, remap, None, cv2.INTER_CUBIC)

        # TODO#4: compute image gradient Ix, Iy, and Iz
        Ix = cv2.filter2D( warped2, -1, h )
        Iy = cv2.filter2D( warped2, -1, h.transpose() )
        Iz = warped2-frame1
        Iz_vec = Iz.flatten(order = 'F')
        Iz_vec = ss.csc_matrix(Iz_vec).transpose()
        # TODO#5: build linear system to solve HS flow
        # generate A,b for linear equation Ax = b
        # you may need use scipy.sparse.spdiags
        Ix_Diag = ss.spdiags(Ix.flatten(order = 'F'), [0], npixels, npixels)
        Iy_Diag = ss.spdiags(Iy.flatten(order = 'F'), [0], npixels, npixels)
        U = uv[:,:,0].flatten(order = 'F')
        U = ss.csc_matrix(U).transpose()
        V = uv[:,:,1].flatten(order = 'F')
        V = ss.csc_matrix(V).transpose()
        
        A11 = Ix_Diag.dot(Ix_Diag)+lam*L
        A12 = Ix_Diag.dot(Iy_Diag)
        A21 = A12
        A22 = Iy_Diag.dot(Iy_Diag)+lam*L
        A = ss.vstack( [ss.hstack([A11,A12]), ss.hstack([A21,A22])] )
        A = ss.csc_matrix(A)
        A.astype('float32')
        b1 = Ix_Diag.dot(Iz_vec)+lam*L.dot(U)
        b2 = Iy_Diag.dot(Iz_vec)+lam*L.dot(V)
        b = -ss.vstack( [b1, b2] )
        b.astype('float32')

        ret = sslg.spsolve(A, b)
        deltauv = np.reshape(ret, uv.shape, order='F')

        deltauv[deltauv is np.nan] = 0
        deltauv[deltauv > 1] = 1
        deltauv[deltauv < -1] = -1

        uv = uv + deltauv

        print 'Warping step: %d, Incremental norm: %3.5f' %(i, np.linalg.norm(deltauv))
        # Output flow
    return uv


def estimateHSflow(frame1, frame2, lam = 80):
    H, W = frame1.shape

    # build the image pyramid
    pyramid_spacing = 1.0 / 0.8
#    pyramid_levels = 1 + np.floor(np.log(min(W, H) / 16.0) / np.log(pyramid_spacing * 1.0))
#    pyramid_levels = int(pyramid_levels)
    pyramid_levels = 4
    smooth_sigma = np.sqrt(2.0)

    pyramid1 = []
    pyramid2 = []

    pyramid1.append(frame1)
    pyramid2.append(frame2)
    
    t_H = H
    t_W = W
    
    gau_frame1 = frame1
    gau_frame2 = frame2

    for m in range(1, pyramid_levels):
        # TODO #1: build Gaussian pyramid for coarse-to-fine optical flow
        # estimation
        # use cv2.GaussianBlur
        t_H = int(t_H*0.8)
        t_W = int(t_W*0.8)
        gau_frame1 = cv2.GaussianBlur(gau_frame1,(3,3),smooth_sigma)
        gau_frame2 = cv2.GaussianBlur(gau_frame2,(3,3),smooth_sigma)
        gau_frame1 = cv2.resize(gau_frame1,(t_W,t_H))
        gau_frame2 = cv2.resize(gau_frame2,(t_W,t_H))                        
        pyramid1.append(gau_frame1)
        pyramid2.append(gau_frame2)
    # coarst-to-fine compute the flow
    uv = np.zeros(((H, W, 2)))

    for levels in range(pyramid_levels - 1, -1, -1):
        print "level %d" % (levels)
        H1, W1 = pyramid1[levels].shape
        uv = cv2.resize(uv, (W1, H1))
        uv = estimateHSflowlayer(pyramid1[levels], pyramid2[levels], uv, lam)

        # TODO #6: use median filter to smooth the flow result in each level in each iteration
        # 
    	uv[:,:,0] = af.median_filter(uv[:,:,0], 7)
    	uv[:,:,1] = af.median_filter(uv[:,:,1], 7)
#         uv = uv.astype('uint8')
#         c1,c2 = cv2.split(uv)
#         c1 = cv2.medianBlur(c1, 3)
#         c2 = cv2.medianBlur(c2, 3)
#         uv = cv2.merge([c1,c2])
#         uv = uv.astype('float')
    
    return uv