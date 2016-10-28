#include "configuration.h"

#ifdef CONFIGURATION_QUALLE

#include <ir44key.h>
#include <ledstrip.h>
#include <ledprogram.h>

// parameters of the led program
Parameter hueFadingPerSecond(0.00, 0.02, 1.0); // Hue Fading (per second)
Parameter hueNextRadius(0.0, 0.2, 0.5); // Next Hue Radius
Parameter valueFactor(0.75, 0.85, 1.0); // Value Factor
Parameter defaultValue(0.1, 1.0, 1.0); // Default Value
Parameter minimumValue(0.0, 0.8, 1.0); // Minimum Value (as factor of default value)
Parameter saturation(0.0, 1.0, 1.0); // Saturation

Parameter stroboOverride(0.0, 0.0, 1.0); // Strobo Override ?type=button,shortcut=f

Parameter stroboEnabled(0.0, 0.0, 1.0); // External Strobo ?type=button,shortcut=b
Parameter stroboBPM(60.0, 300.0, 500.0); // External Strobo BPM
// TODO dummy beat generation somehow?

ParameterManager parameters;

LEDParameters programParameters = {
    &hueFadingPerSecond,
    &hueNextRadius,
    &valueFactor,
    &defaultValue,
    &minimumValue,
    &saturation
};

LEDStrip ledStrip1(3, 5, 6);
LEDStrip ledStrip2(9, 10, 11);
MultipleLEDStrips ledStripsBoth(&ledStrip1, &ledStrip2);

BeatLEDProgram ledProgram1(programParameters);
BeatLEDProgram ledProgram2(programParameters);
StroboLEDProgram stroboProgram(stroboBPM);
ManualLEDProgram manualProgram(defaultValue);
bool manualMode = false;

void handleSetup() {
    parameters.add(hueFadingPerSecond);
    parameters.add(hueNextRadius);
    parameters.add(valueFactor);
    parameters.add(defaultValue);
    parameters.add(minimumValue);
    parameters.add(saturation);
    parameters.add(stroboOverride);
    parameters.add(stroboEnabled);
    parameters.add(stroboBPM);

#if 1
    parameters.setAllModes(Parameter::MODE_SERIAL);
    //stroboOverride.setDigitalReadMode(PIN_BUTTON_1);
    //stroboEnabled.setDigitalReadMode(PIN_BUTTON_2);
#endif

#if 0
    parameters.setAllModes(Parameter::MODE_DEFAULT);
    hueFadingPerSecond.setAnalogReadMode(1);
#endif
}

void handleKeyPressed(unsigned long key, int pressCount) {
    if (key == IR44Key::BRIGHTNESS_UP) {
        defaultValue.setRelativeValue(min(1.0, defaultValue.getRelativeValue() + 0.05));
    } else if (key == IR44Key::BRIGHTNESS_DOWN) {
        defaultValue.setRelativeValue(max(0.0, defaultValue.getRelativeValue() - 0.05));
    } else if (key == IR44Key::MODE_AUTO && pressCount == 1) {
        manualMode = !manualMode;
        if (!manualMode) {
            ledStrip2.resetPinMapping();
            ledStrip2.setColorInverting(false);
        } else {
            //ledStrip2.setPinMapping(2, 1, 0);
        }
        Serial.print("Manual mode: ");
        Serial.println(manualMode);
    } else if (manualMode && key == IR44Key::DIY2 && pressCount == 2) {
        // TODO this is ugly
        ledStrip2.setColorInverting(!ledStrip2.hasColorInverting());
    } else {
        if (manualMode) {
            manualProgram.handleKeyPress(key, pressCount);
        } else if (key == IR44Key::DIY6) {
            stroboOverride.setOverride(1.0);
        } else if (key == IR44Key::MODE_FLASH && pressCount == 1) {
            stroboEnabled.setOverride(1.0);
        }
    }
}

void handleKeyReleased(unsigned long key, int pressCount) {
    if (manualMode) {
        manualProgram.handleKeyRelease(key);
    } else if (key == IR44Key::DIY6) {
        stroboOverride.clearOverride();
    } else if (key == IR44Key::MODE_FLASH) {
        stroboEnabled.clearOverride();
    }
}

void handleBeatOn() {
    if (manualMode) {
        manualProgram.beatOn(ledStripsBoth);
    } else {
        if (stroboEnabled.getValue() < 0.5) {
            ledProgram1.beatOn(ledStrip1);
            ledProgram2.beatOn(ledStrip2);
        }
    }
}

void handleBeatOff() {
    if (manualMode) {
        manualProgram.beatOff(ledStripsBoth);
    } else {
        if (stroboEnabled.getValue() < 0.5) {
            ledProgram1.beatOff(ledStrip1);
            ledProgram2.beatOff(ledStrip2);
        }
    }
}

void handleBeatFade() {
    // update lighting parameters
    parameters.update();

    if (manualMode) {
        manualProgram.beatFade(ledStripsBoth);
    } else {
        // override some parameters if we want to simulate strobo
        if (stroboOverride.getValue() > 0.5) {
            programParameters.stroboOverride();
        } else {
            programParameters.stopStroboOverride();
        }

        // show normal light program or replacement-strobo
        if (stroboEnabled.getValue() < 0.5) {
            ledProgram1.beatFade(ledStrip1);
            ledProgram2.beatFade(ledStrip2);
            //ledStrip2.setRGB(0, 0, 0);
        } else {
            ledStrip1.setRGB(0, 0, 0);
            stroboProgram.beatFade(ledStrip2);
        }
    }
}

#endif
