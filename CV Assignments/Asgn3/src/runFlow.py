import numpy as np
import cv2
import flowTools as ft
import estimateHSflow # include this package if you finish it
from matplotlib import pyplot as plt
import sys
import os
import time

if __name__ == '__main__':
    # read 2 input frames
    path1 = './data/other-data/Hydrangea/frame10.png'
    path2 = './data/other-data/Hydrangea/frame11.png'
    path_flow = './data/other-gt-flow/Hydrangea/flow10.flo'
    path_outputDir = 'D:/'
    outflow_name = 'Hydrangea.flo'
    outtxt_name = 'report.txt'
    
#     path1 = sys.argv[1]
#     path2 = sys.argv[2]
#     path_flow = sys.argv[3]
#     path_outputDir = sys.argv[4]
#     outflow_name = sys.argv[5]
#     outtxt_name = sys.argv[6]
    
    file = open(path_outputDir + outtxt_name , 'w')
    
    img1 = cv2.imread(path1).astype('single')
    img2 = cv2.imread(path2).astype('single')
    # input ground truth optical flow
    flow_gt = ft.readFlowFile(path_flow)
    

    
#     flow_gt = ft.readFlowFile('D:/myflow10.flo')

    img1g = cv2.cvtColor(img1, cv2.COLOR_BGR2GRAY)
    img2g = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)

    # flow_gt[ flow_gt > 1e9 ] = np.nan

    # parameters (adjustable)
    lam = 10

    t1 = time.clock()

    # uncomment this line to use your own flow code
    uv = estimateHSflow.estimateHSflow( img1g, img2g, lam ) 
#     uv = flow_gt

    t2 = time.clock()
    time_consumption = t2 - t1
    file.write('time = %.2f    ' %(time_consumption))

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

    # compute the error with groundtruch0
    # ground truth flow with value > 1e9 do no need to consider
    flow_gt[ flow_gt > 1e5 ] = 0.0
    uv[ flow_gt > 1e5  ] = 0.0
    [aae, stdae, aepe] = ft.flowAngErr(flow_gt[:, :, 0], flow_gt[
                                       :, :, 1], uv[:, :, 0], uv[:, :, 1], 0)
    # print errors
    print '\nAAE %3.3f average EPE %3.3f \n' % (aae, aepe)
    file.write('\nAAE %3.3f average EPE %3.3f \n\n' % (aae, aepe))
    
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
    ft.writeFlowFile(uv, path_outputDir + outflow_name)