#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void showText(LiquidCrystal_I2C& lcd, String string) {
    int row = 0;
    string.trim();
    while (string.length() > 0) {
        lcd.setCursor(0, row);
        
        lcd.print(string.substring(0, min(20, string.length())));
        row++;
        string = string.substring(min(20, string.length() + 1));
        string.trim();
    }
}


int wait1 = 2500;
int wait2 = 500;

void pepps(LiquidCrystal_I2C& lcd, String string) {
    showText(lcd, string);
    delay(wait1);
    lcd.clear(); lcd.home();
    delay(wait2);
}

void setup() {
    Serial.begin(9600);

    lcd.begin(20, 4);
    lcd.noBlink();

    lcd.setCursor(0, 0);
    //lcd.print("Hello World! abcdefghijklmnopqrstuvwxyz");
    //lcd.setCursor(0, 1);
    //lcd.print("Haben wir noch Pepps dabei?");
    // showText(lcd, "Haben wir noch Pepps dabei?");
}
 
void loop() {
    /*
    lcd.setCursor(0, 0);
    lcd.print("Sec since start:");
    lcd.setCursor(0, 1);
    lcd.print(millis() / 1000);
    */

    //delay(1000);
    //Serial.println("Test");

    /*
    pepps(lcd, "Haben wir noch Pepps?");
    pepps(lcd, "Ne?");
    pepps(lcd, "Wer hat den letzten genommen?");
    pepps(lcd, "Hast du die Pepps noch genommen?");
    pepps(lcd, "Hallo! Hast du die Pepps noch genommen?");
    */

    delay(3000);
    pepps(lcd, "Haben wir noch Pepps dabei?");
    pepps(lcd, "Ha'm wir die noch?");
    pepps(lcd, "Keins mehr?");
    pepps(lcd, "Gar nix mehr??");
    pepps(lcd, "Gar keine mehr???");
    pepps(lcd, "Gar kein Pepps mehr?");
    pepps(lcd, "Zwei noch?");
}
