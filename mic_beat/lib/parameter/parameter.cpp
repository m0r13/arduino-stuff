#include "parameter.h"

Parameter::Parameter(float minValue, float defaultValue, float maxValue)
    : mode(MODE_DEFAULT),
      minValue(minValue),
      defaultValue(defaultValue),
      maxValue(maxValue),
      currentValue(defaultValue) {
}

int Parameter::getMode() const {
    return mode;
}

void Parameter::setMode(int mode) {
    this->mode = mode;
}

int Parameter::getAnalogReadPin() const {
    return analogReadPin;
}

void Parameter::setAnalogReadMode(int pin) {
    setMode(MODE_ANALOG_READ);
    analogReadPin = pin;
}

void Parameter::setOverride(float value) {
    overrideValue = value;
    hasOverride = true;
}

void Parameter::clearOverride() {
    hasOverride = false;
}

float Parameter::getValue() const {
    if (hasOverride)
        return overrideValue;
    return mode == MODE_DEFAULT ? defaultValue : currentValue;
}

void Parameter::setRelativeValue(float alpha) {
    currentValue = (1-alpha) * minValue + alpha * maxValue;
}

ParameterManager::ParameterManager()
    : parameterCount(0) {
}

void ParameterManager::setAllModes(int mode) {
    for (int i = 0; i < parameterCount; i++) {
        parameters[i]->setMode(mode);
    }
}

void ParameterManager::add(Parameter& value) {
    parameters[parameterCount++] = &value;
}

void ParameterManager::update() {
    for (int i = 0; i < parameterCount; i++) {
        Parameter* parameter = parameters[i];
        if (parameter->getMode() == Parameter::MODE_ANALOG_READ) {
            parameter->setRelativeValue(float(analogRead(parameter->getAnalogReadPin()) / 1024.0));
        }
    }

    if (Serial.available() >= 2) {
        int parameterIndex = Serial.read();
        float alpha = float(Serial.read()) / 255.0;
        if (parameterIndex < parameterCount && parameters[parameterIndex]->getMode() == Parameter::MODE_SERIAL) {
            parameters[parameterIndex]->setRelativeValue(alpha);
        }
    }
}
