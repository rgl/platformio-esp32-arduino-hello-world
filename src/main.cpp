#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.printf("%lu Hello World!\r\n", millis()/1000);
  delay(1000);
}
