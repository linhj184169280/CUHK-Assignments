import numpy as np
import cv2
from matplotlib import pyplot as plt
import scipy.sparse.linalg as sslg
import flowTools as ft


def estimateHSflowlayer(frame1, frame2, uv, lam=80, maxwarping=10):
    H, W = frame1.shape

    npixels = H * W

    x, y = np.meshgrid(range(W), range(H))

    # TODO#3: build differential matrix and Laplacian matrix according to
    # image size

    # Kernel to get gradient
    h = np.array([1, -8, 0, 8, -1], dtype='single') / 12

    for i in range(maxwarping):

        # TODO#2: warp image using the flow vector
        # an example is in runFlow.py

        # TODO#4: compute image gradient Ix, Iy, and Iz

        # TODO#5: build linear system to solve HS flow
        # generate A,b for linear equation Ax = b

        ret = sslg.spsolve(A, b)
        deltauv = np.reshape(ret, uv.shape)

        deltauv(deltauv > 1) = 1
        deltauv(deltauv < -1) = -1

        uv = uv + deltauv

        print 'Warping step: %d, Incremental norm: %3.5f' %(i, np.linalg.norm(deltauv)
        # Output flow

    return uv


def estimateHSflow(frame1, frame2, lam = 80):
    H, W = frame1.shape

    # build the image pyramid
    pyramid_spacing = 1.0 / 0.8
    pyramid_level = 1 + np.floor(np.log(min(W, H) / 16.0) / np.log(pyramid_spacing * 1.0))
    smooth_sigma = sqrt(2.0)
    # f = ft.fspecialGauss(2 * round(1.5 * smooth_sigma) + 1, smooth_sigma)

    pyramid1 = []
    pyramid2 = []

    pyramid1.append(frame1)
    pyramid2.append(frame2)

    for m in range(1, pyramid_levels):
        # TODO #1: build Gaussian pyramid for coarse-to-fine optical flow
        # estimation

    # coarst-to-fine compute the flow
    uv = np.zeros(((H, W, 2)))

    for levels in range(pyramid_levels - 1, -1, -1):
        print "level %d" % (levels)
        H1, W1 = pyramid1[levels].shape
        uv = cv2.resize(uv, (H1, W1))
        uv = estimateHSflowlayer(pyramid1[levels], pyramid2[levels], uv, lam)
