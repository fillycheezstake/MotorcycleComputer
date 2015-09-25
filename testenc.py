#!/usr/bin/env python
#

#This was a simple heated jacket controller using a raspberry pi, in python.
#Created by Philip Ahlers, using Adafruit Libraries like the CharLCD
#also takes advantage of some rudimentary threading! 

#
import sys
import time
from rotary_class import RotaryEncoder
from Queue import Queue
from threading import Thread

# these are for my own LCD driver I wrote - I'm using Adafruit's for now
# import LCDDriver
# lcd = LCDDriver.lcddriver()


from Adafruit_CharLCD import Adafruit_CharLCD


# Create LCD, passing in MCP GPIO adapter.
lcd = Adafruit_CharLCD(1, 2, 3, 4, 5, 6, 16, 2, 7, False)

# set up GPIO for the jacket output

import RPi.GPIO as GPIO  # import RPi.GPIO module

GPIO.setwarnings(False)

GPIO.setmode(GPIO.BCM)

GPIO.setup(21, GPIO.OUT)  # set port/pin as an output
GPIO.setup(21, GPIO.OUT, initial=0)

p = GPIO.PWM(21, .5)

p.start(50)

theq = Queue()

jacket_dc = 50

lcd.message('   Jacket: ')



# this function will be a worker thread to write jacket status to the screen
def WriteToScreen():
    while True:
        j = theq.get()  # grab a commanded value off the queue
        p.ChangeDutyCycle(j)
        print('worker:' + str(j))  # simply a debug print for the terminal
        lcd.set_cursor(11, 0)
        lcd.message(str(j) + '% \n')
        screenhelp = j / (6.25)
        screenint = int(round(screenhelp))
        lcd.set_cursor(0, 1)
        for i in range(screenint):
            lcd.message(chr(255))
        x = 16
        for i in range(16 - screenint):
            lcd.set_cursor(x, 1)
            lcd.message(' ')
            x = x - 1

        theq.task_done()  # mark this task as done

# this starts ONE worker thread.  The screen can only handle one
# the queue MUST be done in order (for the screen to look right)
worker = Thread(target=WriteToScreen)
worker.setDaemon(True)
worker.start()


# Define GPIO inputs for left encoder
PIN_A = 5  # Pin 8
PIN_B = 6  # Pin 10
BUTTON = 13  # Pin 7
# This is the event callback routine to handle events
def switch_event(event):
    global jacket_dc
    global theq
    if event == RotaryEncoder.CLOCKWISE:
        #print "Clockwise"
        if jacket_dc <= 99:
            jacket_dc = jacket_dc + 2
            theq.put(jacket_dc)
    elif event == RotaryEncoder.ANTICLOCKWISE:
        #print "Anticlockwise"
        if jacket_dc >= 1:
            jacket_dc = jacket_dc - 2
            theq.put(jacket_dc)
    elif event == RotaryEncoder.BUTTONDOWN:
        print "Button down"
    elif event == RotaryEncoder.BUTTONUP:
        print "Button up"
    #print jacket_dc


    return

# Define the switch
rswitch = RotaryEncoder(PIN_A, PIN_B, BUTTON, switch_event)

theq.put(jacket_dc)
#theq.join()

while True:
    time.sleep(1)
