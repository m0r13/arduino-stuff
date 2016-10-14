#include "ir44key.h"

bool IR44Key::isColorKey(unsigned long key) {
    return getColorOfKey(key) != 0;
}

unsigned long IR44Key::getColorOfKey(unsigned long key) {
    switch (key) {
    case RED:       return 0xff0000;
    case GREEN:     return 0x00ff00;
    case BLUE:      return 0x00ff00;
    case WHITE:     return 0xffffff;

    case C01:       return 0xfc9d59;
    case C02:       return 0x52da5c;
    case C03:       return 0x026ed0;
    case C04:       return 0xf5c7d4;

    case C05:       return 0xfc9d59;
    case C06:       return 0x15d0df;
    case C07:       return 0x23227f;
    case C08:       return 0xf5c7d4;

    case C09:       return 0xfc9d59;
    case C10:       return 0x187b98;
    case C11:       return 0x53234d;
    case C12:       return 0x90d8f1;

    case C13:       return 0xf8ee01;
    case C14:       return 0x0b4a7f;
    case C15:       return 0xb04387;
    case C16:       return 0x09d8f1;

    default:        return 0x000000;
    }
}

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

