#include <IRremote.h>

int RECV_PIN = 11;

IRrecv irrecv(RECV_PIN);

decode_results results;
unsigned long last;

void setup() {
    Serial.begin(9600);
    irrecv.enableIRIn();
}

void loop() {
    if (irrecv.decode(&results)) {
        unsigned long key = results.value;
        Serial.println(key, HEX);
        irrecv.resume(); // Receive the next value
    }
}

