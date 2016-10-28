#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// define by using compiler flag:
// $ ano build --cxxflags=-DCONFIGURATION_$CONFIG
//#define CONFIGURATION_QUALLE
//#define CONFIGURATION_KITCHEN

#include <parameter.h>
#include <ledprogram.h>

void handleSetup();

void handleKeyPressed(unsigned long key, int pressCount);
void handleKeyReleased(unsigned long key, int pressCount);

void handleBeatOn();
void handleBeatOff();
void handleBeatFade();

#endif
