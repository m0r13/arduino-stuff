#ifndef potivalue_h
#define potivalue_h

#include <Arduino.h>

template <typename T>
class PotiValue {
public:
    PotiValue(int pin, T minValue, T maxValue);

    T value() const;
    void updateValue();

protected:
    int pin;
    T minValue, maxValue, currentValue;
};

template <typename T>
PotiValue<T>::PotiValue(int pin, T minValue, T maxValue)
    : pin(pin),
      minValue(minValue),
      maxValue(maxValue) {
    updateValue();
}

template <typename T>
T PotiValue<T>::value() const {
    return currentValue;
}

template <typename T>
void PotiValue<T>::updateValue() {
    float analogValue = float(analogRead(pin)) / 1024.0;
    currentValue = (1-analogValue) * minValue + analogValue * maxValue;
}

#endif
