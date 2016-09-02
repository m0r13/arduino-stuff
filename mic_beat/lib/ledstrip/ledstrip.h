#ifndef ledstrip_h
#define ledstrip_h

#include <Arduino.h>

class LEDStrip {
public:
    LEDStrip(int redPin, int greenPin, int bluePin);

    void setRGB(int r, int g, int b);
    void setHSV(float h, float s, float v);

protected:
    int redPin, greenPin, bluePin;
};

#endif

