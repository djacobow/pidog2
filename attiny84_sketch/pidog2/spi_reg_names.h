#ifndef _SPI_REG_NAMES_H
#define _SPI_REG_NAMES_H

#define REGISTER_COUNT (14)

typedef enum reg_names_t {
    REG_STATUS                = 0,
    REG_ON_REMAINING          = 1,
    REG_OFF_REMAINING         = 2,
    REG_ON_REM_RESETVAL       = 3,
    REG_OFF_REM_RESETVAL      = 4,

    REG_TEMP_V33              = 5,
    REG_VSENSA_VSENSB         = 6,
    REG_V5_V5SWTCH            = 7,

    REG_FIRECOUNTS            = 8,
    REG_DEVICE_ID             = 9,

    REG_VSENSE_ON_THRESHOLD   = 10,
    REG_VSENSE_OFF_THRESHOLD  = 11,

    // These scratch registers do not do anything.
    // The Pi can write and read them, and they will
    // persis between Pi powerdowns (as long as the
    // PiDog itself has not reset) so can be used to
    // forward information from one boot of the Pi
    // to another.
    REG_SCRATCH_0             = 12,
    REG_SCRATCH_1             = 13,

    _REG_INVALID              = 255,
} reg_names_t;

typedef enum status_bits_t {
    STAT_WDOG_EN    = 0,
    STAT_WDOG_FIRED = 1,
    STAT_WDOG_SOON  = 2,
    STAT_WAKE_EN    = 3,
    STAT_WAKE_FIRED = 4,
    STAT_PWR_ON     = 5,
    STAT_LED_WARN   = 6,

    // Fire code is 2-bits wide
    STAT_WDOG_FIRE_CODE = 7,
    
    // This register controls how the pidog switches 
    // power on. When set to 0 (default), the power is
    // switched on using the digitalWrite() function
    // and is immediate. When set to 1, the power is 
    // feathered on using a duty cycle that ramps from
    // 0% to 100%. This was implemented to mitigate a
    // pidog brownout that occurs fairly constantly
    // when using a Raspberry Pi4. 
    STAT_SOFT_START     = 9,
    
    // The following status bits are copied directly from
    // bits 0-3 of the ATTiny MCU status register to indicate
    // the reason for the last pidog reset:
    //  PORF  - the reset was caused by power on
    //  EXTRF - the reset was caused externaly
    //  BORF  - the reset was caused vy a voltage brownout
    //  WDRF  - the reset was caused by the ATTiny watchdog
    //  WDCLR - when set by the rpi, the pidog will clear the ATTtiny's MCUSR
    STAT_PORF           = 10,
    STAT_EXTRF          = 11,
    STAT_BORF           = 12,
    STAT_WDRF           = 13,
    STAT_WDCLR          = 14,
} status_bits_t;

#endif
