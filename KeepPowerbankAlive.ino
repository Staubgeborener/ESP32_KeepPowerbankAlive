#define BLINK_GPIO 21 // define gpio
unsigned long previousMillis;

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 5000) {
        previousMillis = currentMillis;
        digitalWrite(BLINK_GPIO, HIGH); 
        delay(50);
        digitalWrite(BLINK_GPIO, LOW);
    }
}

void setup()
{
    pinMode(BLINK_GPIO, OUTPUT);
}
