
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
        spislave_c() : psrl(0), bctr(-1), dv_in(0), dv_out(0), cmd_in(0), cmd_out(0) {};
        cmdhandler_t ch;
        Stream *psrl;
        int8_t bctr;
        reg_t   dv_in, dv_out;
        uint8_t cmd_in, cmd_out;

    public:
        static spislave_c *getInstance() {
            static spislave_c instance;
            return &instance;
        }
        void setCmdHandler(cmdhandler_t ich) { ch = ich; }
        void setDebug(Stream *s) { psrl = s; };
        void init();
        void _ss_int();
        void _byte_int();
};

#endif

