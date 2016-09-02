// Arduino Beat Detector By Damian Peckett 2015
// License: Public Domain.

#include <rgb_hsv.h>

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

const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;

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
}

void setColor(int r, int g, int b) {
    analogWrite(redPin, r);
    analogWrite(greenPin, g);
    analogWrite(bluePin, b);
}

void setHSVColor(float h, float s, float v) {
    long rgb = HSV_to_RGB(h, s, v);
    setColor((rgb >> 16) & 0xff, (rgb >> 8) & 0xff, rgb & 0xff);
}

void beatOn() {
    unsigned long now = micros();
    unsigned long then = lastFourBeats[lastFourBeatsIndex];
    lastFourBeats[lastFourBeatsIndex++] = now;
    lastFourBeatsIndex = lastFourBeatsIndex % lastBeatsCount;
    unsigned long elapsed = now - then;
    float took = (float) elapsed / 1000000.0;
    float bpm = 60.0 * lastBeatsCount / took;
    Serial.print(bpm);
    Serial.print(" ");
    Serial.print(elapsed);
    Serial.print(" ");
    Serial.println(lastFourBeatsIndex);

    float h = random(256) / 255.0;
    setHSVColor(h, 1.0, 1.0);
}

void beatOff() {

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
    //Serial.println("Hello!");
    unsigned long time = micros(); // Used to track rate
    unsigned int usample;
    float sample, value, envelope, beat, thresh;
    unsigned long i;

    /*
    unsigned long iterations = 10000;
    unsigned long start = micros();
    */
    for(i = 0;; ++i){
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
            thresh = 0.02f * (float)analogRead(1);

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

            i = 0;
        }

        // Consume excess clock cycles, to keep at 5000 hz
        for(unsigned long up = time+SAMPLEPERIODUS; time > 20 && time < up; time = micros());
    }
    /*
    unsigned long end = micros();
    float took = float(end - start) / 1000000.0;
    float samplerate = iterations / took;
    //Serial.println(samplerate);
    */
}
