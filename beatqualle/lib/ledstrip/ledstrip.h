#ifndef ledstrip_h
#define ledstrip_h

#include <Arduino.h>

// converts any floating point hue to a hue in [0; 1]
float normalizeHue(float hue);

// returns a random hue in [hue-radius; hue+radius]
float randomHueNear(float hue, float radius);

// converts a color in HSV (h, s, v \in [0;1]) to RGB (r 3rd byte, g 2nd byte, b 1st byte)
long hsvToRGB(float h, float s, float v);

class LEDStrip {
public:
    LEDStrip(int redPin, int greenPin, int bluePin);

    void resetPinMapping();
    void setPinMapping(int redMapping, int greenMapping, int blueMapping);

    virtual void setRGB(int r, int g, int b);
    void setHSV(float h, float s, float v);

protected:
    int redPin, greenPin, bluePin;
    int redMapping, greenMapping, blueMapping;
    int red, green, blue;
};

class MultipleLEDStrips : public LEDStrip {
public:
    MultipleLEDStrips(LEDStrip& leds1, LEDStrip& leds2);

    virtual void setRGB(int r, int g, int b);

//protected:
    LEDStrip& leds1, leds2;
};

#endif

