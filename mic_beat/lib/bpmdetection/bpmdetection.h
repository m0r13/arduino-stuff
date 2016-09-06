#ifndef bpmdetection_h
#define bpmdetection_h

#include <Arduino.h>

class BPMDetection {
public:
    BPMDetection();

    float getBPM() const;
    
    void beatOn();
    void beatOff();

protected:
    static const int LAST_BEATS_COUNT = 8;
    unsigned long lastFourBeats[LAST_BEATS_COUNT];
    int lastFourBeatsIndex;
};

#endif
