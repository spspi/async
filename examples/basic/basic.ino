#include <Async.h>
int led = 13;
boolean on = false;

void blinker() {
  on = !on;
  digitalWrite(led, on ? HIGH : LOW);
}

void setup() {
  pinMode(led, OUTPUT);
  Async.intervalms(blinker,500); // led L blinks every 500ms
}

void loop() {
  Async.loop();
  // any other code can be here
  // any synchronous delay should be less than 500ms here
}
