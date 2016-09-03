// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

#include <ledstrip.h>

// Our Global Sample Rate, 5000hz
#define SAMPLEPERIODUS 200

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

const int clipLed = 8;
const int beatLed = 9;

/**
 So settings:
 - hue fading speed
 - next hue maximum distance
 - value fading speed
 - value minimum
 - overall value
 - dummy beats: bpm
*/
const float hueFadingPerSecond = 0.02;
const float hueFadingPerSample = hueFadingPerSecond / 25;
const float valueFactor = 0.8;
const float minimumValue = 0.5;
const float hueNextRadius = 0.2; // not used at the moment

LEDStrip leds(3, 5, 6);
float currentH = randomHueNear(0.5, 0.5), currentV;
int fadingH;

int clipCounter = 0;
int clipCounterMax = 1000;

bool beatActive = false;
const int lastBeatsCount = 8;
unsigned long lastFourBeats[lastBeatsCount] = {micros()};
int lastFourBeatsIndex = 0;

void setup() {
    Serial.begin(9600);
    
    // Set ADC to 77khz, max for 10bit
    sbi(ADCSRA,ADPS2);
    cbi(ADCSRA,ADPS1);
    cbi(ADCSRA,ADPS0);

    //The pin with the LED
    pinMode(clipLed, OUTPUT);
    pinMode(beatLed, OUTPUT);

    beatOn();
    beatOff();
}

float modHue(float hue) {
    hue = fmod(hue, 1.0);
    if (hue < 0)
        return 1 + hue;
    return hue;
}

float randomHueNear(float h, float radius) {
    float minimumPossible = h - radius;
    float maximumPossible = h + radius;
    float alpha = float(random(1025)) / 1024.0;
    return modHue((1-alpha) * minimumPossible + alpha * maximumPossible);
}

void beatOn() {
    unsigned long now = micros();
    unsigned long then = lastFourBeats[lastFourBeatsIndex];
    lastFourBeats[lastFourBeatsIndex++] = now;
    lastFourBeatsIndex = lastFourBeatsIndex % lastBeatsCount;
    unsigned long elapsed = now - then;
    float secondsSpent = (float) elapsed / 1000000.0;
    float bpm = 60.0 * lastBeatsCount / secondsSpent;
    Serial.print(bpm);
    Serial.print(" ");
    //Serial.print(elapsed);
    //Serial.print(" ");
    Serial.println(lastFourBeatsIndex);

    float h = random(256) / 255.0;
    //float h = randomHueNear(currentH, hueNextRadius);
    leds.setHSV(h, 1.0, 1.0);
    currentH = h;
    currentV = 1.0;
    fadingH = 0;
    //Serial.println(h);
}

void beatOff() {
    fadingH = random(0, 2) == 0 ? -1 : 1;
}

// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 9.1f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample / 160.f;
    yv[0] = yv[1]; 
    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 7.015f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
}

byte sampleToSerial(float sample) {
    int s = max(0, sample + 503);
    return s << 2;
}

void loop() {
    /*
    while (1) {
        beatOn();
        delay(4 * 0.5 * 60*1000 / 120);
        beatOff();
        delay(4 * 0.5 * 60*1000 / 120);
    }
    */

    unsigned long time = micros(); // Used to track rate
    unsigned int usample;
    float sample, value, envelope, beat, thresh;

    int iterations = 150 * 200;
    int j = 0;
    int breakAfterJIterations = iterations / 200;
    unsigned long start = micros();
    unsigned long waited = 0;
    for(int i = 0;; ++i) {
        // Read ADC and center so +-512
        usample = analogRead(0);
        if (usample == 1023 || usample < 10) {
            clipCounter = clipCounterMax;
            digitalWrite(clipLed, HIGH);
        }
        if (clipCounter > 0) {
            clipCounter--;
            if (clipCounter == 0)
                digitalWrite(clipLed, LOW);
        }

        sample = (float) usample - 503.f;

        // Filter only bass component
        value = bassFilter(sample);
        //Serial.write(sampleToSerial(value));

        // Take signal amplitude and filter
        if(value < 0)value=-value;
        envelope = envelopeFilter(value);

        // Every 200 samples (25hz) filter the envelope 
        if(i == 200) {
            // Filter out repeating bass sounds 100 - 180bpm
            beat = beatFilter(envelope);

            // Threshold it based on potentiometer on AN1
            // thresh = 0.02f * (float)analogRead(1);
            thresh = 9.0;

            // If we are above threshold, light up LED
            if (beat > thresh) {
                if (!beatActive) {
                    digitalWrite(beatLed, HIGH);
                    beatActive = true;
                    beatOn();
                }
            } else {
                if (beatActive) {
                    digitalWrite(beatLed, LOW);
                    beatActive = false;
                    beatOff();
                }
            }

            if (fadingH != 0) {
                currentH += fadingH * hueFadingPerSample;
                if (currentH < 0 || currentH > 1.0)
                    currentH = modHue(currentH);
                currentV = max(minimumValue, currentV * valueFactor);
                leds.setHSV(currentH, 1.0, currentV);
            }

            i = 0;
            j++;
            if (j >= breakAfterJIterations)
                break;
        }

        // Consume excess clock cycles, to keep at 5000 hz
        unsigned long waitedStart = micros();
        for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
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
