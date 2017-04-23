#!/bin/false
# This file is part of Espruino, a JavaScript interpreter for Microcontrollers
#
# Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#
# ----------------------------------------------------------------------------------------
# This file contains information for a specific board - the available pins, and where LEDs,
# Buttons, and other in-built peripherals are. It is used to build documentation as well
# as various source and header files for Espruino.
# ----------------------------------------------------------------------------------------

import pinutils;

info = {
 'name' : "NGBeacon",
 'link' :  [ "https://www.nordicsemi.com/Products/Bluetooth-Smart-Bluetooth-low-energy/nRF51822" ],
 'default_console' : "EV_BLUETOOTH",
 'default_console_tx' : "D9",
 'default_console_rx' : "D8",
 'default_console_baudrate' : "9600",
 'variables' : 2000,
 'bootloader' : 1,
 'binary_name' : 'espruino_%v_ngbeacon.bin',
 'build' : {
   'optimizeflags' : '-O3',
   'libraries' : [
     'BLUETOOTH',
     'NET'
   ],
   'makefile' : [
     'DFU_PRIVATE_KEY=targets/nrf5x_dfu/ngbeacon_dfu_key.pem',
     'DFU_SETTINGS=--application-version 0xff --hw-version 52 --sd-req 0x8C'
   ]
 }
};

chip = {
  'part' : "NRF52832",
  'family' : "NRF52",
  'package' : "QFN48",
  'ram' : 64,
  'flash' : 512,
  'speed' : 64,
  'usart' : 1,
  'spi' : 3,
  'i2c' : 2,
  'adc' : 1,
  'dac' : 0,
   # If using DFU bootloader, it sits at 0x3C000 - 0x40000 (0x40000 is end of flash)
   # Might want to change 256 -> 240 in the code below
  'saved_code' : {
    'address' : ((118 - 3) * 4096), # Bootloader takes pages 120-127, FS takes 117-119
    'page_size' : 4096,
    'pages' : 3,
    'flash_available' : 512 - ((31 + 8 + 1 + 3)*4) # Softdevice uses 31 pages of flash, bootloader 8, FS 1, code 3. Each page is 4 kb.
  }
};

devices = {
  'BTN1' : { 'pin' : 'D17', 'inverted' : True, 'pinstate' : 'IN_PULLUP'},
  'LED1':  { 'pin' : 'D26', 'inverted' : True },
  'LED2':  { 'pin' : 'D15' },
  'LED3':  { 'pin' : 'D14' },
  'RX_PIN_NUMBER' : { 'pin' : 'D8'},
  'TX_PIN_NUMBER' : { 'pin' : 'D9'},
  'CTS_PIN_NUMBER' : { 'pin' : 'D10'},
  'RTS_PIN_NUMBER' : { 'pin' : 'D11'},
};

# left-right, or top-bottom order THIS IS INCORRECT!!!!!
board = {
  'left' : [ 'VDD', 'VDD', 'RESET', 'VDD','5V','GND','GND','PD3','PD4','PD28','PD29','PD30','PD31'],
  'right' : [ 'PD27', 'PD26', 'PD2', 'GND', 'PD25','PD24','PD23', 'PD22','PD20','PD19','PD18','PD17','PD16','PD15','PD14','PD13','PD12','PD11','PD10','PD9','PD8','PD7','PD6','PD5','PD21','PD1','PD0'],
};
board["_css"] = """
""";

def get_pins():
  pins = pinutils.generate_pins(0,31) # 32 General Purpose I/O Pins.
  return pins
