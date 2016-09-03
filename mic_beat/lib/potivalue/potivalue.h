#ifndef potivalue_h
#define potivalue_h

#include <Arduino.h>

template <typename T>
class PotiValue {
public:
    PotiValue(T minValue, T defaultValue, T maxValue);

    T value() const;
    void updateValue();

    void setPin(int pin);
    void setFixed();

protected:
    bool fixed;
    int pin;
    T minValue, defaultValue, maxValue, currentValue;
};

template <typename T>
PotiValue<T>::PotiValue(T minValue, T defaultValue, T maxValue)
    : fixed(true),
      minValue(minValue),
      defaultValue(defaultValue),
      maxValue(maxValue) {
}

template <typename T>
T PotiValue<T>::value() const {
    return fixed ? defaultValue : currentValue;
}

template <typename T>
void PotiValue<T>::updateValue() {
    if (fixed)
        return;
    float analogValue = float(analogRead(pin)) / 1024.0;
    currentValue = (1-analogValue) * minValue + analogValue * maxValue;
}

template <typename T>
void PotiValue<T>::setPin(int pin) {
    this->fixed = false;
    this->pin = pin;
    updateValue();
}

template <typename T>
void PotiValue<T>::setFixed() {
    this->fixed = true;   
}

#endif
