#include "potivalue.h"

PotiValue::PotiValue(float minValue, float defaultValue, float maxValue)
    : fixed(true),
      minValue(minValue),
      defaultValue(defaultValue),
      maxValue(maxValue) {
}

float PotiValue::value() const {
    return fixed ? defaultValue : currentValue;
}

void PotiValue::updateValue() {
    if (fixed)
        return;
    float analogValue = float(analogRead(pin)) / 1024.0;
    currentValue = (1-analogValue) * minValue + analogValue * maxValue;
}

void PotiValue::setPin(int pin) {
    this->fixed = false;
    this->pin = pin;
    updateValue();
}

void PotiValue::setFixed() {
    this->fixed = true;   
}
