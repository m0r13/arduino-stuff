#include <SoftwareSerial.h>
#include <IRremote.h>

int RECV_PIN = 7;

SoftwareSerial mySerial(3, 2);
IRrecv irrecv(RECV_PIN);

decode_results results;
unsigned long last;

void setup() {
    Serial.begin(9600);
    irrecv.enableIRIn();
    delay(5000);
    mySerial.begin(9600);
}

void loop() {
    if (irrecv.decode(&results)) {
        unsigned long key = results.value;
        Serial.println(key, HEX);
        //mySerial.println(key, HEX);
        mySerial.write((char) (key & 0xff));
        mySerial.write((char) ((key >> 8) & 0xff));
        mySerial.write((char) ((key >> 16) & 0xff));
        mySerial.write((char) ((key >> 24) & 0xff));
        //mySerial.println("Hello World!");
        irrecv.resume(); // Receive the next value
    }
}

