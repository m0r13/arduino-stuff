#include "ir44key.h"

IRInput::IRInput(int pin)
    : receiver(pin), lastKey(0), lastKeyPressed(0), lastKeyPressCount(0) {
}

void IRInput::enableIRIn() {
    receiver.enableIRIn();
}

bool IRInput::processInput(unsigned long& key, int& pressCount) {
    if (!receiver.decode(&results)) {
        if (lastKey != 0 && millis() - lastKeyPressed > 150) {
            // last key was released
            releasedKey = lastKey;
            lastKey = 0;
        }
        return false;
    }
    receiver.resume();

    key = results.value;
    if (key == REPEAT)
        key = lastKey;
    if (key == lastKey) {
        // this key is the same as last -> increase count
        lastKeyPressCount++;
    } else {
        // another key -> last key was released, reset count
        releasedKey = lastKey;
        lastKeyPressCount = 1;
    }

    lastKey = key;
    lastKeyPressed = millis();
    pressCount = lastKeyPressCount;
    return true;
}

bool IRInput::hasReleasedKey() const {
    return releasedKey != 0;
}

unsigned long IRInput::getReleasedKey() const {
    return releasedKey;
}

void IRInput::setReleasedKeyProcessed() {
    releasedKey = 0;
}

