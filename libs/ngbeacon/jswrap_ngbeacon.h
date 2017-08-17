/*
 * This file is part of Espruino, a JavaScript interpreter for Microcontrollers
 *
 * Copyright (C) 2016 Gordon Williams <gw@pur3.co.uk>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * ----------------------------------------------------------------------------
 * Contains JavaScript interface for ng-beacon
 * ----------------------------------------------------------------------------
 */
#include "jspin.h"

int jswrap_ngbeacon_start();
int jswrap_ngbeacon_setPixel(int led, int rgb, bool write);
int jswrap_ngbeacon_write();
void jswrap_ngbeacon_clear(bool write);
uint32_t jswrap_spinner_spinCount();
JsVarFloat jswrap_spinner_rpm();
