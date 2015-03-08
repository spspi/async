#include <Async.h>
int led = 13;

void blinker() {
  Async.pulsems(led, 100, HIGH); // turn on led for 100ms
}

// how long does on loop cycle take?
// use control monitor (Ctrl+Shift+M) to see
void info() {
  Serial.println(Async.quantum);
}

void setup() {
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  // watch the paralel processes
  Async.intervalms(blinker,500); // led L blinks every 500ms
  Async.intervalms(info,700); // led TX blinks every 700ms
}

void loop() {
  Async.loop();
  // any other code can be here
  // any synchronous delay should be less than 500ms here
}
