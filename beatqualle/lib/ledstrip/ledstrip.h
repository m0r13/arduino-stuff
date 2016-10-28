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

    bool hasColorInverting() const;
    void setColorInverting(bool colorInverting);

    virtual void setRGB(int r, int g, int b);
    virtual void setHSV(float h, float s, float v);

    virtual void off();

protected:
    virtual void setInternalRGB(int r, int g, int b);

    int redPin, greenPin, bluePin;
    int redMapping, greenMapping, blueMapping;
    int red, green, blue;
    bool colorInverting;
};

class MultipleLEDStrips : public LEDStrip {
public:
    MultipleLEDStrips(LEDStrip* ledStrip1, LEDStrip* ledStrip2);

    virtual void setRGB(int r, int g, int b);

    virtual void off();

protected:
    LEDStrip *ledStrip1, *ledStrip2;
};

#endif

