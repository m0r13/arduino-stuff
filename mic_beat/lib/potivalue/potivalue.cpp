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

void PotiValue::setFixed(bool fixed) {
    this->fixed = fixed;   
}

void PotiValue::setDefault(float defaultValue) {
    this->defaultValue = defaultValue;
}

PotiValueManager::PotiValueManager()
    : mode(MODE_MANUALLY),
      value_count(0) {
}

PotiValueManager::~PotiValueManager() {
}

void PotiValueManager::setMode(int mode) {
    this->mode = mode;

    for (int i = 0; i < value_count; i++) {
        values[i]->setFixed(mode == MODE_SERIAL);
    }
}

void PotiValueManager::addValue(PotiValue& value) {
    values[value_count++] = &value;
}

void PotiValueManager::updateValues() {
    if (mode == MODE_MANUALLY) {
        for (int i = 0; i < value_count; i++) {
            values[i]->updateValue();
        }
    } else if (mode == MODE_SERIAL) {
        while (Serial.available() >= 2) {
            int id = Serial.read();
            int value = Serial.read();
            Serial.print("read ");
            Serial.print(id);
            Serial.print(" ");
            Serial.println(value);
            if (id >= value_count)
                continue;
            values[id]->setDefault((float) value / 255.0);
        }
    }
}
