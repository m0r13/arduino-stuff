// Based on:
// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

#include <ledstrip.h>
#include <ledprogram.h>
#include <parameter.h>
#include <beatdetection.h>
#include <bpmdetection.h>

const int SAMPLE_RATE = 5000; // in Hz
const int SAMPLE_PERIOD = 1000000 / SAMPLE_RATE; // us
const int BEAT_SAMPLE_N = 200; // check for beats every 200 samples (25Hz)

const int PIN_CLIP_LED = 8;
const int PIN_BEAT_LED = 12;

// parameters of the led program
Parameter hueFadingPerSecond(0.00, 0.02, 1.0);
Parameter hueNextRadius(0.0, 0.3, 0.5);
Parameter valueFactor(0.75, 0.85, 1.0);
Parameter defaultValue(0.1, 0.5, 1.0);
Parameter minimumValue(0.1, 0.5, 1.0); // minimum value is actually meant as percentage of default value
Parameter saturation(0.0, 1.0, 1.0);
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
BPMDetection bpmDetection;
LEDProgram ledProgram1(9, 10, 11, programParameters);
LEDProgram ledProgram2(3, 5, 6, programParameters);

int clipCounter = 0;
int clipCounterMax = 1000;
// 262/899 seem to be min/max für TL082CP 5V
int clipMin = 265;
int clipMax = 895;

bool beatActive = false;
const int lastBeatsCount = 8;
unsigned long lastFourBeats[lastBeatsCount] = {micros()};
int lastFourBeatsIndex = 0;

void setup() {
    Serial.begin(9600);
    randomSeed(analogRead(0));

    // set ADC to 77khz, max for 10bit
    ADCSRA = (ADCSRA & ~0b111) | 0b100;

    pinMode(PIN_CLIP_LED, OUTPUT);
    pinMode(PIN_BEAT_LED, OUTPUT);

    parameters.add(hueFadingPerSecond);
    parameters.add(hueNextRadius);
    parameters.add(valueFactor);
    parameters.add(defaultValue);
    parameters.add(minimumValue);
    parameters.add(saturation);

#if 1
    parameters.setAllModes(Parameter::MODE_SERIAL);
#endif

#if 1
    parameters.setAllModes(Parameter::MODE_DEFAULT);
    //hueFadingPerSecond.setAnalogReadMode(1);
    //hueNextRadius.setAnalogReadMode(1);
    //valueFactor.setAnalogReadMode(1);
    defaultValue.setAnalogReadMode(1);
    //minimumValue.setAnalogReadMode(1);
    //saturation.setAnalogReadMode(1);
#endif

    // emulate a first beat to set a led strip random color
    beatOn();
    beatOff();
}

// called when a beat starts
void beatOn() {
    bpmDetection.beatOn();
    ledProgram1.beatOn();
    ledProgram2.beatOn();
}

// called when a beat ends
void beatOff() {
    bpmDetection.beatOff();
    ledProgram1.beatOff();
    ledProgram2.beatOff();
}

// called 25 times in a second, right after the beat detection
void beatFade() {
    // update lighting parameters
    parameters.update();
    ledProgram1.beatFade();
    ledProgram2.beatFade();
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
        int usample = analogRead(0);
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
        for(unsigned long up = time + SAMPLE_PERIOD; time > 20 && time < up; time = micros());
        waited += micros() - waitedStart;
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
