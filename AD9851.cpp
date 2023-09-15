/*
  Author: Eyobed A.
  Institution: INSA, 2016 E.C
  License: GPL v3.0+
  Brief: Class implementations
*/

#include <Arduino.h>
#include "AD9851.h"

void AD9851::InitSerial(int W_CLK, int FQ_UD, int RESET, int DATA) {
  // Serial init
  serialMode = true;

  _W_CLK = W_CLK;
  _FQ_UD = FQ_UD;
  _RESET = RESET;
  _DATA = DATA;

  pinMode(_W_CLK, OUTPUT);
  pinMode(_FQ_UD, OUTPUT);
  pinMode(_DATA, OUTPUT);
  pinMode(_RESET, OUTPUT);

  // Pulldown initializations
  digitalWrite(_DATA, LOW);
  digitalWrite(_FQ_UD, LOW);
  digitalWrite(_W_CLK, LOW);

  Reset();

  /* If you are not using the parallel port, 
  tie pins D0 and D1 to 5V via a resistor and D2 to GND*/

  *_port = B11000011;
  pulseHigh(_W_CLK);
  pulseHigh(_FQ_UD);

  SetPhase(0);
  SetFrequency(0);
  Load();
}

void AD9851::InitParallel(int W_CLK, int FQ_UD, int RESET, volatile byte *port) {

  // Parallel init
  serialMode = false;


  _W_CLK = W_CLK;
  _FQ_UD = FQ_UD;
  _RESET = RESET;
  _port = port;


  pinMode(_W_CLK, OUTPUT);
  pinMode(_FQ_UD, OUTPUT);
  pinMode(_RESET, OUTPUT);

  *(_port - 1) = B11111111;
  *_port = B00000000;

  Reset();
}

void AD9851::SetFrequency(unsigned long frequency) {
  frequencyWord = (frequency * pow2_32 * 1UL) / DDS_CLOCK;
}

void AD9851::SetPhase(int phase) {
  // Normalize the phase angle between 0 and 180 degrees
  int desiredPhase = phase % 181;
  if (desiredPhase < 0)
    desiredPhase += 181;

  // Convert the desired phase to a 4-bit phase register value
  phaseWord = static_cast<uint8_t>((desiredPhase / 11.25));
}

void AD9851::Load() {
  if (serialMode) {
    AD9851::LoadSerial();
  } else {
    AD9851::LoadParallel();
  }
}

void AD9851::LoadSerial() {

  /*In serial load mode, subsequent rising edges of W_CLK shift
      the 1-bit data on Pin 25 (D7) through the 40 bits of programming information.*/

  for (int i = 0; i < 32; i++) {
    digitalWrite(_DATA, (frequencyWord & 0x01));
    pulseHigh(_W_CLK);
    frequencyWord >>= 1;
  }


  for (int i = 0; i < 3; i++) {
      if (i == 0) {
        if (pllState)
          digitalWrite(_DATA, HIGH);
        else
          digitalWrite(_DATA, LOW);
      } else if (i == 2) {
        if (powerOn)
          digitalWrite(_DATA, LOW);
        else
          digitalWrite(_DATA, HIGH);
      } else {
        digitalWrite(_DATA, LOW);
      }
      pulseHigh(_W_CLK);
    }

    for (int i = 0; i < 5; i++) {
      // Send phase
      digitalWrite(_DATA, (phaseWord & 0x01));
      pulseHigh(_W_CLK);
      phaseWord >>= 1;
    }

    pulseHigh(_FQ_UD);
  }

  void AD9851::LoadParallel() {
    // Pulse FQ_UD
    pulseHigh(_FQ_UD);

    int w = B11111000;  // Initialize with the default command byte (all bits set except the last two)

    // Set the PLL state bit
    if (pllState) {
      w |= 0x01;  // Set the last bit to 1
    } else {
      w &= 0xFE;  // Set the last bit to 0
    }

    // Set the power down bit
    if (powerOn) {
      w |= 0xF8;  // Set the third bit from the last to 1
    } else {
      w &= 0x04;  // Set the third bit from the last to 0
    }

    // Write W0
    int w0 = (phaseWord >> 5) | w;
    *_port = w0;
    pulseHigh(_W_CLK);

    // Write W1
    *_port = (byte)(frequencyWord >> 24);
    pulseHigh(_W_CLK);

    // Write W2
    *_port = (byte)(frequencyWord >> 16);
    pulseHigh(_W_CLK);

    // Write W3
    *_port = (byte)(frequencyWord >> 8);
    pulseHigh(_W_CLK);

    // Write W4
    *_port = (byte)frequencyWord;
    pulseHigh(_W_CLK);

    // Update frequency
    pulseHigh(_FQ_UD);
  }

  void AD9851::Reset() {
    pulseHigh(_RESET);
  }

  void AD9851::EnablePLL(bool pll) {
    if (pll) {
      pllState = true;
    } else {
      pllState = false;
    }
  }


  void AD9851::powerDown(bool power) {
    if (power) {
      powerOn = false;
    } else {
      powerOn = true;
    }
  }
