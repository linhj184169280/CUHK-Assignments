import numpy as np
import cv2
import flowTools as ft
# import estimateHSflow # include this package if you finish it
from matplotlib import pyplot as plt

if __name__ == '__main__':
    # read 2 input frames
    img1 = cv2.imread('./data/other-data/RubberWhale/frame10.png').astype('single')
    img2 = cv2.imread('./data/other-data/RubberWhale/frame11.png').astype('single')
    # input ground truth optical flow
    flow_gt = ft.readFlowFile('./data/other-gt-flow/RubberWhale/flow10.flo')

    img1g = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
    img2g = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)

    # flow_gt[ flow_gt > 1e9 ] = np.nan

    # parameters (adjustable)
    lam = 10
    # uncomment this line to use your own flow code
    # uv = estimateHSflow( frame1, frame2, lam ) 
    uv = flow_gt

    # warp frame2 to frame1 accroding to your flow uv
    H = img1.shape[0]
    W = img1.shape[1]
    WW, HH = np.meshgrid(range(W), range(H))
    remap = np.zeros([H, W, 2])
    remap[:, :, 0] = WW + uv[:, :, 0]
    remap[:, :, 1] = HH + uv[:, :, 1]
    remap = remap.astype('single')
    warped2 = cv2.remap(img2, remap, None, cv2.INTER_CUBIC)

    cv2.imshow('image1', img1.astype('uint8'))
    cv2.waitKey(0)
    cv2.imshow('warped image2', warped2.astype('uint8'))
    cv2.waitKey(0)
    cv2.imshow('estimate flow', ft.flowToColor(uv).astype('uint8'))
    cv2.waitKey(0)

    warped2[warped2 == np.nan] = 0

    # compute the error with groundtruch
    [aae, stdae, aepe] = ft.flowAngErr(flow_gt[:, :, 0], flow_gt[
                                       :, :, 1], uv[:, :, 0], uv[:, :, 1], 0)
    # print errors
    print '\nAAE %3.3f average EPE %3.3f \n' % (aae, aepe)

    # show the results
    plt.subplot(221), plt.imshow(
        img1[:, :, [2, 1, 0]].astype('uint8')), plt.title('Frame1')
    plt.subplot(222), plt.imshow(
        img2[:, :, [2, 1, 0]].astype('uint8')), plt.title('Frame2')
    plt.subplot(223), plt.imshow(ft.flowToColor(
        uv)[:, :, [0, 1, 2]].astype('uint8')), plt.title('Flow')
    plt.subplot(224), plt.imshow(warped2[:, :, [2, 1, 0]].astype(
        'uint8')), plt.title('Warped Image')

    plt.show()

    # save your optical flow
