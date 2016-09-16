
#include "ledprogram.h"

LEDProgram::LEDProgram(int redPin, int greenPin, int bluePin, const LEDParameters& parameters)
    : leds(redPin, greenPin, bluePin), p(parameters) {
}

namespace {

// converts any floating point hue to a hue in [0; 1]
float hueMod(float hue) {
    hue = fmod(hue, 1.0);
    if (hue < 0)
        return 1 + hue;
    return hue;
}

// returns a random hue in [hue-radius; hue+radius]
float randomHueNear(float hue, float radius) {
    float minimumPossible = hue - radius;
    float maximumPossible = hue + radius;
    float alpha = float(random(1025)) / 1024.0;
    return hueMod((1-alpha) * minimumPossible + alpha * maximumPossible);
}

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
            currentHue = hueMod(currentHue);
        currentValue = max(p.minimumValue->getValue() * p.defaultValue->getValue(), currentValue * p.valueFactor->getValue());
        leds.setHSV(currentHue, p.saturation->getValue(), currentValue);
    }
}
