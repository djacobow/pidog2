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
    STAT_WDOG_FIRE_CODE = 7,
} status_bits_t;

#endif
