# python simplebeep.py &
# coding: utf-8

import RPi.GPIO as GPIO
import time

SOUNDER = 26

GPIO.setmode(GPIO.BCM)
GPIO.setup(SOUNDER, GPIO.OUT, initial = GPIO.LOW)

Hz = 100
p = GPIO.PWM(SOUNDER, 1)

p.start(50)

for i in range(19, 20) :

    p.ChangeFrequency(i * Hz)
    time.sleep(0.1)

p.stop()
GPIO.cleanup()
