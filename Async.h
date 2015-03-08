#pragma once

/**
 * The lower the ASYNC_* macros are, the faster the loop() performance is (the macros means maximal index in arrays, loops them every cycle)
 */

/**
 * The limit of concurrent Async handlers
 */
#ifndef ASYNC_SLOTS
#define ASYNC_SLOTS 16
#endif

/**
 * The maximal index of pin used by Async
 */
#ifndef ASYNC_PINS
#define ASYNC_PINS 16
#endif

/**
 * Performs asynchronous operations. To make it work, place Async.loop() inside the global loop().
 * Be careful with delays, global pulseIn and other synchronous functions that signifficantly slows
 * loop() execution: minimal asynchronous call delay won't be smaller than total synchronous delay.
 * The total synchronous delay (in microseconds) is refreshed into Async.quantum every loop.
 *
 * @author Jan Turoň <janturon@email.cz>
 * @version 1.0, 2015
 */
class asyncclass {
  typedef void(*handler)(); /// this is the format of handlers mentioned below - void f(), where "f" is any function
  typedef unsigned long ulong;

  static void nullfn() { }
  ulong fnStamp[ASYNC_SLOTS];
  bool fnRepeat[ASYNC_SLOTS];
  ulong pinVal[ASYNC_SLOTS];
  static int noChange;

  int setSlot(handler fn, ulong us, int slot, bool repValue);
  void loopSlots();
  void loopPins();

public:
  ulong /// time is measured in microseconds (us) unless specified otherwise
    pinFired[ASYNC_PINS], /// fire time of corresponding pin caused by Async.pulse or Async.pulseIn; should be read only
    pinDelay[ASYNC_PINS], /// how long should pin fire; can be altered anytime
    pinReading[ASYNC_PINS], /// when pin was asked to measure pulseIn; the time between request and actual fire equals to pinFired[i]-pinReading[i]
    fnPeriod[ASYNC_SLOTS]; /// time period of calling the handler at corresponding slot; can be altered anytime
  handler fn[ASYNC_SLOTS]; /// handler at corresponding slot; can be altered anytime
  ulong
    time, /// set to micro() every loop
    quantum; /// time of the previous global loop() execution

  asyncclass(); // do not create instances, use Async object
  void loop(); // call this inside .ino loop()
  
  /**
   * Compute time difference between from a to times (in proper units: ms or us).
   * Handles time oferflow, which uccurs
   *  - every 70 minutes calling micros()
   *  - every 50 days calling millis()
   */
   ulong timediff(ulong from, ulong to);

  /**
   * Call handler periodically
   * @param handler f handler to execute
   * @param ulong us execution period in microseconds
   * @param int slot where to place the handler (0 - ASYNC_SLOTS); if not specified (or -1) the first free slot is taken
   * @return int slot used for handler; -1 = couldn't find free slot
   */
  int interval(handler f, ulong us, int slot=-1);
  
  /**
   * Same as interval, but the period is in milliseconds
   * @see interval
   */
  int intervalms(handler f, ulong ms, int slot=-1);

  /**
   * Same as interval, but the handler is executed only once (after the period).
   * The slot is auto-cleared after the period.
   * @see interval
   */
  int timeout(handler f, ulong us, int slot=-1);

  /**
   * Same as timeout but the period is in milliseconds
   * @see timeout
   */  
  int timeoutms(handler f, ulong ms, int slot=-1);

  /**
   * Clears slot at specified position.
   * If the timeout handler wasn't executed yet, it won't be executed in the future.
   * @param int slot position to clear
   */
  void clearSlot(int slot);
 
   /**
   * Get the first free slot
   * @return int free slot position (or -1 if all slots are taken)
   */
  int getFreeSlot();

  /**
   * Fire at pin for a while. Pin is auto-cleared after use.
   * @param int pin where to fire
   * @param ulong us how long to fire. If value is HIGH, LOW will be set to the pin after the period and vice versa.
   * @param int value HIGH or LOW
   */
  void pulse(int pin, ulong us, int value);
  
  /**
   * Same as pulse, but the time is in milliseconds
   * @see pulse
   */
  void pulsems(int pin, ulong ms, int value);

  /**
   * Wait for pin to fire and measure the length of the pulse. The pin IS NOT cleared after use to enable the following reading:
   * pinFired[pin] - pinReading[pin] = time between request and fire (pinFired[pin]=0 if still waiting for fire)
   * pinDelay[pin] - pinFired[pin] = the length of the pulse (in microseconds) (pinDelay[pin]=0 if pulse is not finished)
   */
  void pulseIn(int pin, int value);

  /**
   * Detect if pin is busy (used by Async.pulse or Async.pulseIn and not cleared yet)
   */
  bool pinBusy(int pin);

  /**
   * Clears the pin (mark as free for pinBusy call).
   * It is auto-cleared after Async.pulse(), but after Async.pulseIn() it needs to be cleared manually).
   * There will be no issue if Async.pulse() or Async.pulseIn() is called on busy pin: busy-clear state is merely informational.
   * @param int pin which pin to clear
   * @param int value which value to set: HIGH or LOW. Omit this param to keep the current value.
   */
  void clearPin(int pin, int value=asyncclass::noChange);
};

extern asyncclass Async; /// global object defined in Async.cpp

