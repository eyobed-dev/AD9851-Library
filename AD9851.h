#ifndef AD9851_H
#define AD9851_H

#include <Arduino.h>

#define pulseHigh(pin) digitalWrite(pin, HIGH); delayMicroseconds(10); digitalWrite(pin, LOW)
#define pow2_32 4294967296

class AD9851 {
public:
    void InitSerial(int W_CLK, int FQ_UD, int RESET, int DATA);
    void InitParallel(int W_CLK, int FQ_UD, int RESET, volatile uint8_t* port);
    void SetFrequency(unsigned long frequency);
    void SetPhase(int phase);
    void Load();
    void Reset();

private:
    int _W_CLK;
    int _FQ_UD;
    int _RESET;
    int _DATA;
    volatile uint8_t _port = 0x0;
    bool serialMode;
    unsigned long frequencyWord = 0x0;
    int phaseWord = 0;
    const long DDS_CLOCK = 180000000;
    void LoadSerial();
    void LoadParallel();
};

#endif
