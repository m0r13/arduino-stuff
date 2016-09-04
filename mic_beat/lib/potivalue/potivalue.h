#ifndef potivalue_h
#define potivalue_h

#include <Arduino.h>

class PotiValue {
public:
    PotiValue(float minValue, float defaultValue, float maxValue);

    float value() const;
    void updateValue();

    void setPin(int pin);
    void setFixed(bool fixed);
    void setDefault(float defaultValue);

protected:
    bool fixed;
    int pin;
    float minValue, defaultValue, maxValue, currentValue;
};

class PotiValueManager {
public:
    PotiValueManager();
    ~PotiValueManager();

    void setMode(int mode);
    void addValue(PotiValue& value);
    void updateValues();

    static const int MODE_MANUALLY = 0;
    static const int MODE_SERIAL = 1;

protected:
    int mode;

    PotiValue* values[10];
    size_t value_count;
};

#endif
