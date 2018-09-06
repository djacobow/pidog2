#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include "regfile.h"
#include "spislave.h"
#include "ema.h"
#include "adcReader.h"
#include "spi_reg_names.h"

#define SERIAL_DEBUG 1

#ifdef SERIAL_DEBUG
  #include "SoftwareSerial_tx.h"
#endif

typedef uint32_t reg_t;

const uint8_t PIN_LED_WARN = 1;
const uint8_t PIN_LED_0    = 2;
const uint8_t PIN_PWR      = 3;

#ifdef SERIAL_DEBUG
SoftwareSerialTX srl(PIN_LED_WARN);
#endif

#define VERSION_MAJOR 0x02
#define VERSION_MINOR 0x01
const reg_t   HW_VERSION          = (VERSION_MAJOR << 8) | VERSION_MINOR;

const reg_t DEFAULT_ON_TIME     = 180;
const reg_t DEFAULT_OFF_TIME    = 180;
const reg_t WARN_SECS           = 30;

const size_t rf_size = 10;

typedef regfile_c<reg_t, rf_size> myregfile_c;

myregfile_c rf;
adcReader_c <myregfile_c, 1, 16> adcreader(rf);

uint32_t last_tick;
uint32_t loop_count;

reg_t handleCommand(uint8_t cmd, reg_t indata) {
    reg_t odata = 0;
    uint8_t insn = (cmd & 0xc0) >> 6;
    reg_names_t reg = cmd & 0xf;

    if (reg >= rf_size - 1) insn = 0;
    switch (insn) {
        case 0:
           odata = rf.get(reg);
           break;
        case 1:
           odata = rf.setOr(reg, indata);
           break;
        case 2:
           odata = rf.setAnd(reg, indata);
           break;
        case 3:
           odata = rf.set(reg, indata);
           break;
    }
    // rf.dump();
    return odata;
};

spislave_c  spi(&handleCommand);



void doSecondWork() {
    uint32_t s = rf.get(REG_STATUS);
    if ((s & _BV(STAT_WDOG_EN)) && (s & _BV(STAT_PWR_ON))) {
        reg_t on_rem = rf.get(REG_ON_REMAINING);
        if (!on_rem) {
            s |=  _BV(STAT_WDOG_FIRED);
            s &= ~_BV(STAT_WAKE_FIRED);
            s &= ~_BV(STAT_PWR_ON);
            rf.set(REG_OFF_REMAINING,rf.get(REG_OFF_REM_RESETVAL));
            rf.sethl(REG_FIRECOUNTS,rf.gethl(REG_FIRECOUNTS,register_bottom)+1,register_bottom);
        } else {
            if (on_rem < WARN_SECS) {
                s |= _BV(STAT_LED_WARN);
            }
            rf.set(REG_ON_REMAINING,on_rem-1);
        }
    }
    if ((s & _BV(STAT_WAKE_EN)) && (~s & _BV(STAT_PWR_ON))) {
        reg_t off_rem = rf.get(REG_OFF_REMAINING);
        if (!off_rem) {
            s |= _BV(STAT_WAKE_FIRED);
            s |= _BV(STAT_PWR_ON);
            s &= ~_BV(STAT_LED_WARN);
            s &= ~_BV(STAT_WDOG_FIRED);
            rf.set(REG_ON_REMAINING,rf.get(REG_ON_REM_RESETVAL));
            rf.sethl(REG_FIRECOUNTS,rf.gethl(REG_FIRECOUNTS,register_top)+1,register_top);
        } else {
            if (off_rem < WARN_SECS) {
                s |= _BV(STAT_LED_WARN);
            }
            rf.set(REG_OFF_REMAINING,off_rem-1);
        }
    }

    bool suppress = true;
    if (!suppress) {
        digitalWrite(PIN_PWR,      s & _BV(STAT_PWR_ON));
#ifndef SERIAL_DEBUG
        digitalWrite(PIN_LED_WARN, s & _BV(STAT_LED_WARN));
        digitalWrite(PIN_LED_0,    s & _BV(STAT_WDOG_FIRED));
#endif
    }

    rf.set(REG_STATUS,s);
    rf.dump();
};


const uint8_t TICKS_PER_SECOND = 16;
const uint32_t MICROS_PER_TICK = (1e6 / TICKS_PER_SECOND);

uint8_t floopy;

void doTickWork() {
    static uint8_t tick_count;
    bool second = ! (tick_count % TICKS_PER_SECOND);
    tick_count += 1;
    wdt_reset();

    if (second) { 
        doSecondWork();
        return;
    }
   
    adcreader.doRead();
    return;

}


void setup() {


    pinMode(PIN_PWR,      OUTPUT);
    pinMode(PIN_LED_WARN, OUTPUT);
#ifndef SERIAL_DEBUG
    pinMode(PIN_LED_0,    OUTPUT);
#endif

    if (true) {
        for (uint8_t i=0;i<11;i++) {
            digitalWrite(PIN_LED_0,i&0x1);
            delay(i & 0x1 ? 10 : 20);
        }
    }
    noInterrupts();

    // initialize the register file, start
    // in shutdon mode
    rf.clear();
#ifdef SERIAL_DEBUG
    srl.begin(38400);
    srl.print("hello!");
    rf.set_debug(&srl);
#endif
    rf.set(rf_size - 1, HW_VERSION);
    rf.set(REG_ON_REMAINING, DEFAULT_ON_TIME);
    rf.set(REG_OFF_REMAINING, DEFAULT_OFF_TIME);
    rf.set(REG_OFF_REM_RESETVAL, DEFAULT_OFF_TIME);
    rf.set(REG_ON_REM_RESETVAL,  DEFAULT_ON_TIME);
    rf.set(REG_FIRECOUNTS, 0);
    rf.set(REG_STATUS, 
        _BV(STAT_WDOG_EN) |
        _BV(STAT_WAKE_EN) |
        _BV(STAT_PWR_ON)
    );

    wdt_enable(WDTO_8S);

    spi.init();
    spi.setDebug(&srl);
    setupSPIInterrupts(&spi);

    interrupts();
    last_tick = millis();
    loop_count = 0;
}

void loop() {
    static uint32_t last_time;
    if (micros() - last_time >= MICROS_PER_TICK) {
        last_time += MICROS_PER_TICK;
        doTickWork();
        loop_count += 1;
    }
    delayMicroseconds(MICROS_PER_TICK / 10);
}


ISR(BADISR_vect)  { }
