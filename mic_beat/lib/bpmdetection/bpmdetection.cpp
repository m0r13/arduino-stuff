#include "bpmdetection.h"

BPMDetection::BPMDetection()
    : lastFourBeatsIndex(0) {
    for (int i = 0; i < LAST_BEATS_COUNT; i++) {
        lastFourBeats[i] = micros();
    }
}

float BPMDetection::getBPM() const {
    return 420.0;
}

void BPMDetection::beatOn() {
    unsigned long now = micros();
    unsigned long then = lastFourBeats[lastFourBeatsIndex];
    lastFourBeats[lastFourBeatsIndex++] = now;
    lastFourBeatsIndex = lastFourBeatsIndex % LAST_BEATS_COUNT;
    unsigned long elapsed = now - then;
    float secondsSpent = (float) elapsed / 1000000.0;
    float bpm = 60.0 * LAST_BEATS_COUNT / secondsSpent;
    Serial.print(bpm);
    Serial.print(" ");
    //Serial.print(elapsed);
    //Serial.print(" ");
    Serial.println(lastFourBeatsIndex);
}

void BPMDetection::beatOff() {
}

