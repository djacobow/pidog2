
#ifndef REGFILE_H
#define REGFILE_H

#include <Arduino.h>
#include <Stream.h>

typedef enum register_half_t {
    register_bottom = 0,
    register_top,
} register_half_t;

template<typename REGW, size_t REG_COUNT>
class regfile_c {
    private:
        REGW registers[REG_COUNT];
        Stream *ser;
        void pp8hex(uint8_t i) {
            if (ser) {
                if (i < 16) ser->print("0");
                ser->print(i,HEX);
            }
        }
        void pp32hex(REGW i) {
            uint8_t j = sizeof(REGW);
            while (j) {
                pp8hex((uint8_t)((i >> ((j-1)*8)) & 0xff));
                j--;
            }
        }

    public:
        regfile_c() {
          ser = 0;
        }
        void set_debug(Stream *nser) {
          ser = nser;
        }
        void clear() {
          for (uint8_t i=0;i<REG_COUNT;i++) registers[i] = 0;
        };

        void dump() {
            if (ser) {
                ser->print("[ ");
                for (uint8_t i=0; i< REG_COUNT; i++) {
                    ser->print("0x");
                    pp32hex(registers[i]);
                    if (i < (REG_COUNT-1)) ser->print(", ");
                }
                ser->println(" ]");
            }
        }

        REGW sethl(uint8_t addr, REGW i, register_half_t half) {
            size_t sz = sizeof(REGW);
            REGW mask = ~0; // all ones
            mask >>= (sz * 4);
            i &= mask;
            if (half == register_bottom) {
                mask <<= (sz * 4);
            } else {
                i <<= (sz * 4);
            }
            REGW o = registers[addr % REG_COUNT];

            o &= mask;
            o |= i;

            registers[addr % REG_COUNT] = o;
            return o;
        }

        REGW gethl(uint8_t addr, register_half_t half) {
            REGW v = registers[addr % REG_COUNT];
            size_t sz = sizeof(REGW);
            if (half != register_bottom) {
                v >>= (sz * 4);
            }
            REGW halfmask = ~(0x0);
            halfmask >>= (sz * 4);
            v &= halfmask;
            return v;
        }

        REGW get(uint8_t addr) { 
            return registers[addr % REG_COUNT];
        }

        REGW setOr(uint8_t addr, REGW val) {
            registers[addr % REG_COUNT] |= val;
            return get(addr);
        }
        REGW setAnd(uint8_t addr, REGW val) {
            registers[addr % REG_COUNT] &= val;
            return get(addr);
        }
        REGW set(uint8_t addr, REGW val) {
            registers[addr % REG_COUNT] = val;
            return get(addr);
        }

        REGW *getptr(uint8_t addr) {
            addr = addr % REG_COUNT;
            return &(registers[addr]);
        };
};

#endif
