/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2013 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * This file is designed to be parsed during the build process
 *
 * Contains JavaScript interface for ng-beacon
 * ----------------------------------------------------------------------------
 */


#include "jswrap_ngbeacon.h"
#include "jshardware.h"

#define SHT2x_ADDRESS 0x40
#define SHT2X_POLYNOMIAL 0x131

static uint8_t calculateCrc(uint8_t bytes[], uint8_t len) {
  uint8_t i, bit, crc = 0;
  for (i = 0; i < len; i++) {
    crc ^= bytes[i];
    for (bit = 8; bit > 0; --bit) {
      crc = (crc & 0x80) ? ((crc << 1) ^ SHT2X_POLYNOMIAL) : (crc << 1);
    }
  }
  return crc;
}

static float readShtSensor(uint8_t cmd) {
  uint8_t result[3] = {0};
  jshI2CWrite(EV_I2C1, SHT2x_ADDRESS, 1, &cmd, true);
  jshI2CRead(EV_I2C1, SHT2x_ADDRESS, sizeof(result), result, true);
  uint8_t crc = calculateCrc(result, 2);
  if (result[2] != crc) {
    return NAN;
  }
  float value = (result[0] << 8) | (result[1] & ~0x03);
  return value;
}

/*JSON{
  "type" : "init",
  "generate" : "jswrap_ngbeacon_init"
}*/
void jswrap_ngbeacon_init() {
  // enable I2C (for sensors)
  JshI2CInfo inf;
  jshI2CInitInfo(&inf);
  inf.pinSDA = JSH_PORTD_OFFSET+28; // 'D28'
  inf.pinSCL = JSH_PORTD_OFFSET+29; // 'D29'
  jshI2CSetup(EV_I2C1, &inf);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "ngbeacon",
    "name" : "temperature",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_ngbeacon_temperature",
    "return" : ["float", "Temperature reading value, in celcius" ]
}
*/
JsVarFloat jswrap_ngbeacon_temperature() {
  float value = readShtSensor(0xe3);
  return -46.85 + 175.72 / 65536.0 * value;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "ngbeacon",
    "name" : "humidity",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_ngbeacon_humidity",
    "return" : ["float", "Humidity percentage" ]
}
*/
JsVarFloat jswrap_ngbeacon_humidity() {
  float value = readShtSensor(0xe5);
  return -6.0 + 125.0 / 65536.0 * value;
}
