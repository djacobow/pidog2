
#ifndef __SPISLAVE_H
#define __SPISLAVE_H

#include <avr/io.h>

#define BITMSK(bpos) (0x1 << bpos)

const uint8_t PIN_MISO = 5;
const uint8_t PIN_MOSI = 4;
const uint8_t PIN_SCK  = 6;
const uint8_t PIN_SS   = 0; // PB0 PCINT8

typedef uint32_t reg_t;
typedef reg_t (*cmdhandler_t)(uint8_t, reg_t);

class spislave_c {
    private:
        cmdhandler_t ch;
        uint8_t bctr;
        uint8_t cmd_in, cmd_out;
        reg_t   dv_in, dv_out;
        Stream *psrl;
    public:
        spislave_c(cmdhandler_t);
        void setDebug(Stream *s) { psrl = s; };
        void init();
        void _ss_int();
        void _byte_int();
        void docmd();
};

void setupSPIInterrupts(spislave_c *p);

#endif

