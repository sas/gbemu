#include <iostream>

#include <cpu/instr.h>
#include <cpu/registers.h>
#include <memory/as.h>
#include <memory/cartridge.h>
#include <memory/ram.h>
#include <utils/mapped_file.h>

namespace gbemu {
  class gb
  {
    public:
      gb(const utils::mapped_file& rom)
        : cartridge_(as_, rom), ram_(as_)
      {
      }

      void run()
      {
        /* dump rom name */
        for (uint16_t addr = 0x134; addr < 0x142; ++addr)
          std::cout << as_[addr];
        std::cout << std::endl;

        /* run */
        for (uint16_t addr = 0; addr < 42000; ++addr)
        {
          gbemu::cpu::instr* instr = gbemu::cpu::decode(as_, addr);
          instr->dump(std::cout);
          delete instr;
        }
      }

    private:
      cpu::registers registers_;
      memory::as as_;
      memory::cartridge cartridge_;
      memory::ram ram_;
  };
}

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    extern char* __progname; /* from crt0.o */
    std::cerr << "usage: " << __progname << " ROM" << std::endl;
    return 1;
  }

  /* open the rom */
  gbemu::utils::mapped_file rom(argv[1]);
  rom.map();

  /* create the emulator and run */
  gbemu::gb gb(rom);
  gb.run();
}
