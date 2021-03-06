#include "beatdetection.h"

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

BeatDetection::BeatDetection()
    : i(0) {
}

int BeatDetection::processSample(float sample) {
    float beatThreshold = 9.0;
    i++;

    // Filter only bass component
    float value = bassFilter(sample);

    // Take signal amplitude and filter
    if(value < 0)
        value =- value;
    float envelope = envelopeFilter(value);

    // Every 200 samples (25hz) filter the envelope 
    if (i == 200) {
        i = 0;

        // Filter out repeating bass sounds 100 - 180bpm
        float beat = beatFilter(envelope);

        // Threshold it based on potentiometer on AN1
        // beatThreshold = 0.02f * (float)analogRead(1);
        //beatThreshold = 9.0;
        //beatThreshold = 0.02 * beatThreshold.getValue();

        // If we are above threshold, light up LED
        return beat > beatThreshold ? BEAT_ON : BEAT_OFF;
    }
    return BEAT_KEEP;
}


BeatGenerator::BeatGenerator(float bpm)
    : bpm(bpm),
      maxTimerOn(1000000 * 60 / bpm / 200 * 0.5),
      maxTimerOff(1000000 * 60 / bpm / 200 * 0.5),
      timer(0), currentStatus(false) {
}

int BeatGenerator::processSample(float sample) {
    timer--;
    if (timer <= 0) {
        timer = !currentStatus ? maxTimerOff : maxTimerOn;
        currentStatus = !currentStatus;
        return currentStatus <= 0 ? BeatDetection::BEAT_ON : BeatDetection::BEAT_OFF;
    }
    return BeatDetection::BEAT_KEEP;
}
