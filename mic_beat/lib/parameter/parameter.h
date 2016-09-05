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
    void readFromAnalogPin();

    float getValue() const;
    void setRelativeValue(float alpha);

    static const int MODE_DEFAULT = 0;
    static const int MODE_ANALOG_READ = 1;
    static const int MODE_SERIAL = 2;

protected:
    int mode;
    int analogReadPin;
    float minValue, defaultValue, maxValue, currentValue;
};

class ParameterManager {
public:
    ParameterManager();

    void setAllModes(int mode);
    void add(Parameter& value);

    void update();

protected:
    Parameter* parameters[10];
    size_t parameterCount;
};

#endif
