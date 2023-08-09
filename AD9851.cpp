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

  pulseHigh(_RESET);

  // If you are not using the parallel port, tie pins D0 and D1 to 5V via a resistor and D2 to GND
  _port = B11000011;
  pulseHigh(_W_CLK);
  pulseHigh(_FQ_UD);
  _port = B00000000;
  SetPhase(0);
  SetFrequency(0);
  Load();
}

void AD9851::InitParallel(int W_CLK, int FQ_UD, int RESET, volatile uint8_t *port) {
  // Parallel init
  serialMode = false;

  _W_CLK = W_CLK;
  _FQ_UD = FQ_UD;
  _RESET = RESET;
  _port = *port;

  pinMode(_W_CLK, OUTPUT);
  pinMode(_FQ_UD, OUTPUT);
  pinMode(_RESET, OUTPUT);


  // Determine the DDRx register based on the port
  volatile uint8_t *ddrRegister;
  if (port == &PORTB) {
    ddrRegister = &DDRB;
  } else if (port == &PORTC) {
    ddrRegister = &DDRC;
  } else if (port == &PORTD) {
    ddrRegister = &DDRD;
  } else {
    // Handle unsupported port
    // You can add appropriate error handling here
    return;
  }

  // Set the entire port as output using the determined DDRx register
  // *ddrRegister = 0xFF; // Set DDRx register value

  pulseHigh(_RESET);
}

void AD9851::SetFrequency(unsigned long frequency) {
  frequencyWord = (frequency * pow2_32 * 1UL) / DDS_CLOCK;
}

void AD9851::SetPhase(int phase) {
  // Normalize the phase angle between 0 and 359.999 degrees
  int desiredPhase = fmod(desiredPhase, 360);
  if (desiredPhase < 0)
    desiredPhase += 360;

  // Convert the desired phase to a 4-bit phase register value
  uint8_t phaseWord = static_cast<uint8_t>((desiredPhase / 360.0) * 16);
}

void AD9851::Load() {
  if (serialMode) {
    AD9851::LoadSerial();
  } else {
    AD9851::LoadParallel();
  }
}

void AD9851::LoadSerial() {
  for (int i = 0; i < 32; i++) {
    digitalWrite(_DATA, (frequencyWord & 0x01));
    pulseHigh(_W_CLK);
    frequencyWord >>= 1;
  }

  for (int i = 0; i < 4; i++) {
    if (i < 1)
      digitalWrite(_DATA, HIGH);
    else
      digitalWrite(_DATA, LOW);

    pulseHigh(_W_CLK);
  }

  for (int i = 0; i < 4; i++) {
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

  // Write W0
  _port = B00000001;
  pulseHigh(_W_CLK);

  // Write W1
  _port = (byte)(frequencyWord >> 24);
  pulseHigh(_W_CLK);

  // Write W2
  _port = (byte)(frequencyWord >> 16);
  pulseHigh(_W_CLK);

  // Write W3
  _port = (byte)(frequencyWord >> 8);
  pulseHigh(_W_CLK);

  // Write W4
  _port = (byte)frequencyWord;
  pulseHigh(_W_CLK);

  // Update frequency
  pulseHigh(_FQ_UD);
}

void AD9851::Reset() {
  pulseHigh(_RESET);
}
