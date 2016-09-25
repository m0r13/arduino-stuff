#ifndef beatdetection_h
#define beatdetection_h

#include <Arduino.h>

float bassFilter(float sample);
float envelopeFilter(float sample);
float beatFilter(float sample);

class BeatDetection {
public:
    BeatDetection();

    int processSample(float sample);

    static const int BEAT_ON = 0;
    static const int BEAT_OFF = 1;
    static const int BEAT_KEEP = 2;

protected:
    int i;
};

class BeatGenerator {
public:
    BeatGenerator(float bpm);

    int processSample(float sample);

protected:
    float bpm;
    int maxTimerOn, maxTimerOff, timer, i;
    bool currentStatus;
};

#endif
