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
 * Contains JavaScript interface for the hexagonal Espruino badge
 * ----------------------------------------------------------------------------
 */
#include "jspin.h"

int jswrap_badge_capSense(int corner);
int jswrap_badge_getBatteryPercentage();

void jswrap_badge_init();
void jswrap_badge_kill();
bool jswrap_badge_idle();
