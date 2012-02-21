#ifndef CPU_REGISTERS_H_
# define CPU_REGISTERS_H_

# include <stdint.h>

namespace gbemu { namespace cpu {

class registers
{
private:
  /*
  ** Used to manipulate two 8-bit registers at one time.
  ** I could have used dirty casts, but that's not portable, so... :)
  */
  class double_assign_proxy
  {
  public:
    double_assign_proxy(uint8_t& msb, uint8_t& lsb)
      : msb_(msb), lsb_(lsb)
    {
    }

    double_assign_proxy& operator=(uint16_t val)
    {
      msb_ = val >> 8;
      lsb_ = val & 0xff;
      return *this;
    }

    operator uint16_t ()
    {
      return (msb_ << 8) | lsb_;
    }

  private:
    uint8_t& msb_;
    uint8_t& lsb_;
  };

public:

  uint8_t a, f;
  uint8_t b, c;
  uint8_t d, e;
  uint8_t h, l;
  uint16_t sp;
  uint16_t pc;

  double_assign_proxy af;
  double_assign_proxy bc;
  double_assign_proxy de;
  double_assign_proxy hl;

  registers()
    : af(a, f), bc(b, c), de(d, e), hl(h, l)
  {
  }
};

}}

#endif /* !CPU_REGISTERS_H_ */
