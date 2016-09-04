#ifndef potivalue_h
#define potivalue_h

#include <Arduino.h>

class PotiValue {
public:
    PotiValue(float minValue, float defaultValue, float maxValue);

    float value() const;
    void updateValue();

    void setPin(int pin);
    void setFixed();

protected:
    bool fixed;
    int pin;
    float minValue, defaultValue, maxValue, currentValue;
};

#endif
