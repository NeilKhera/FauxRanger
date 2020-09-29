import cv2
import numpy
from matplotlib import pyplot as plt

cap1 = cv2.VideoCapture('Moon_(2).avi')
cap2 = cv2.VideoCapture('Moon.avi')
count = 0

while cap1.isOpened() and cap2.isOpened():
    ret1, frame1 = cap1.read()
    ret2, frame2 = cap2.read()

    if not ret1 or not ret2:
        break

    frame1_new = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
    frame2_new = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)
    
    stereo = cv2.StereoSGBM_create(minDisparity=0, numDisparities=280, blockSize=15, P1=213, P2=1500, disp12MaxDiff=30, preFilterCap=32, uniquenessRatio=11, speckleWindowSize=0, speckleRange=0)
    disparity = stereo.compute(frame1_new, frame2_new)
    print(count)
    disparity_normalized = cv2.normalize(src=disparity, dst=disparity, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX)

    cv2.imwrite("frame%d.jpg" % count, disparity_normalized)
    #if count == 2:
    #    cv2.imwrite("left.jpg", frame1_new)
    #    cv2.imwrite("right.jpg", frame2_new)
    #    break
    count = count + 1


cap1.release()
cap2.release()
cv2.destroyAllWindows()
