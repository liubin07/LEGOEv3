#!/usr/bin/env pybricks-micropython
from pybricks.hubs import EV3Brick
from pybricks.parameters import Color
from pybricks.tools import wait, StopWatch
from pybricks.experimental import run_parallel

# This program requires LEGO EV3 MicroPython v2.0 or higher.

ev3 = EV3Brick()
ev3.speaker.set_volume(30, 'PCM')
watch = StopWatch()

def task_screen():
    idx = 0
    while True:
        ev3.screen.load_image('pic/frame%d.png' % idx)
        curms = watch.time()
        nidx = int(curms * 15 / 1000)
        idx = nidx + 1
        waitms = int(idx * 1000 / 15 - curms)
        wait(waitms)
        if idx >= 3284:
            break
    return

def task_speaker():
    ev3.speaker.play_file('BadApple.wav')
    return

run_parallel(task_screen, task_speaker)
