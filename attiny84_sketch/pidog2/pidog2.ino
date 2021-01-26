#include <Arduino.h>
#include <avr/wdt.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include "regfile.h"
#include "spislave.h"
#include "ema.h"
#include "adcReader.h"
#include "spi_reg_names.h"

//#define SERIAL_DEBUG 1
//#define NO_PATIENCE_DEBUG 1

#ifdef SERIAL_DEBUG
  #include "SoftwareSerial_tx.h"
#endif

typedef uint32_t reg_t;

const uint8_t PIN_LED_0    = 1;
const uint8_t PIN_LED_1    = 2;
const uint8_t PIN_PWR      = 3;

uint32_t vhits;
ISR(WDT_vect) { 
    vhits++;
}

#ifdef SERIAL_DEBUG
SoftwareSerialTX srl(PIN_LED_0);
#endif

#define VERSION_MAJOR 0x02
#define VERSION_MINOR 0x06
const reg_t   HW_VERSION        = 
    ((reg_t)'p' << 24)   |
    ((reg_t)'d' << 16)   |
    (VERSION_MAJOR << 8) | 
    VERSION_MINOR;


#ifdef NO_PATIENCE_DEBUG
const reg_t DEFAULT_ON_TIME       = 30;
const reg_t DEFAULT_OFF_TIME      = 30;
const reg_t WARN_SECS             = 10;
const reg_t VSENSE_ON_THRESHOLD   = 0;
const reg_t VSENSE_OFF_THRESHOLD  = 0;
#else
const reg_t DEFAULT_ON_TIME       = 900;
const reg_t DEFAULT_OFF_TIME      = 900;
const reg_t WARN_SECS             = 30;
const reg_t VSENSE_ON_THRESHOLD   = 0; //On theshold in mV. Zero disables this function.
const reg_t VSENSE_OFF_THRESHOLD  = 0; //Off theshold in mV. Zero disables this function.
#endif

const size_t rf_size = 12;

typedef regfile_c<reg_t, rf_size> myregfile_c;

myregfile_c rf;
adcReader_c <myregfile_c, 1, 16> adcreader(rf);

reg_t handleCommand(uint8_t cmd, reg_t indata) {
    reg_t odata = 0;
    uint8_t insn = (cmd & 0xc0) >> 6;
    reg_names_t reg = (reg_names_t)(cmd & 0xf);

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



void doSecondWork() {
    uint32_t s = rf.get(REG_STATUS);
    if ((s & _BV(STAT_WDOG_EN)) && (s & _BV(STAT_PWR_ON))) {
        reg_t on_rem = rf.get(REG_ON_REMAINING);
        reg_t a_off_thresh = rf.gethl(REG_VSENSE_OFF_THRESHOLD,register_top);
        reg_t b_off_thresh = rf.gethl(REG_VSENSE_OFF_THRESHOLD,register_bottom);
        bool a_under = a_off_thresh && (rf.gethl(REG_VSENSA_VSENSB,register_top) < a_off_thresh);
        bool b_under = b_off_thresh && (rf.gethl(REG_VSENSA_VSENSB,register_bottom) < b_off_thresh);
        if ( !on_rem || a_under || b_under) {
            #ifdef SERIAL_DEBUG
            srl.println("Powering off pi.");
            #endif
            s |=  _BV(STAT_WDOG_FIRED);
            s &= ~(_BV(STAT_WDOG_FIRE_CODE) | _BV(STAT_WDOG_FIRE_CODE+1));                        //00b - on-remaining timer expired 
            if (a_under && b_under) s |= (_BV(STAT_WDOG_FIRE_CODE) | _BV(STAT_WDOG_FIRE_CODE+1)); //11b - vsensb & vsensb dropped below threshold 
            else if (a_under)       s |= _BV(STAT_WDOG_FIRE_CODE);                                //01b - vsensa dropped below threshold
            else if (b_under)       s |= _BV(STAT_WDOG_FIRE_CODE+1);                              //10b - vsensb dropped below threshold
            s &= ~_BV(STAT_WAKE_FIRED);
            s &= ~_BV(STAT_PWR_ON);
            s &= ~_BV(STAT_LED_WARN);
            rf.set(REG_OFF_REMAINING,rf.get(REG_OFF_REM_RESETVAL));
            rf.sethl(REG_FIRECOUNTS,rf.gethl(REG_FIRECOUNTS,register_bottom)+1,register_bottom);
        } else {
            if (on_rem < WARN_SECS) s |= _BV(STAT_LED_WARN);
            rf.set(REG_ON_REMAINING,on_rem-1);
        }
    }
    if ((s & _BV(STAT_WAKE_EN)) && (~s & _BV(STAT_PWR_ON))) {
        reg_t off_rem = rf.get(REG_OFF_REMAINING);
        if (!off_rem) {
            reg_t a_on_thresh = rf.gethl(REG_VSENSE_ON_THRESHOLD,register_top);
            reg_t b_on_thresh = rf.gethl(REG_VSENSE_ON_THRESHOLD,register_bottom); 
            bool a_over = rf.gethl(REG_VSENSA_VSENSB,register_top) >= a_on_thresh;
            bool b_over = rf.gethl(REG_VSENSA_VSENSB,register_bottom) >= b_on_thresh;
            bool enable = (!a_on_thresh && !b_on_thresh)                   || 
                          (a_on_thresh && a_over && b_on_thresh && b_over) || 
                          (a_on_thresh && a_over && !b_on_thresh)          || 
                          (b_on_thresh && b_over && !a_on_thresh);
            if (enable == true) {
                #ifdef SERIAL_DEBUG
                srl.println("Powering on pi.");
                #endif
                s |= _BV(STAT_WAKE_FIRED);
                s |= _BV(STAT_PWR_ON);
                s &= ~_BV(STAT_LED_WARN);
                s &= ~_BV(STAT_WDOG_FIRED);
                rf.set(REG_ON_REMAINING,rf.get(REG_ON_REM_RESETVAL));
                rf.sethl(REG_FIRECOUNTS,rf.gethl(REG_FIRECOUNTS,register_top)+1,register_top);
            } else if ( a_on_thresh || b_on_thresh ) {
              rf.set(REG_OFF_REMAINING,rf.get(REG_OFF_REM_RESETVAL));
              s &= ~_BV(STAT_LED_WARN);
            }
        } else {
            if (off_rem < WARN_SECS) s |= _BV(STAT_LED_WARN);
            rf.set(REG_OFF_REMAINING,off_rem-1);
        }
    }

    rf.set(REG_STATUS,s);

    if (true) {
        // NB the power pin has negative polarity
        digitalWrite(PIN_PWR,      !(s & _BV(STAT_PWR_ON)));
#ifndef SERIAL_DEBUG
        digitalWrite(PIN_LED_0,    s & _BV(STAT_LED_WARN));
        digitalWrite(PIN_LED_1,    rf.gethl(REG_FIRECOUNTS,register_bottom) > 0);
#endif
    }

    rf.dump();
};


const uint8_t TICKS_PER_SECOND = 4;
const uint32_t MILLIS_PER_TICK = (1e3 / TICKS_PER_SECOND);
uint32_t next_tick;

void doTickWork() {
    static uint8_t tick_count;
    bool second = ! (tick_count % TICKS_PER_SECOND);
    tick_count += 1;

    if (second) { 
        doSecondWork();
        return;
    }
   
    adcreader.doRead();
    return;

}


void setup() {

    // disable the watchdog
    MCUSR = 0;
    wdt_enable(WDTO_2S);
    wdt_reset();
    // WDTCSR = _BV(WDCE) | _BV(WDIE);

    pinMode(PIN_PWR,      OUTPUT);
    pinMode(PIN_LED_0,    OUTPUT);
#ifndef SERIAL_DEBUG
    pinMode(PIN_LED_1,    OUTPUT);
#endif

    if (false) {
        for (uint8_t i=0;i<11;i++) {
            digitalWrite(PIN_LED_1,i&0x1);
            delay(i & 0x1 ? 10 : 20);
        }
    }
    noInterrupts();

    // initialize the register file, start
    // in shutdon mode
    rf.clear();
#ifdef SERIAL_DEBUG
    srl.begin(38400);
    srl.println("hello!");
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
    rf.set(REG_VSENSE_ON_THRESHOLD,VSENSE_ON_THRESHOLD);
    rf.set(REG_VSENSE_OFF_THRESHOLD,VSENSE_OFF_THRESHOLD);

    spislave_c *spi = spislave_c::getInstance();
    spi->setCmdHandler(&handleCommand);
    spi->init();
#ifdef SERIAL_DEBUG
    spi->setDebug(&srl);
#endif

    interrupts();
#ifdef SERIAL_DEBUG
    srl.println("setup complete");
#endif
    next_tick = millis();
}



void sleep250() {
    uint8_t adcsra_was = ADCSRA;
    ADCSRA = 0;

    wdt_enable(WDTO_250MS);
    wdt_reset();
    WDTCSR |= _BV(WDIE);
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    sleep_mode();
    
    // and now we are awake again
    ADCSRA = adcsra_was;
}


void loop() {
    bool use_sleep = !(rf.get(REG_STATUS) & _BV(STAT_PWR_ON));
    if (use_sleep) {
        sleep250();
        doTickWork();      
    } else {
        wdt_reset();
        bool new_tick = (millis() >= next_tick);
        if (new_tick) {
            doTickWork();
            next_tick += MILLIS_PER_TICK;
        }
        delay(MILLIS_PER_TICK / 10);
    }
}


ISR(BADISR_vect)  { }
