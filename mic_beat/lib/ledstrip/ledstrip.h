#ifndef ledstrip_h
#define ledstrip_h

#include <Arduino.h>

// converts any floating point hue to a hue in [0; 1]
float normalizeHue(float hue);

// returns a random hue in [hue-radius; hue+radius]
float randomHueNear(float hue, float radius);

class LEDStrip {
public:
    LEDStrip(int redPin, int greenPin, int bluePin);

    void setRGB(int r, int g, int b);
    void setHSV(float h, float s, float v);

protected:
    int redPin, greenPin, bluePin;
};

#endif

