// Based on:
// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

#include <SoftwareSerial.h>

#include <IRremote.h>
#include <ledstrip.h>
#include <ledprogram.h>
#include <parameter.h>
#include <beatdetection.h>
#include <bpmdetection.h>
#include <ir44key.h>

const int SAMPLE_RATE = 5000; // in Hz
const int SAMPLE_PERIOD = 1000000 / SAMPLE_RATE; // us
const int BEAT_SAMPLE_N = 200; // check for beats every 200 samples (25Hz)

const int PIN_CLIP_LED = 4;
const int PIN_BEAT_LED = 2;
const int PIN_IR_RECEIVE = 7;
const int PIN_BUTTON_1 = -1;
const int PIN_BUTTON_2 = -1;

IRInput irInput(PIN_IR_RECEIVE);

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

BeatDetection beatDetection;
//BeatGenerator beatDetection(125.0);
bool beatActive = false;

BPMDetection bpmDetection;
LEDStrip ledStrip1(3, 5, 6);
LEDStrip ledStrip2(9, 10, 11);
MultipleLEDStrips ledStripsBoth(ledStrip1, ledStrip2);

BeatLEDProgram ledProgram1(programParameters);
BeatLEDProgram ledProgram2(programParameters);
StroboLEDProgram stroboProgram(stroboBPM);
ManualLEDProgram manualProgram(defaultValue);
bool manualMode = false;

int clipCounter = 0;
int clipCounterMax = 1000;
// 262/899 seem to be min/max für TL082CP 5V
int clipMin = 265 + 10;
int clipMax = 895 - 10;

//SoftwareSerial mySerial(8, 12);

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));

    // set ADC to 77khz, max for 10bit
    ADCSRA = (ADCSRA & ~0b111) | 0b100;

    irInput.enableIRIn();

    pinMode(PIN_CLIP_LED, OUTPUT);
    pinMode(PIN_BEAT_LED, OUTPUT);
    //pinMode(PIN_BUTTON_1, INPUT);
    //pinMode(PIN_BUTTON_2, INPUT);

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

    // emulate a first beat to set a led strip random color
    beatOn();
    beatOff();

    //mySerial.begin(9600);
}

// called when a beat starts
void beatOn() {
    bpmDetection.beatOn();

    if (manualMode) {
        manualProgram.beatOn(ledStripsBoth);
    } else {
        if (stroboEnabled.getValue() < 0.5) {
            ledProgram1.beatOn(ledStrip1);
            ledProgram2.beatOn(ledStrip2);
        }
    }
}

// called when a beat ends
void beatOff() {
    bpmDetection.beatOff();
    if (manualMode) {
        manualProgram.beatOff(ledStripsBoth);
    } else {
        if (stroboEnabled.getValue() < 0.5) {
            ledProgram1.beatOff(ledStrip1);
            ledProgram2.beatOff(ledStrip2);
        }
    }
}

// called 25 times in a second, right after the beat detection
void beatFade() {
    unsigned long key = 0;
    int pressCount = 0;
    if (irInput.processInput(key, pressCount)) {
        Serial.print("key: ");
        Serial.print(key, HEX);
        Serial.print(" pressCount: ");
        Serial.print(pressCount);
        Serial.print(" ");
        Serial.println(millis());
        if (key == IR44Key::BRIGHTNESS_UP) {
            defaultValue.setRelativeValue(min(1.0, defaultValue.getRelativeValue() + 0.05));
        } else if (key == IR44Key::BRIGHTNESS_DOWN) {
            defaultValue.setRelativeValue(max(0.0, defaultValue.getRelativeValue() - 0.05));
        } else if (key == IR44Key::MODE_AUTO && pressCount == 1) {
            manualMode = !manualMode;
            Serial.print("Manual mode: ");
            Serial.println(manualMode);
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

    if (irInput.hasReleasedKey()) {
        unsigned long releasedKey = irInput.getReleasedKey();
        /*
        Serial.print(releasedKey, HEX);
        Serial.print(" no longer repeating! ");
        Serial.println(millis());
        */
        if (manualMode) {
            manualProgram.handleKeyRelease(key);
        } else if (releasedKey == IR44Key::DIY6) {
            stroboOverride.clearOverride();
        } else if (releasedKey == IR44Key::MODE_FLASH) {
            stroboEnabled.clearOverride();
        }
        irInput.setReleasedKeyProcessed();
    }

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

void loop() {
    // actual beat detection algorithm by Damian Peckett
    unsigned long time = micros(); // Used to track rate
    float beatThreshold = 9.0;

    // number of beat iterations (25Hz) after which to check samplerate
    int beatIterations = 150;
    // how man normal sample iterations (5000Hz) that are
    int iterations = beatIterations * BEAT_SAMPLE_N;
    int j = 0;
    unsigned long start = micros();
    unsigned long waited = 0;
    for(int i = 0;; ++i) {
        // Read ADC and center so +-512
        int usample = analogRead(1);
        if (usample <= clipMin || usample >= clipMax) {
            clipCounter = clipCounterMax;
            digitalWrite(PIN_CLIP_LED, HIGH);
        }
        if (clipCounter > 0) {
            clipCounter--;
            if (clipCounter == 0)
                digitalWrite(PIN_CLIP_LED, LOW);
        }

        float sample = (float) usample - 503.f;
        int beatStatus = beatDetection.processSample(sample);
        if (beatStatus != BeatDetection::BEAT_KEEP) {
            if (beatStatus == BeatDetection::BEAT_ON) {
                if (!beatActive) {
                    digitalWrite(PIN_BEAT_LED, HIGH);
                    beatActive = true;
                    beatOn();
                }
            } else {
                if (beatActive) {
                    digitalWrite(PIN_BEAT_LED, LOW);
                    beatActive = false;
                    beatOff();
                }
            }
        }

        j++;
        if (j == 200) {
            beatFade();
            j = 0;
        }

        // Consume excess clock cycles, to keep at 5000 hz
        unsigned long waitedStart = micros();
        // uncommented due to ir taking up enough cycles right now
        //for(unsigned long up = time + SAMPLE_PERIOD; time > 20 && time < up; time = micros());
        waited += micros() - waitedStart;

        if (i > iterations)
            break;
    }

    unsigned long end = micros();
    float secondsSpent = float(end - start) / 1000000.0;
    float samplerate = iterations / secondsSpent;
    float secondsWaited = waited / 1000000.0;
    Serial.print("Samplerate: ");
    Serial.print(samplerate);
    Serial.print("; ");
    Serial.print(100 * secondsWaited / secondsSpent);
    Serial.println("% in idle");
}
