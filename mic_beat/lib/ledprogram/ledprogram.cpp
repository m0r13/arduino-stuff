
#include "ledprogram.h"

LEDProgram::LEDProgram(int redPin, int greenPin, int bluePin, const LEDParameters& parameters)
    : leds(redPin, greenPin, bluePin), p(parameters) {
}

void LEDProgram::beatOn() {
    currentHue = randomHueNear(currentHue, p.hueNextRadius->getValue());
    currentValue = p.defaultValue->getValue();
    leds.setHSV(currentHue, p.saturation->getValue(), currentValue);
    fadingHue = 0;
}

void LEDProgram::beatOff() {
    fadingHue = random(0, 2) == 0 ? -1 : 1;
}

void LEDProgram::beatFade() {// do some fading if we are outside of a beat
    if (fadingHue != 0) {
        currentHue += fadingHue * p.hueFadingPerSecond->getValue() / 25.0;
        if (currentHue < 0 || currentHue > 1.0)
            currentHue = normalizeHue(currentHue);
        currentValue = max(p.minimumValue->getValue() * p.defaultValue->getValue(), currentValue * p.valueFactor->getValue());
        leds.setHSV(currentHue, p.saturation->getValue(), currentValue);
    }
}
