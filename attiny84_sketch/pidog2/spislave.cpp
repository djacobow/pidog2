#include <Arduino.h>
#include "spislave.h"
#include <avr/interrupt.h>
#include <avr/io.h>

ISR(PCINT1_vect)  { 
    spislave_c::getInstance()->_ss_int();
}

ISR(USI_OVF_vect) { 
    spislave_c::getInstance()->_byte_int();
}

/*
 * USIDR == Data Register (loaded as clocked)
 * USIBR == Buffer REgister (loaded after overflow)
 * USISR == status register
 */

void
spislave_c::init() {
    pinMode(PIN_MISO, INPUT); // do not switch to output unless /CS
    pinMode(PIN_MOSI, INPUT);
    pinMode(PIN_SCK,  INPUT);
    pinMode(PIN_SS ,  INPUT);

    // three wire mode
    USICR = _BV(USIWM0) | _BV(USICS1);

    // enable interrupts on SS only
    PCMSK1 = _BV(PCINT8);
    GIMSK  = _BV(PCIE1);
}

void
spislave_c::_ss_int() {
    if (!digitalRead(PIN_SS)) {
        pinMode(PIN_MISO, OUTPUT);
        if (false && psrl) {
            psrl->println("T_start");
            // psrl->print("last USIDR ");
            // psrl->println(USIDR,HEX);
        }
        bctr = 0;
        USICR |= _BV(USIOIE);
        USISR = _BV(USIOIF);
        USIDR = cmd_out;
    } else {
        USICR &= ~_BV(USIOIE);
        USISR = 0;
        bctr = -1;
        pinMode(PIN_MISO, INPUT);
        // if (psrl) psrl->println("T_end");
    } 


};

void 
spislave_c::_byte_int() {
    uint8_t newbyte = USIDR;

    USISR = _BV(USIOIF);
    USIDR = 0xff & (dv_out >> 24);

    // do not do anythign at all if the SS is not low
    if (digitalRead(PIN_SS) || (bctr < 0)) return; 

    if (!bctr) {
        cmd_in = newbyte;
    } else {
        dv_in = (dv_in << 8) | newbyte;
    }

    if (false && psrl) {
        psrl->print("counter: ");
        psrl->print(bctr);
        psrl->print(" cmd_in: ");
        psrl->print(cmd_in,HEX);
        psrl->print(" dv_in: ");
        psrl->print(dv_in,HEX);
        psrl->print(" cmd_out: ");
        psrl->print(cmd_out,HEX);
        psrl->print(" dv_out: ");
        psrl->println(dv_out,HEX);
    }

    dv_out <<= 8;
  
    if (bctr == 4) {
        reg_t rv = ch(cmd_in, dv_in);
        dv_out = rv;
        cmd_out = cmd_in | 0x40;
        bctr = -1;
    }

    bctr += 1;
};

