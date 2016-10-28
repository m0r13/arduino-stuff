#include "ir44key.h"

#include <SoftwareSerial.h>

bool IR44Key::isColorKey(unsigned long key) {
    return getColorOfKey(key) != 0;
}

unsigned long IR44Key::getColorOfKey(unsigned long key) {
    switch (key) {
    case RED:       return 0xff0000;
    case GREEN:     return 0x00ff00;
    case BLUE:      return 0x0000ff;
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


SoftwareSerial mySerial(8, 12);

IRInput::IRInput(int pin)
    : receiver(pin), lastKey(0), lastKeyPressed(0), lastKeyPressCount(0), releasedKeyPressCount(0) {
}

void IRInput::enableIRIn() {
    //receiver.enableIRIn();
    mySerial.begin(9600);
}

bool IRInput::processInput(unsigned long& key, int& pressCount) {
    if (mySerial.available() >= 4) {
        key = 0;
        key |= mySerial.read();
        key |= (unsigned long) mySerial.read() << 8;
        key |= (unsigned long) mySerial.read() << 16;
        key |= (unsigned long) mySerial.read() << 24;
    } else {
    //if (!receiver.decode(&results)) {
        if (lastKey != 0 && millis() - lastKeyPressed > 150) {
            // last key was released
            releasedKey = lastKey;
            releasedKeyPressCount = lastKeyPressCount;
            lastKey = 0;
        }
        return false;
    }
    receiver.resume();

    //key = results.value;
    if (key == REPEAT)
        key = lastKey;
    if (key == lastKey) {
        // this key is the same as last -> increase count
        lastKeyPressCount++;
    } else {
        // another key -> last key was released, reset count
        releasedKey = lastKey;
        releasedKeyPressCount = lastKeyPressCount;
        lastKeyPressCount = 1;
    }

    lastKey = key;
    lastKeyPressed = millis();
    pressCount = lastKeyPressCount;
    return true;
}

bool IRInput::getReleasedKey(unsigned long& key, int& pressCount) {
    if (releasedKey == 0)
        return false;
    key = releasedKey;
    pressCount = releasedKeyPressCount;
    releasedKey = 0;
    releasedKeyPressCount = 0;
    return true;
}

