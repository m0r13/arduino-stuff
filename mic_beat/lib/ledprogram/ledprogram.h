#ifndef ledprogram_h
#define ledprogram_h

#include <ledstrip.h>
#include <parameter.h>

struct LEDParameters {
    Parameter *hueFadingPerSecond, *hueNextRadius, *valueFactor, *defaultValue, *minimumValue, *saturation;
};

class LEDProgram {
public:
    LEDProgram(int redPin, int greenPin, int bluePin, const LEDParameters& parameters);

    void beatOn();
    void beatOff();
    void beatFade();

protected:
    LEDStrip leds;
    LEDParameters p;

    float currentHue, currentValue;
    int fadingHue;
};

#endif
