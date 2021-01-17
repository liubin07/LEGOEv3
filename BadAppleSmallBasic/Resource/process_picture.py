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
            # LEGO screen: 178 * 128
            # output size: 176 * 128
            resizeImage = cv2.resize(frame, (176, 128))
            grayImage = cv2.cvtColor(resizeImage, cv2.COLOR_BGR2GRAY)
            ret, bImage = cv2.threshold(grayImage, 127, 255, cv2.THRESH_BINARY)
            # generate picture for animation
            cv2.imwrite("pic/frame%d.bmp" % f_o, bImage)
            # output rgf file
            # 176->0xb0, 128->0x80
            header = b"\xb0\x80"
            one_byte = 0
            with open("picture/frame%d.rgf" % f_o, "wb") as outfile:
                outfile.write(header)
                for i in range(0, 128):
                    for j in range(0, 176):
                        if j % 8 == 0 and not (i == 0 and j == 0):
                            outfile.write(one_byte.to_bytes(1, byteorder='big'))
                            one_byte = 0
                        bit = 1
                        if bImage[i,j] == 255:
                            bit = 0
                        one_byte = one_byte + (bit << (j % 8))
                outfile.write(one_byte.to_bytes(1, byteorder='big'))
                outfile.close()
            # end output rgf file

            f_o = f_o + 1
        f_i = f_i + 1

    print("Done.")
else:
    print("Fail to open video file!")
vc.release()
