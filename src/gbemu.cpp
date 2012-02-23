#include <iostream>

#include <memory.h>
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
        memory_.add_read_handler(
            { 0, 42 },
            [](uint16_t off){ std::cout << "read at offset " << off << std::endl; return 69; }
        );
      }

      void run()
      {
        for (uint16_t o = 10; o < 20; ++o)
        {
          std::cout << memory_[o] << std::endl;
        }
      }

    private:
      const gbemu::utils::mapped_file& cartridge_;

      gbemu::cpu::registers registers_;
      gbemu::memory memory_;
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

  /* create the emulator and run */
  gbemu::gb gb(f);
  gb.run();
}
