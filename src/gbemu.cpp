#include <iostream>

#include <cpu/registers.h>
#include <debug/cartridge.h>
#include <utils/mapped_file.h>

namespace gbemu {
  class gb
  {
    public:
      gb(const gbemu::utils::mapped_file& cartridge)
        : cartridge_(cartridge)
      {
      }

      void run()
      {
      }

    private:
      const gbemu::utils::mapped_file& cartridge_;

      gbemu::cpu::registers registers_;
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

  /* open the cartridge */
  gbemu::utils::mapped_file f(argv[1]);
  f.map();

#ifndef NDEBUG
  gbemu::debug::cartridge c(f);
  c.dump(std::cout);
#endif /* !NDEBUG */

  /* create the emulator and run */
  gbemu::gb gb(f);
  gb.run();
}
