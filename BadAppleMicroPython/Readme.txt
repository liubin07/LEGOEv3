# BadAppleMicroPython
This project was used to play BadApple video in LEGO EV3 screen.
It includes the following two folders:

1) bad_apple_prepare
process_video.py was used to generate a series of images from BadApple.mp4.
By using opencv functions, this python program captured 15 images per seconds,
and converted these images to 169*127 binary gray image to adapt to EV3 screen's
resolution 177 * 127.
To seperate the sound file, we can run the bellow ffmpeg command.

ffmpeg -i BadApple.mp4 -f wav BadApple.wav

2) bad_apple
This folder was created by using vscode + MicroPython for LEGO EV3 extension.
Run main.py to play the animation with sound. 
