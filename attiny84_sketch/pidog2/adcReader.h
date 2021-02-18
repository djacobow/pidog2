
#ifndef _ANALOG_H
#define _ANALOG_H

#include "spi_reg_names.h"
#include "regfile.h"
#include "ema.h"

template<typename RF_TYPE, uint8_t EMA_ALPHA, uint8_t EMA_DENOM>
class adcReader_c {
    private:
        uint8_t next;
        RF_TYPE &rf;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_vcc;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_vcc_sw;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_vcc33;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_vbat_a;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_vbat_b;
        ema_c <uint16_t, uint32_t, EMA_ALPHA, EMA_DENOM> avg_temp;
        
    public:
        adcReader_c(RF_TYPE &irf) : next(0), rf(irf) { };
;
        void doInit() {
            avg_vcc.init(readOwnVCC());
            avg_temp.init(readOwnTemp());
            analogReference(INTERNAL1V1);
            avg_vcc_sw.init(analogRead(A2));
            avg_vcc33.init(analogRead(A3));
            avg_vbat_a.init(analogRead(A1));
            avg_vbat_b.init(analogRead(A0));
        }
        
        uint8_t doRead() {
            reg_names_t r = _REG_INVALID;
            reg_t v       = 0xdeadbeef;
            register_half_t h = register_bottom;
            switch (next % 6) {
                 
                case 0 :
                    r = REG_TEMP_V33;
                    h = register_top;
                    v = avg_temp.update(readOwnTemp());
                    break;
                case 1 :
                    r = REG_TEMP_V33;
                    h = register_bottom;
                    analogReference(INTERNAL1V1);
                    v = avg_vcc33.update(analogRead(A3));
                    break;
                case 2 :
                    r = REG_VSENSA_VSENSB;
                    h = register_top;
                    analogReference(INTERNAL1V1);
                    v = avg_vbat_b.update(analogRead(A0));
                    break;
                case 3 :
                    r = REG_VSENSA_VSENSB;
                    h = register_bottom;
                    analogReference(INTERNAL1V1);
                    v = avg_vbat_a.update(analogRead(A1));
                    break;
                case 4 :
                    r = REG_V5_V5SWTCH;
                    h = register_top;
                    v = avg_vcc.update(readOwnVCC());
                    break;
                case 5 :
                    r = REG_V5_V5SWTCH;
                    h = register_bottom;
                    analogReference(INTERNAL1V1);
                    v = avg_vcc_sw.update(analogRead(A2));
                    break;
            } 
            
            if (r != _REG_INVALID) rf.sethl(r,v,h);
            next += 1;
            return next;
        };

        uint16_t readOwnTemp() {
            ADMUX   = B100010; // select temp sensor
            // ADMUX  &= ~_BV(ADLAR);
            // select 1.1 volt reference
            ADMUX  |=  _BV(REFS1); 
            ADMUX  &= ~_BV(REFS0);
            delay(2);
            // no interrupts and no auto-trigger
            ADCSRA &= ~(_BV(ADATE) | _BV(ADIE));
            // turn her on and start a conversion
            ADCSRA |= _BV(ADEN);
            ADCSRA |= _BV(ADSC);
            delay(2);
            while (bit_is_set(ADCSRA,ADSC));
            // supposed to read ADCL first, then ADCH
            uint16_t ov = ADCL;
            ov |= (ADCH << 8);
            return ov;
        }

        uint16_t readOwnVCC() {
            uint32_t result;
            ADMUX = _BV(MUX5) | _BV(MUX0);
            delay(2);
            ADCSRA |= _BV(ADSC);
            while (bit_is_set(ADCSRA,ADSC));
            result = ADCL;
            result |= ADCH << 8;
            // reading directly in mV
            result = 1126400L / result;
            return result;
        };

};
#endif
