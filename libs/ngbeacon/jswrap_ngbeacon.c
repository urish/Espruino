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
#include "jstimer.h"
#include "jsvar.h"

#include "nrf_soc.h"
#include "nrf_drv_spi.h"

#define LATCH1_PIN            3
#define LATCH2_PIN            4

#define SPI0_CONFIG_SCK_PIN   28
#define SPI0_CONFIG_MOSI_PIN  27
#define LED_COUNT             10
#define FRAME_SIZE            (LED_COUNT * 3)

static const nrf_drv_spi_t m_spi_master_0 = NRF_DRV_SPI_INSTANCE(0);

static uint8_t rgbData[LED_COUNT * 3] = {0};
static bool initialized = false;

static JsVar *frameData = NULL;
static uint8_t *frameDataPtr = NULL;

uint16_t frameCounter = 0;

static void drawFrame(JsSysTime time, void* userdata) {
  uint32_t len = jsvGetArrayBufferLength(frameData);
  frameCounter = (frameCounter + 1) % (len / FRAME_SIZE);
  uint8_t buf[LED_COUNT * 4 + 12] = {0};
  uint8_t *frame = &frameDataPtr[frameCounter * FRAME_SIZE];

  for (uint8_t i = 0; i < LED_COUNT; i++) {
    buf[4 + i * 4] = 0xff;
    buf[4 + i * 4 + 1] = frame[i*3+2];
    buf[4 + i * 4 + 2] = frame[i*3+1];
    buf[4 + i * 4 + 3] = frame[i*3];
  }

  nrf_drv_spi_transfer(&m_spi_master_0, buf, sizeof(buf), NULL, 0);
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
    "name" : "dfu",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_spinner_dfu"
}*/
void jswrap_spinner_dfu() {
  sd_power_gpregret_set(0, 0x1);
  NVIC_SystemReset();
}

/*JSON{
    "type" : "staticmethod",
    "class" : "spinner",
    "name" : "schedule",
    "ifdef" : "NGBEACON",
    "generate" : "jswrap_schedule_frames",
    "params" : [
      ["frames", "JsVar", "frames"],
      ["interval", "float", "interval"]
    ],
    "return" : ["int", "result"]
}*/
uint32_t jswrap_schedule_frames(JsVar *frames, JsVarFloat interval) {
  // TODO ensure frames is actually an Array buffer 
  // TODO jsvGetArrayBufferLength(frames) % FRAME_SIZE == 0, length > 0 (RGB * 10 LEDS)
  uint32_t len = jsvGetArrayBufferLength(frames);
  if (len == 0) {
    return len;
  }
  
  if (frameData) {
    jsvUnLock(frameData);
    frameData = NULL;
    frameDataPtr = NULL;
  }
  
  frameData = jsvNewArrayBufferWithPtr(len, &frameDataPtr);
  if (!frameData) {
    return 0;
  }

  for (int i = 0; i < len; i++) {
    frameDataPtr[i] = jsvGetIntegerAndUnLock(jsvArrayBufferGet(frames, i));
  }

  JsSysTime frameTime = jshGetTimeFromMilliseconds(interval);
  jstStopExecuteFn(drawFrame, NULL);
  jstExecuteFn(drawFrame, NULL, jshGetSystemTime() + frameTime, frameTime);

  return len;
}
