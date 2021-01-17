# BadAppleSmallBasic
This project was used to play BadApple video in LEGO EV3 screen.
It includes the following two folders:

1) Resource
raw2rsf.cpp was used to convert raw PCM_u8 audio to EV3 rsf audio.
You can use g++(Mingw) to compile this file.
g++ raw2rsf.cpp -o raw2rsf
process_sound.py was used to generate a series of rsf audio files from BadApple.mp4.
It calls ffmpeg command and raw2rsf command.
process_picture.py was used to generate a series of images(EV3 rgf format) from BadApple.mp4.

2) BadApplePlayer
BadApplePlayer.sb was created by using EV3 Small Basic extension.
Use EV3Explorer to upload and compile this file.
