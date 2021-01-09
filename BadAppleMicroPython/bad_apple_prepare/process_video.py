import sys
import cv2

o_fps = 15
vc = cv2.VideoCapture("BadApple.mp4")
if vc.isOpened():
    i_fps = vc.get(cv2.CAP_PROP_FPS)
    num = int(i_fps/o_fps)
    f_i = 0
    f_o = 0
    first_data = True
    while True:
        rval, frame = vc.read()
        if not rval:
            break
        if f_i % num == 0:
            # LEGO screen: 177 * 127
            # output size: 169 * 127
            resizeImage = cv2.resize(frame, (169, 127))
            grayImage = cv2.cvtColor(resizeImage, cv2.COLOR_BGR2GRAY)
            ret, bImage2 = cv2.threshold(grayImage, 127, 255, cv2.THRESH_BINARY)
            # generate picture for animation
            cv2.imwrite("pic\\frame%d.png" % f_o, bImage2)

            f_o = f_o + 1
        f_i = f_i + 1

    print("Done.")
else:
    print("Fail to open video file!")
vc.release()
