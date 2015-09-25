__author__ = 'philipahlers'

#!/usr/bin/env python

#stab at creating a MCP23008 driver by Philip Ahlers, http://www.philipahlers.com

import smbus
import time

bus = smbus.SMBus(1) #the B+ uses 1, not 0
MCP23008addr = 0x20  #i2c address of the MCP23008

#Register addresses
MCP23008_IODIR = 0x00
MCP23008_IPOL = 0x01
MCP23008_GPINTEN = 0x02
MCP23008_DEFVAL = 0x03
MCP23008_INTCON = 0x04
MCP23008_IOCON = 0x05
MCP23008_GPPU = 0x06
MCP23008_INTF = 0x07
MCP23008_INTCAP = 0x08
MCP23008_GPIO = 0x09
MCP23008_OLAT = 0x0A

class MCP23008():

#reads a byte from the specified address on the MCP23008
    def read_byte(self,adr):
        return bus.read_byte_data(MCP23008addr, adr)

#reads a byte from the specified address on the MCP23008
    def write_byte(self,adr, value):
        bus.write_byte_data(MCP23008addr, adr, value)

    def __init__(self):
        self.write_byte(MCP23008_IODIR,0x00) #sets all GPIOs on the MCP23008 as outputs

    def output(self,pin,state):
        curr = self.read_byte(MCP23008_GPIO)
        if state==True:
            if pin==0:
                helper = 0x01
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==1:
                helper = 0x02
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==2:
                helper = 0x04
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==3:
                helper = 0x08
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==4:
                helper = 0x10
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==5:
                helper = 0x20
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==6:
                helper = 0x40
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==7:
                helper = 0x80
                tomcp = helper | curr
                self.write_byte(MCP23008_GPIO,tomcp)
        if state==False:
            if pin==0:
                helper = 0x01 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==1:
                helper = 0x02 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==2:
                helper = 0x04 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==3:
                helper = 0x08 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==4:
                helper = 0x10 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==5:
                helper = 0x20 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==6:
                helper = 0x40 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)
            if pin==7:
                helper = 0x80 ^ 0xFF
                tomcp = helper & curr
                self.write_byte(MCP23008_GPIO,tomcp)

    def output_pins(self, pins):
        for pin, value in pins.iteritems():
            self.output(pin, value)