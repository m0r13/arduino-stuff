#ifndef potivalue_h
#define potivalue_h

#include <Arduino.h>

class PotiValue {
public:
    PotiValue(float minValue, float defaultValue, float maxValue);

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

class PotiValueManager {
public:
    PotiValueManager();

    void setAllModes(int mode);
    void addValue(PotiValue& value);

    void update();

protected:
    PotiValue* values[10];
    size_t valueCount;
};

#endif
