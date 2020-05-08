/*
  AnalogFirmata.h - Firmata library
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2009-2015 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated December 23rd, 2016
*/

#include <ConfigurableFirmata.h>
#include "AnalogFirmata.h"
#include "AnalogOutputFirmata.h"

AnalogOutputFirmata::AnalogOutputFirmata()
{
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
}

void AnalogOutputFirmata::reset()
{

}

boolean AnalogOutputFirmata::handlePinMode(byte pin, int mode)
{
  if (mode == PIN_MODE_PWM && IS_PIN_PWM(pin)) {
    pinMode(PIN_TO_PWM(pin), OUTPUT);
    /* changed by ind4fibre team 11.27.2019 for ESP32 */
    #if defined(ARDUINO_ARCH_ESP32)
      ledcAttachPin(PIN_TO_PWM(pin), 0); //set channel for esp32 adc
      ledcSetup(0, 5000, 8); //set channel for esp32 adc, freq and pwm 8 bits
      ledcWrite(0, 0); //channel 0, value 0
    #else
      analogWrite(PIN_TO_PWM(pin), 0);
    #endif
    return true;
  }
  return false;
}

void AnalogOutputFirmata::handleCapability(byte pin)
{
  if (IS_PIN_PWM(pin)) {
    Firmata.write(PIN_MODE_PWM);
    Firmata.write(DEFAULT_PWM_RESOLUTION);
  }
}

boolean AnalogOutputFirmata::handleSysex(byte command, byte argc, byte* argv)
{
  if (command == EXTENDED_ANALOG) {
    if (argc > 1) {
      int val = argv[1];
      if (argc > 2) val |= (argv[2] << 7);
      if (argc > 3) val |= (argv[3] << 14);
      analogWriteCallback(argv[0], val);
      return true;
    }
    return false;
  } else {
    return handleAnalogFirmataSysex(command, argc, argv);
  }
}
