#ifndef AD9850_H
#define AD9850_H

#include <Arduino.h>

#define pulseHigh(pin) digitalWrite(pin, HIGH); delayMicroseconds(10); digitalWrite(pin, LOW)

class AD9850
{
  public:


    void initSerial(int , int , int , int );
    void initParallel(int , int , int , volatile uint8_t *);
    void setFrequency(unsigned long );
    void setPhase(int );
    void load();
    void reset();
    
    
  private:
      int _W_CLK;
      int _FQ_UD;
      int _RESET;
      int _DATA;
      volatile uint8_t _port=0x0;
      bool serialMode;
      unsigned long frequencyWord = 0x0;
      int phaseWord = 0;
      const long DDS_CLOCK = 180000000;
      void loadSerial();
      void loadParallel();
  };

#endif
