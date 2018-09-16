#ifndef SoftwareSerialTXTX_h
#define SoftwareSerialTXTX_h

#include <inttypes.h>
#include <Stream.h>

/******************************************************************************
* Definitions
******************************************************************************/

#ifndef GCC_VERSION
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

class SoftwareSerialTX : public Stream
{
private:
  // per object data
  uint8_t _transmitBitMask;
  volatile uint8_t *_transmitPortRegister;

  uint16_t _tx_delay;
  uint16_t _inverse_logic:1;

  // static data
  static SoftwareSerialTX *active_object;

  // private methods
  void recv() __attribute__((__always_inline__));
  void tx_pin_write(uint8_t pin_state) __attribute__((__always_inline__));
  void setTX(uint8_t transmitPin);

  // Return num - sub, or 1 if the result would be < 1
  static uint16_t subtract_cap(uint16_t num, uint16_t sub);

  // private static method for timing
  static inline void tunedDelay(uint16_t delay);

public:
  // public methods
  SoftwareSerialTX(uint8_t transmitPin, bool inverse_logic = false);
  ~SoftwareSerialTX();
  void begin(long speed);
  void end();
  bool overflow() { return false; };
  int peek() { return 0; };
  virtual int read() { return 0; };
  virtual int available() { return 0; };

  virtual size_t write(uint8_t byte);
  virtual void flush() {};
  operator bool() { return true; }
  
  using Print::write;

  // public only for easy access by interrupt handlers
  static inline void handle_interrupt() __attribute__((__always_inline__));
};

// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round

#endif
