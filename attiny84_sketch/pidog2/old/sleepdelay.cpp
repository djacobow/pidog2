#include <Arduino.h>
#include "sleepdelay.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

static sleeper_c *psleeper = 0;
void setupSleeperInterrupt(sleeper_c *p) { psleeper = p; };

ISR(TIMER1_COMPA_vect) {
    if (psleeper) psleeper->_wake();
}

sleeper_c::sleeper_c() : sleeping(false) {};

void
sleeper_c::_wake() {
    TIMSK0 |= _BV(TOIE0);   // get millis running again
    TIMSK1 &= ~_BV(OCIE1A); // clear interrupt enable flag
    TCCR1B &= ~0x7;         // set prescaler to stop timer
    sleeping = false;
};

void
sleeper_c::_sleepsetup(uint16_t ocr, uint8_t prescal) {

    sleeping = true;

    cli();
    
    // disable the millis interrupt, otherwise, it will
    // wake us up right away!
    TIMSK0 &= ~_BV(TOIE0);

    TCCR1A = 0;

    // set prescaler
    TCCR1B &= 0xf8;
    TCCR1B |= 0x5; // (prescal & 0x7); 

    // set CTC mode
    // TCCR1B &= 0xe7;
    // TCCR1B |= 0x08;

    // set compare register to desired duration
    OCR1A = ocr;

    // allow interrupt on compare register A only
    TIMSK1 |= _BV(OCIE1A);

    // reset count
    TCNT1 = 0;

    // set the sleep mode to IDLE -- this is uP stopped but
    // everything else running. Not super sleepy, but this 
    // is the only mode where a timer other than watchdog 
    // can wake
    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();
    sleep_enable();
    sleep_cpu();    
  
    while(sleeping) delayMicroseconds(10);
};

void
sleeper_c::sleepMillis(uint16_t t) {
    _sleepsetup((125UL * (uint32_t)t) >> 4, 0x5);
};

void
sleeper_c::sleepMicros(uint16_t t) {
    _sleepsetup(t, 0x2);
};
