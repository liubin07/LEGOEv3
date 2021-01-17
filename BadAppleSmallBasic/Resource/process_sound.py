import os
import subprocess
import datetime

# video len is 219s, each sound file is 8s, totally 28 files.

start = datetime.datetime(2000, 1, 1, 0, 0, 0, 0)
for i in range(0, 28):
    args = '-i %s -acodec pcm_u8 -f u8 -ar 8000 -ac 1 -ss %s -t 8 %d.raw' %\
        ('BadApple.mp4', start.strftime('%H:%M:%S'), i)
    subprocess.call('ffmpeg ' + args)
    subprocess.call('raw2rsf %d.raw sound/%d.rsf' % (i, i))
    os.remove('%d.raw ' % i)
    start = start + datetime.timedelta(seconds = 8)
