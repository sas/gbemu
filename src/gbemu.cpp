#include <iostream>

#include <cpu/registers.h>
#include <memory/as.h>
#include <memory/cartridge.h>
#include <memory/ram.h>
#include <utils/mapped_file.h>

namespace gbemu {
  class gb
  {
    public:
      gb(const gbemu::utils::mapped_file& rom)
        : cartridge_(as_, rom), ram_(as_)
      {
      }

      void run()
      {
        std::cout << as_[0x134] << as_[0x135] << as_[0x136] << std::endl;
      }

    private:
      gbemu::cpu::registers registers_;
      gbemu::memory::as as_;
      gbemu::memory::cartridge cartridge_;
      gbemu::memory::ram ram_;
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
