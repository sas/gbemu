#include <iostream>

#include "cartridge.h"

namespace gbemu {

cartridge::cartridge(const gbemu::utils::mapped_file& file)
  : file_(file)
{
  std::cout << std::hex << file_.read<char[16]>(0x134) << std::endl;
}

}
