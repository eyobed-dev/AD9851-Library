# AD9851
This is a clone of repository at https://gitlab.com/dds6740637/AD9851/


## Intro

This is a library class implementing the functionalities of AD9851 180MHz DDS synthesizer. It supports configuring the chip in serial or parallel mode during runtime. The specialities of this chip is that it has 180MHz System clock derived from a PLL run by a 30MHz crystal. 

## Usage

Download the zip file from releases section and add it to the IDE

In your code, create an object representing the DDS.

```
AD9851 ad9851;
```
To use serial data loading use

```
ad9851.InitSerial(W_CLK,FQ_UD,RESET,DATA);
```
if your project does not use parallel loading you must tie D0 and D1 to 5V through a 10kOhm resistor.

To use parallel loading define a port(Ex. PORTD) and use 

```
ad9851.InitParallel(W_CLK,FQ_UD,RESET,&port)
```
The following member functions are available

     SetFrequency(unsigned long frequency);
     SetPhase(int phase);
     EnablePLL(bool pll);
     powerDown(bool power);
     Load();
     Reset();
