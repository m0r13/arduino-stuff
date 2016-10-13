#ifndef ledprogram_h
#define ledprogram_h

#include <ledstrip.h>
#include <parameter.h>

struct LEDParameters {
    Parameter *hueFadingPerSecond, *hueNextRadius, *valueFactor, *defaultValue, *minimumValue, *saturation;
};

class LEDProgram {
public:
    virtual void beatOn(LEDStrip& leds) = 0;
    virtual void beatOff(LEDStrip& leds) = 0;
    virtual void beatFade(LEDStrip& leds) = 0;
};

class BeatLEDProgram : public LEDProgram {
public:
    BeatLEDProgram(const LEDParameters& parameters);

    virtual void beatOn(LEDStrip& leds);
    virtual void beatOff(LEDStrip& leds);
    virtual void beatFade(LEDStrip& leds);

protected:
    LEDParameters p;

    float currentHue, currentValue;
    int fadingHue;
};

class StroboLEDProgram : public LEDProgram {
public:
    StroboLEDProgram(Parameter& bpm);

    void resetTimer();

    virtual void beatOn(LEDStrip& leds);
    virtual void beatOff(LEDStrip& leds);
    virtual void beatFade(LEDStrip& leds);

protected:
    Parameter& bpm;
    int maxTimer, timer;
    bool currentStatus;
};

class ManualLEDProgram : public LEDProgram {
public:
    ManualLEDProgram(Parameter& brightness);

    void handleKeyPress(unsigned long key, int count);
    void handleKeyRelease(unsigned long key);
    void update();

    virtual void beatOn(LEDStrip& leds);
    virtual void beatOff(LEDStrip& leds);
    virtual void beatFade(LEDStrip& leds);

protected:
    Parameter& brightness;

    int red, green, blue;
    bool updateRequired;
};

#endif
