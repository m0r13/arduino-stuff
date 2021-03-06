#include "ledprogram.h"

#include <ir44key.h>

void LEDParameters::stroboOverride() {
    valueFactor->setOverride(0.0);
    minimumValue->setOverride(0.0);
    saturation->setOverride(0.0);
}

void LEDParameters::stopStroboOverride() {
    valueFactor->clearOverride();
    minimumValue->clearOverride();
    saturation->clearOverride();
}

BeatLEDProgram::BeatLEDProgram(const LEDParameters& parameters)
    : p(parameters) {
}

void BeatLEDProgram::beatOn(LEDStrip& leds) {
    currentHue = randomHueNear(currentHue, p.hueNextRadius->getValue());
    currentValue = p.defaultValue->getValue();
    leds.setHSV(currentHue, p.saturation->getValue(), currentValue);
    fadingHue = 0;
}

void BeatLEDProgram::beatOff(LEDStrip& leds) {
    fadingHue = random(0, 2) == 0 ? -1 : 1;
}

void BeatLEDProgram::beatFade(LEDStrip& leds) {// do some fading if we are outside of a beat
    if (fadingHue != 0) {
        currentHue += fadingHue * p.hueFadingPerSecond->getValue() / 25.0;
        if (currentHue < 0 || currentHue > 1.0)
            currentHue = normalizeHue(currentHue);
        currentValue = max(p.minimumValue->getValue() * p.defaultValue->getValue(), currentValue * p.valueFactor->getValue());
        leds.setHSV(currentHue, p.saturation->getValue(), currentValue);
    }
}

StroboLEDProgram::StroboLEDProgram(Parameter& bpm)
    : bpm(bpm), maxTimer(0), timer(0), currentStatus(false) {
}

void StroboLEDProgram::resetTimer() {
    timer = 0;
    currentStatus = true;
}

void StroboLEDProgram::beatOn(LEDStrip& leds) {

}

void StroboLEDProgram::beatOff(LEDStrip& leds) {

}

void StroboLEDProgram::beatFade(LEDStrip& leds) {
    timer++;
    maxTimer = 1000000 * 60 / bpm.getValue() / (1000000 / 25) * 0.5;
    if (timer >= maxTimer) {
        timer = 0;
        currentStatus = !currentStatus;
        int value = currentStatus ? 255 : 0;
        leds.setRGB(value, value, value);
    }
}


ManualLEDProgram::ManualLEDProgram(Parameter& brightness)
    : brightness(brightness), red(255), green(0), blue(0), updateRequired(true) {
}

void ManualLEDProgram::handleKeyPress(unsigned long key, int count) {
    updateRequired = true;
    if (IR44Key::isColorKey(key)) {
        unsigned long color = IR44Key::getColorOfKey(key);
        red = color >> 16;
        green = (color >> 8) & 0xff;
        blue = color & 0xff;
    } else if (key == IR44Key::DIY1) {
       red = random(0, 256);
       green = random(0, 256);
       blue = random(0, 256);
    } else {
        updateRequired = false;
    }
}

void ManualLEDProgram::handleKeyRelease(unsigned long key) {
}

void ManualLEDProgram::update() {
    updateRequired = true;
}

void ManualLEDProgram::beatOn(LEDStrip& leds) {
}

void ManualLEDProgram::beatOff(LEDStrip& leds) {
}

void ManualLEDProgram::beatFade(LEDStrip& leds) {
    if (updateRequired) {
        float b = brightness.getValue();
        leds.setRGB(red * b, green * b, blue * b);
        updateRequired = false;
    }
}
