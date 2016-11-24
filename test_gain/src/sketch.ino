#include <SPI.h>
#include <LiquidCrystal_I2C.h>

const int CS = 5;
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void potWrite(int value) {
    digitalWrite(CS, LOW);
    SPI.transfer(0x00);
    SPI.transfer(value);
    digitalWrite(CS, HIGH);
}

const size_t taps = 128;
const float R1 = 10*1000;
const float R2 = 10*1000;

float gain(int tap) {
    return R1 * tap / (taps-1) / R2;
    //return R1 / (R2 * tap / (taps-1));
}

float inverseGain(float gain) {
    return R2 * gain * (taps-1) / R1;
    //return R1 / gain / R2 * (taps-1);
}

float gainDb(int tap) {
    return 20.0 * log10(gain(tap));
}

float inverseGainDb(float db) {
    return inverseGain(pow(10.0, db / 20.0));
}

float minGain, minTap;
float maxGain, maxTap;

float mapRange(float srcA, float srcB, float x, float destA, float destB) {
    
};

void setup() {
    Serial.begin(9600);
    lcd.begin(20, 4);
    lcd.noBlink();
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);
    SPI.begin();

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

float mix(float a, float b, float alpha) {
    return a*(1 - alpha) + b*alpha;
}

void loop() {
    float analogValue = min(1.0, analogRead(A0) / 1021.0);
    Serial.println(analogValue);

    float gain = mix(minGain, maxGain, analogValue);
    int tap = int(inverseGainDb(gain));
    if (tap < 1) {
        tap = 1;
    } else if (tap >= taps) {
        tap = taps - 1;
    }
    float actualGain = gainDb(tap);

    lcd.setCursor(0, 0);
    lcd.print("Gain: ");
    lcd.print(gain);
    lcd.print("dB ");
    lcd.setCursor(0, 1);
    lcd.print("Poti tap: ");
    lcd.print(tap);
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
    
    potWrite(tap);

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
