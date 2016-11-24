#include <SPI.h>
#include <LiquidCrystal_I2C.h>

const int CS_NEGATIVE = 5;
const int CS_POSITIVE = 8;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void potWrite(int cs, int value) {
    digitalWrite(cs, LOW);
    SPI.transfer(0x00);
    SPI.transfer(value);
    digitalWrite(cs, HIGH);
}

class DigitalGain {
public:
    DigitalGain(size_t taps, float resistor1, float resistor2, bool negativeGain = false)
        : taps(taps), resistor1(resistor1), resistor2(resistor2), negativeGain(negativeGain) {
        minTap = 1;
        maxTap = taps-1;
        minGain = gainDb(minTap);
        maxGain = gainDb(maxTap);
        if (maxGain < minGain) {
            minTap = taps-1;
            maxTap = 1;
            minGain = gainDb(minTap);
            maxGain = gainDb(maxTap);
        }
    }

    float getMinTap() const { return minTap; }
    float getMaxTap() const { return maxTap; }
    float getMinGain() const { return minGain; }
    float getMaxGain() const { return maxGain; }

    float gain(int tap) const {
        if (negativeGain)
            return resistor1 * tap / (taps-1) / resistor2;
        return resistor1 / (resistor2 * tap / (taps-1));
    }

    float inverseGain(float gain) const {
        if (negativeGain)
            return resistor2 * gain * (taps-1) / resistor1;
        return resistor1 / gain / resistor2 * (taps-1);
    }

    float gainDb(int tap) const {
        return 20.0 * log10(gain(tap));
    }

    float inverseGainDb(float db) const {
        return inverseGain(pow(10.0, db / 20.0));
    }

protected:
    size_t taps;
    float resistor1, resistor2;
    bool negativeGain;

    size_t minTap, maxTap;
    float minGain, maxGain;
};

void setup() {
    Serial.begin(9600);
    lcd.begin(20, 4);
    lcd.noBlink();
    pinMode(CS_NEGATIVE, OUTPUT);
    pinMode(CS_POSITIVE, OUTPUT);
    digitalWrite(CS_NEGATIVE, HIGH);
    digitalWrite(CS_POSITIVE, HIGH);
    SPI.begin();
}

float mix(float a, float b, float alpha) {
    return a*(1 - alpha) + b*alpha;
}

const size_t taps = 128;
DigitalGain negativeGain(taps, 10*1000, 10*1000, true);
DigitalGain positiveGain(taps, 10*1000, 10*1000, false);

void loop() {
    float analogValue = min(1.0, (analogRead(A0) / 4) / 256.0);

    float minGain = max(-20.0f, negativeGain.getMinGain());
    float maxGain = min(20.0f, positiveGain.getMaxGain());
    float gain = mix(minGain, maxGain, analogValue);

    size_t tap1 = taps-1, tap2 = taps-1;
    float actualGain;
    if (gain < 0) {
        tap1 = int(negativeGain.inverseGainDb(gain));
        actualGain = negativeGain.gainDb(tap1);
    } else {
        tap2 = int(positiveGain.inverseGainDb(gain));
        actualGain = positiveGain.gainDb(tap2);
    }

    lcd.setCursor(0, 0);
    lcd.print("Gain: ");
    lcd.print(gain);
    lcd.print("dB ");
    lcd.setCursor(0, 1);
    lcd.print("Tap 1: ");
    lcd.print(tap1);
    lcd.print(" Tap2: ");
    lcd.print(tap2);
    lcd.print("  ");
    lcd.print("   ");
    lcd.setCursor(0, 2);
    lcd.print("Act. gain: ");
    lcd.print(actualGain);
    lcd.print("dB ");
    /*
    lcd.setCursor(0, 3);
    lcd.print("Test: ");
    lcd.print(analogValue);
    lcd.print("   ");
    */
    
    potWrite(CS_NEGATIVE, tap1);
    potWrite(CS_POSITIVE, tap2);

    /*
    Serial.print("Min gain: ");
    Serial.print(minGain);
    Serial.println("dB ");
    Serial.print("Max gain: ");
    Serial.print(maxGain);
    Serial.println("dB");
    Serial.println(inverseGainDb(minGain));
    Serial.println(inverseGainDb(maxGain));
    delay(5000);
    */
}
