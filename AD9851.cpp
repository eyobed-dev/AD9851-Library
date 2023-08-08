#include <Arduino.h>
#include "AD9851.h"




void AD9850::initSerial(int W_CLK, int FQ_UD, int RESET, int DATA) {
  //Serial init
  serialMode = true;

  _W_CLK = W_CLK;
  _FQ_UD = FQ_UD;
  _RESET = RESET;
  _DATA = DATA;

  pinMode(_W_CLK, OUTPUT);
  pinMode(_FQ_UD, OUTPUT);
  pinMode(_DATA, OUTPUT);
  pinMode(_RESET, OUTPUT);

  // pulldown initializations
  digitalWrite(_DATA, LOW);
  digitalWrite(_FQ_UD, LOW);
  digitalWrite(_W_CLK, LOW);

  pulseHigh(_RESET);

  // if you are not using the parallel port, tie pins D0 and D1 to 5V via a resistor and D2 to GND
  _port = B11000011;
  pulseHigh(_W_CLK);
  pulseHigh(_FQ_UD);
  _port = B00000000;
  setPhase(0);
  setFrequency(0);
  load();
}

void AD9850::initParallel(int W_CLK, int FQ_UD, int RESET, volatile uint8_t *port) {
  //Parallel init
  serialMode = false;



  _W_CLK = W_CLK;
  _FQ_UD = FQ_UD;
  _RESET = RESET;
  _port = *port;

  pinMode(_W_CLK, OUTPUT);
  pinMode(_FQ_UD, OUTPUT);
  pinMode(_RESET, OUTPUT);



  DDRD = B11111111;

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

void AD9850::setFrequency(unsigned long frequency) {

  frequencyWord = (frequency * pow(2, 32) * 1UL) / DDS_CLOCK;
}

void AD9850::setPhase(int phase) {

  // Normalize the phase angle between 0 and 359.999 degrees
  int  desiredPhase = fmod(desiredPhase, 360);
  if (desiredPhase < 0)
    desiredPhase += 360;

  // Convert the desired phase to a 4-bit phase register value
  uint8_t phaseWord = static_cast<uint8_t>((desiredPhase / 360.0) * 16);
}

void AD9850::load() {
  if (serialMode)
  {
    AD9850::loadSerial();
  }
  else
  {
    AD9850::loadParallel();
  }
}

void AD9850::loadSerial() {

  for (int i = 0; i < 32; i++)
  {
    digitalWrite(_DATA, (frequencyWord & 0x01));
    pulseHigh(_W_CLK);
    frequencyWord >>= 1;
  }

  for (int i = 0; i < 4; i++)
  {
    if (i < 1)
      digitalWrite(_DATA, HIGH);
    else
      digitalWrite(_DATA, LOW);

    pulseHigh(_W_CLK);
  }

  for (int i = 0; i < 4; i++)
  {
    // send phase
    digitalWrite(_DATA, (phaseWord & 0x01));
    pulseHigh(_W_CLK);
    phaseWord >>= 1;
  }

  pulseHigh(_FQ_UD);
}

void AD9850::loadParallel()
{
  // pulse fu_ud
  pulseHigh(_FQ_UD);

  // write w0
  _port = B00000001;
  pulseHigh(_W_CLK);

  // write w1
  _port = (byte)(frequencyWord >> 24);
  pulseHigh(_W_CLK);

  // write w2
  _port = (byte)(frequencyWord >> 16);
  pulseHigh(_W_CLK);

  // write w3
  _port = (byte)(frequencyWord >> 8);
  pulseHigh(_W_CLK);

  // write w4
  _port = (byte)frequencyWord;
  pulseHigh(_W_CLK);
  // update frequency
  pulseHigh(_FQ_UD);
}

void AD9850::reset() {
 pulseHigh(_RESET);
}
