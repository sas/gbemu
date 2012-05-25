#ifndef CPU_INSTR_H_
# define CPU_INSTR_H_

# include <iostream>

# include <cpu/registers.h>
# include <memory/as.h>

namespace gbemu { namespace cpu {

class instr
{
public:
  virtual ~instr() {}
  virtual void exec(gbemu::memory::as& as, gbemu::cpu::registers& regs) = 0;
  virtual void dump(std::ostream& out) = 0;
};

instr* decode(const gbemu::memory::as& as, uint16_t addr);

}}

#endif /* !CPU_INSTR_H_ */
