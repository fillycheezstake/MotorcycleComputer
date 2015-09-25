#!/usr/bin/env python

#
# HD44780 LCD Test Script for
# Raspberry Pi
#
# Author : Matt Hawkins
# Site   : http://www.raspberrypi-spy.co.uk
# Edited for MCP23008 expander usage by Philip Ahlers, http://www.philipahlers.com
# Date   : 26/07/2012
#

# The wiring for the LCD is as follows:
# 1 : GND
# 2 : 5V
# 3 : Contrast (0-5V)*
# 4 : RS (Register Select)
# 5 : R/W (Read Write)       - GROUND THIS PIN
# 6 : Enable or Strobe
# 7 : Data Bit 0             - NOT USED
# 8 : Data Bit 1             - NOT USED
# 9 : Data Bit 2             - NOT USED
# 10: Data Bit 3             - NOT USED
# 11: Data Bit 4
# 12: Data Bit 5
# 13: Data Bit 6
# 14: Data Bit 7
# 15: LCD Backlight +5V**
# 16: LCD Backlight GND

# Commands
LCD_CLEARDISPLAY        = 0x01
LCD_RETURNHOME          = 0x02
LCD_ENTRYMODESET        = 0x04
LCD_DISPLAYCONTROL      = 0x08
LCD_CURSORSHIFT         = 0x10
LCD_FUNCTIONSET         = 0x20
LCD_SETCGRAMADDR        = 0x40
LCD_SETDDRAMADDR        = 0x80

# Entry flags
LCD_ENTRYRIGHT          = 0x00
LCD_ENTRYLEFT           = 0x02
LCD_ENTRYSHIFTINCREMENT = 0x01
LCD_ENTRYSHIFTDECREMENT = 0x00

# Control flags
LCD_DISPLAYON           = 0x04
LCD_DISPLAYOFF          = 0x00
LCD_CURSORON            = 0x02
LCD_CURSOROFF           = 0x00
LCD_BLINKON             = 0x01
LCD_BLINKOFF            = 0x00

# Move flags
LCD_DISPLAYMOVE         = 0x08
LCD_CURSORMOVE          = 0x00
LCD_MOVERIGHT           = 0x04
LCD_MOVELEFT            = 0x00

# Offset for up to 4 rows.
LCD_ROW_OFFSETS         = (0x00, 0x40, 0x14, 0x54)

#import
import MCP23008
import time

# Define MCP to LCD mapping
LCD_RS = 1
LCD_E  = 2
LCD_D4 = 3
LCD_D5 = 4
LCD_D6 = 5
LCD_D7 = 6

# Define some device constants
LCD_WIDTH = 16    # Maximum characters per line
LCD_CHR = True
LCD_CMD = False

LCD_LINE_1 = 0x80 # LCD RAM address for the 1st line
LCD_LINE_2 = 0xC0 # LCD RAM address for the 2nd line

# Timing constants
E_PULSE = 0.00000001
E_DELAY = 0.00000001

MCP = MCP23008.MCP23008()

class lcddriver():
    


    def __init__(self):
        # Initialise display

        self.lcd_byte(0x33,LCD_CMD)
        self.lcd_byte(0x32,LCD_CMD)
        self.lcd_byte(0x28,LCD_CMD)
        self.lcd_byte(0x0C,LCD_CMD)
        self.lcd_byte(0x06,LCD_CMD)
        self.lcd_byte(0x01,LCD_CMD)

    def message_old(self,printme):
    # Send string to display

        for char in printme:
            self.lcd_byte(ord(char),LCD_CHR)


    def message(self, text):
        """Write text to display.  Note that text can include newlines."""
        line = 0
        # Iterate through each character.
        for char in text:
            # Advance to next line if character is a new line.
            if char == '\n':
                line += 1
                # Move to left or right side depending on text direction.
                col = 0 if LCD_ENTRYLEFT & LCD_ENTRYLEFT > 0 else 2-1
                self.set_cursor(col, line)
            # Write the character to the display.
            else:
                self.lcd_byte(ord(char), True)

    def set_cursor(self, col, row):
        """Move the cursor to an explicit column and row position."""
        # Clamp row to the last row of the display.
        if row > 2:
            row = 2
        # Set location.
        self.lcd_byte(LCD_SETDDRAMADDR | (col + LCD_ROW_OFFSETS[row]),LCD_CMD)

    def home(self):
        self.lcd_byte(LCD_RETURNHOME,LCD_CMD)
    
    def clear(self):
        self.lcd_byte(LCD_CLEARDISPLAY,LCD_CMD)
    
    def _pulse_enable(self):
        # Pulse the clock enable line off, on, off to send command.
        MCP.output(LCD_E, False)
        self._delay_microseconds(1)       # 1 microsecond pause - enable pulse must be > 450ns
        MCP.output(LCD_E, True)
        self._delay_microseconds(1)       # 1 microsecond pause - enable pulse must be > 450ns
        MCP.output(LCD_E, False)
        self._delay_microseconds(1)       # commands need > 37us to settle

    def _delay_microseconds(self, microseconds):
        # Busy wait in loop because delays are generally very short (few microseconds).
        end = time.time() + (microseconds/1000000.0)
        while time.time() < end:
            pass


    def lcd_byte(self, value, char_mode=False):
        """Write 8-bit value in character or data mode.  Value should be an int
        value from 0-255, and char_mode is True if character data or False if
        non-character data (default).
        """
        # One millisecond delay to prevent writing too quickly.
        self._delay_microseconds(1000)
        # Set character / data bit.
        MCP.output(LCD_RS, char_mode)
        # Write upper 4 bits.
        MCP.output_pins({ LCD_D4: ((value >> 4) & 1) > 0,
                                 LCD_D5: ((value >> 5) & 1) > 0,
                                 LCD_D6: ((value >> 6) & 1) > 0,
                                 LCD_D7: ((value >> 7) & 1) > 0 })
        self._pulse_enable()
        # Write lower 4 bits.
        MCP.output_pins({ LCD_D4: (value        & 1) > 0,
                                 LCD_D5: ((value >> 1) & 1) > 0,
                                 LCD_D6: ((value >> 2) & 1) > 0,
                                 LCD_D7: ((value >> 3) & 1) > 0 })
        self._pulse_enable()



    def lcd_byte_old(self,bits, mode):
        # Send byte to data pins
        # bits = data
        # mode = True  for character
        #        False for command
    
        MCP.output(LCD_RS, mode) # RS
    
        # High bits
        MCP.output(LCD_D4, False)
        MCP.output(LCD_D5, False)
        MCP.output(LCD_D6, False)
        MCP.output(LCD_D7, False)
        if bits&0x10==0x10:
            MCP.output(LCD_D4, True)
        if bits&0x20==0x20:
            MCP.output(LCD_D5, True)
        if bits&0x40==0x40:
            MCP.output(LCD_D6, True)
        if bits&0x80==0x80:
            MCP.output(LCD_D7, True)
    
        # Toggle 'Enable' pin
        time.sleep(E_DELAY)
        MCP.output(LCD_E, True)
        time.sleep(E_PULSE)
        MCP.output(LCD_E, False)
        time.sleep(E_DELAY)
    
        # Low bits
        MCP.output(LCD_D4, False)
        MCP.output(LCD_D5, False)
        MCP.output(LCD_D6, False)
        MCP.output(LCD_D7, False)
        if bits&0x01==0x01:
            MCP.output(LCD_D4, True)
        if bits&0x02==0x02:
            MCP.output(LCD_D5, True)
        if bits&0x04==0x04:
            MCP.output(LCD_D6, True)
        if bits&0x08==0x08:
            MCP.output(LCD_D7, True)
    
        # Toggle 'Enable' pin
        time.sleep(E_DELAY)
        MCP.output(LCD_E, True)
        time.sleep(E_PULSE)
        MCP.output(LCD_E, False)
        time.sleep(E_DELAY)


