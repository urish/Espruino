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
 'name' : "Beaglebone Black",
 'default_console' : "EV_USBSERIAL",
 'variables' :  0, # 0 = resizable variables, rather than fixed
 'binary_name' : 'espruino_%v_beaglebone',
 'build' : {
   'optimizeflags' : '-O3',
   'libraries' : [
     'NET',
     'GRAPHICS',
     'FILESYSTEM',
     'CRYPTO','SHA256','SHA512',
     'TLS',
     'HASHLIB',
     'TELNET',
   ],
   'makefile' : [
     'LINUX=1',
     'DEFINES += -DBEAGLEBONE_BLACK -DSYSFS_GPIO_DIR="\"/sys/class/gpio\"',
   ]
 }
};
chip = {
  'part' : "BEAGLEBONEBLACK",
  'family' : "LINUX",
  'package' : "",
  'ram' : 0,
  'flash' : 256, # size of file used to fake flash memory (kb)
  'speed' : -1,
  'usart' : 0,
  'spi' : 0,
  'i2c' : 1,
  'adc' : 0,
  'dac' : 0,
};
devices = {
};

def get_pins():
  pins = pinutils.generate_pins(2,127)
  pinutils.findpin(pins, "PD19", True)["functions"]["I2C2_SCL"]=0;
  pinutils.findpin(pins, "PD20", True)["functions"]["I2C2_SDA"]=0;
  return pins
