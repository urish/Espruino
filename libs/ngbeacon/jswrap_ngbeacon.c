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

#include "nrf_soc.h"
#include "nrf_drv_spi.h"

#define LATCH1_PIN            3
#define LATCH2_PIN            4

#define SPI0_CONFIG_SCK_PIN   28
#define SPI0_CONFIG_MOSI_PIN  27
#define LED_COUNT             10

static uint32_t spinCounter = 0;
static JsSysTime previousTick = 0;
static JsSysTime lastTick = 0;

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

static uint8_t rgbData[LED_COUNT * 3] = {0};
static bool initialized = false;

static void onLatchChange(bool state, IOEventFlags flags) {
  if (state) {
    previousTick = lastTick;
    lastTick = jshGetSystemTime();
    spinCounter++;
  }
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "start",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_ngbeacon_start",
    "return" : ["int", "start result" ]
}*/
int jswrap_ngbeacon_start() {
  nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
  config.sck_pin   = SPI0_CONFIG_SCK_PIN;
  config.mosi_pin  = SPI0_CONFIG_MOSI_PIN;
  config.frequency = NRF_DRV_SPI_FREQ_1M;
  config.mode      = NRF_DRV_SPI_MODE_1;
  config.bit_order = NRF_DRV_SPI_BIT_ORDER_LSB_FIRST;
  uint32_t rc = nrf_drv_spi_init(&m_spi_master_0, &config, NULL);
  if (rc == NRF_SUCCESS) {
    initialized = true;
  }

  IOEventFlags exti = jshPinWatch(LATCH1_PIN, true);
  jshSetEventCallback(exti, onLatchChange);

  return rc;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "setPixel",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_ngbeacon_setPixel",
    "params" : [
      ["led","int","led index"],
      ["rgb","int","red (MSB) + green + blue (LSB)"],
      ["write","bool","write (default = false)"]
    ],
    "return" : ["int", "write result" ]
}*/
int jswrap_ngbeacon_setPixel(int led, int rgb, bool write) {
  if (led >= 0 && led < LED_COUNT) {
    rgbData[led*3] = (rgb >> 16) & 0xff;
    rgbData[led*3+1] = (rgb >> 8) & 0xff;
    rgbData[led*3+2] = rgb & 0xff;
    if (write) {
      return jswrap_ngbeacon_write();
    }
  }
  return 0;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "write",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_ngbeacon_write",
    "return" : ["int", "write result" ]
}*/
int jswrap_ngbeacon_write() {
  uint8_t buf[LED_COUNT * 4 + 12] = {0};

  if (!initialized) {
    uint32_t rc = jswrap_ngbeacon_start();
    if (rc != NRF_SUCCESS) {
      return rc;
    }
  }

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    buf[4 + i * 4] = 0xff;
    buf[4 + i * 4 + 1] = rgbData[i * 3 + 2];
    buf[4 + i * 4 + 2] = rgbData[i * 3 + 1];
    buf[4 + i * 4 + 3] = rgbData[i * 3];
  }

  return nrf_drv_spi_transfer(&m_spi_master_0, buf, sizeof(buf), NULL, 0);
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "clear",
    "ifdef" : "NGBEACON",
    "params" : [
      ["write","bool","write (default = false)"]
    ],
    "generate" : "jswrap_ngbeacon_clear"
}
*/
void jswrap_ngbeacon_clear(bool write) {
  memset(rgbData, 0, sizeof(rgbData));
  if (write) {
    jswrap_ngbeacon_write();
  }
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "spinCount",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_spinner_spinCount",
    "return" : ["int", "number of spins" ]
}*/
uint32_t jswrap_spinner_spinCount() {
  return spinCounter;
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "rpm",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_spinner_rpm",
    "return" : ["float", "spinner speed in RPM" ]
}*/
JsVarFloat jswrap_spinner_rpm() {
  return 60000.0 / jshGetMillisecondsFromTime(lastTick - previousTick);
}
