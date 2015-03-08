#include <Arduino.h>
#include <limits.h>
#include <Async.h>

asyncclass::asyncclass() {
  time = quantum = micros();
  for(int i=0; i<ASYNC_SLOTS; ++i) fn[i] = asyncclass::nullfn;
  for(int i=0; i<ASYNC_PINS; ++i) {
    pinFired[i] = pinReading[i] = pinDelay[i] = 0;
    pinVal[i] = HIGH;
  }
}

void asyncclass::loopSlots() {
  for(int i=0; i<ASYNC_SLOTS; ++i) {
    if(fn[i]==asyncclass::nullfn) continue;
    if(timediff(fnStamp[i], time) >= fnPeriod[i]) {
      fnStamp[i] = time;
      fn[i]();
      if(!fnRepeat[i]) clearSlot(i);
    }
  }
}

void asyncclass::loopPins() {
  for(int i=0; i<ASYNC_PINS; ++i) {
    if(pinFired[i]==0 && pinReading[i]==0) continue;
    int notVal = pinVal[i]==HIGH ? LOW : HIGH;
    if(pinReading[i]==0 && timediff(pinFired[i],time) >= pinDelay[i]) {
      digitalWrite(i, notVal);
      pinFired[i] = pinDelay[i] = 0;
    }
    else if(pinReading[i]>0) {
      if(pinFired[i]==0 && digitalRead(i)==pinVal[i]) pinFired[i] = time;
      else if(pinFired[i]>0 && digitalRead(i)==notVal) pinDelay[i] = time;
    }
  }
}

void asyncclass::loop() {
  ulong time2 = micros();
  quantum = time2 - time;
  time = time2;
  loopSlots();
  loopPins();
}

unsigned long asyncclass::timediff(ulong from, ulong to) {
  if(to >= from) return to - from;
  return ULONG_MAX - from + to;
}

int asyncclass::setSlot(handler f, ulong us, int slot, bool repValue) {
  if(slot==-1) slot = getFreeSlot();
  if(slot==-1) return slot;
  fn[slot] = f;
  fnPeriod[slot] = us;
  fnStamp[slot] = 0;
  fnRepeat[slot] = repValue;
  return slot;
}

int asyncclass::interval(handler f, ulong us, int slot) {
  return setSlot(f, us, slot, true);
}

int asyncclass::intervalms(handler f, ulong ms, int slot) {
  return setSlot(f, ms*1000, slot, true);
}

int asyncclass::timeout(handler f, ulong us, int slot) {
  return setSlot(f, us, slot, false);
}

int asyncclass::timeoutms(handler f, ulong ms, int slot) {
  return setSlot(f, ms*1000, slot, false);
}

int asyncclass::getFreeSlot() {
  for(int i=0; i<ASYNC_SLOTS; ++i) if(fn[i]==asyncclass::nullfn) return i;
  return -1;
}

void asyncclass::clearSlot(int slot) {
  fn[slot] = asyncclass::nullfn;
}

void asyncclass::pulse(int pin, ulong us, int value) {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, value);
  pinFired[pin] = time;
  pinDelay[pin] = us;
  pinVal[pin] = value;
}

void asyncclass::pulsems(int pin, ulong ms, int value) {
  pulse(pin, ms*1000, value);
}

void asyncclass::pulseIn(int pin, int value) {
  pinMode(pin, INPUT);
  pinReading[pin] = time;
  pinVal[pin] = value;
}

bool asyncclass::pinBusy(int pin) {
  return pinFired[pin]!=0 || pinReading[pin]!=0;
}

void asyncclass::clearPin(int pin, int value) {
  pinReading[pin] = pinFired[pin] = pinDelay[pin] = 0;
  if(value!=asyncclass::noChange) digitalWrite(pin, value);
}

int asyncclass::noChange = -511;


asyncclass Async;
