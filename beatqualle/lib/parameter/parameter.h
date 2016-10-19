#ifndef parameter_h
#define parameter_h

#include <Arduino.h>

class Parameter {
public:
    Parameter(float minValue, float defaultValue, float maxValue);

    int getMode() const;
    void setMode(int mode);

    int getAnalogReadPin() const;
    void setAnalogReadMode(int pin);

    int getDigitalReadPin() const;
    void setDigitalReadMode(int pin);

    void setOverride(float value);
    void clearOverride();

    float getValue() const;
    float getBoolValue() const;
    float getRelativeValue() const;
    void setRelativeValue(float alpha);

    static const int MODE_DEFAULT = 0;
    static const int MODE_ANALOG_READ = 1;
    static const int MODE_DIGITAL_READ = 2;
    static const int MODE_SERIAL = 3;

protected:
    int mode;
    int analogReadPin, digitalReadPin;
    float minValue, defaultValue, maxValue, currentValue, overrideValue;
    bool hasOverride;
};

class ParameterManager {
public:
    ParameterManager();

    void setAllModes(int mode);
    void add(Parameter& value);

    void update();

protected:
    Parameter* parameters[42];
    size_t parameterCount;
};

#endif
