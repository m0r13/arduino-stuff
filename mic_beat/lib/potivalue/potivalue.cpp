#include "potivalue.h"

PotiValue::PotiValue(float minValue, float defaultValue, float maxValue)
    : mode(MODE_DEFAULT),
      minValue(minValue),
      defaultValue(defaultValue),
      maxValue(maxValue),
      currentValue(defaultValue) {
}

int PotiValue::getMode() const {
    return mode;
}

void PotiValue::setMode(int mode) {
    this->mode = mode;
}

int PotiValue::getAnalogReadPin() const {
    return analogReadPin;
}

void PotiValue::setAnalogReadMode(int pin) {
    setMode(MODE_ANALOG_READ);
    analogReadPin = pin;
}

float PotiValue::getValue() const {
    return mode == MODE_DEFAULT ? defaultValue : currentValue;
}

void PotiValue::setRelativeValue(float alpha) {
    currentValue = (1-alpha) * minValue + alpha * maxValue;
}

PotiValueManager::PotiValueManager()
    : valueCount(0) {
}

void PotiValueManager::setAllModes(int mode) {
    for (int i = 0; i < valueCount; i++) {
        values[i]->setMode(mode);
    }
}

void PotiValueManager::addValue(PotiValue& value) {
    values[valueCount++] = &value;
}

void PotiValueManager::update() {
    for (int i = 0; i < valueCount; i++) {
        PotiValue* value = values[i];
        if (value->getMode() == PotiValue::MODE_ANALOG_READ) {
            value->setRelativeValue(float(analogRead(value->getAnalogReadPin()) / 1024.0));
        }
    }

    if (Serial.available() >= 2) {
        int valueIndex = Serial.read();
        float alpha = float(Serial.read()) / 255.0;
        if (valueIndex < valueCount && values[valueIndex]->getMode() == PotiValue::MODE_SERIAL) {
            values[valueIndex]->setRelativeValue(alpha);
        }
    }
}
