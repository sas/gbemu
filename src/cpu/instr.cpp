#include <map>
#include <string>

#include "instr.h"

namespace gbemu { namespace cpu {

class unique_instr : public instr
{
private:
  enum unique_op
  {
    nop = 0x00,   stop = 0x10,
    halt = 0x76,
    di = 0xf3,    ei = 0xfb,
  };

public:
  unique_instr(uint8_t opcode)
    : op_((unique_op) opcode)
  {
    (void) opcode;
  }

  virtual void exec(gbemu::memory::as& as, gbemu::cpu::registers& regs)
  {
    (void) as;
    (void) regs;
  }

  virtual void dump(std::ostream& out)
  {
    static std::map<unique_op, std::string> op2str = {
      { nop, "nop",  }, { stop, "stop" },
      { halt, "halt", },
      { di, "di",   },  { ei, "ei" },
    };

    out << op2str[op_] << std::endl;
  }

private:
  unique_op op_;
};

class arith_instr : public instr
{
private:
  enum arith_op
  {
    add   = 0x00, adc   = 0x01,
    sub   = 0x02, sbc   = 0x03,
    _and  = 0x04, _xor  = 0x05,
    _or   = 0x06, cp    = 0x07,
  };
  enum arith_src
  {
    b = 0x00,   c = 0x01,
    d = 0x02,   e = 0x03,
    h = 0x04,   l = 0x05,
    hl = 0x06,  a = 0x07,
  };

public:
  arith_instr(uint8_t opcode)
    : op_((arith_op) ((opcode & 0x38) >> 3)), src_((arith_src) (opcode & 0x07))
  {
  }

  virtual void exec(gbemu::memory::as& as, gbemu::cpu::registers& regs)
  {
    (void) as;
    (void) regs;
  }

  virtual void dump(std::ostream& out)
  {
    static std::map<arith_op, std::string> op2str = {
      { add, "add" }, { adc, "adc" },
      { sub, "sub" }, { sbc, "sbc" },
      { _and, "and" }, { _xor, "xor" },
      { _or, "or" }, { cp, "cp" },
    };
    static std::map<arith_src, std::string> src2str = {
      { b, "b" }, { c, "c" },
      { d, "d" }, { e, "e" },
      { h, "h" }, { l, "l" },
      { hl, "(hl)" }, { a, "a" },
    };

    out << op2str[op_] << " a, " << src2str[src_] << std::endl;
  }

private:
  arith_op  op_;
  arith_src src_;
};

class invalid_instr : public instr
{
public:
  invalid_instr(uint8_t opcode)
    : opcode_(opcode)
  {
  }

  virtual void exec(gbemu::memory::as& as, gbemu::cpu::registers& regs)
  {
    (void) as; (void) regs;
  }

  virtual void dump(std::ostream& out)
  {
    out << "invalid (0x" << std::hex << (int) opcode_ << std::dec << ")" << std::endl;
  }

private:
  uint8_t opcode_;
};

instr* decode(const gbemu::memory::as& as, uint16_t addr)
{
  uint8_t opcode;

  opcode = as[addr];

#define FILTER_INSTR(Mask, Value, Type)     \
    else if ((opcode & (Mask)) == (Value))  \
      return new Type(opcode)

  if (false)
    return 0;
  FILTER_INSTR(0xff, 0x00, unique_instr);
  FILTER_INSTR(0xff, 0x10, unique_instr);
  FILTER_INSTR(0xff, 0x76, unique_instr);
  FILTER_INSTR(0xff, 0xf3, unique_instr);
  FILTER_INSTR(0xff, 0xfb, unique_instr);

  FILTER_INSTR(0xc0, 0x80, arith_instr);
  else
    return new invalid_instr(opcode);

#undef FILTER_INSTR
}

}}
