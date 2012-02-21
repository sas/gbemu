#include <iostream>

#include <cartridge.h>
#include <cpu/registers.h>
#include <utils/mapped_file.h>

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    extern char* __progname; /* from crt0.o. */
    std::cerr << "usage: " << __progname << " ROM" << std::endl;
    return 1;
  }

  gbemu::cpu::registers r;

  r.hl = 0x4269;

  std::cout << std::hex << (int)r.h << " " << (int)r.l << std::endl;
  std::cout << std::hex << r.hl << std::endl;
}
