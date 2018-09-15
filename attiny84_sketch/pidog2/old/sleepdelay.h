#ifndef __SLEEPDELAY_H
#define __SLEEPDELAY_H

#include <stdint.h>


class sleeper_c {
    public:
        sleeper_c();
        void sleepMicros(uint16_t t);
        void sleepMillis(uint16_t t);
        void _wake();
    private:
        void _sleepsetup(uint16_t ocr, uint8_t prescal);
        bool sleeping;
};

void setupSleeperInterrupt(sleeper_c *p);

#endif
