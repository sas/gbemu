#include <stdexcept>

#include "cartridge.h"

namespace gbemu { namespace memory {

cartridge::cartridge(gbemu::memory::as& as, const gbemu::utils::mapped_file& rom)
  : rom_(rom)
{
  uint8_t cartridge_type = rom.read<uint8_t>(0x147);

  switch (cartridge_type)
  {
    case 0x00:
      this->register_handlers_romonly(as);
      break;

    case 0x01:
    case 0x02:
    case 0x03:
      this->register_handlers_mbc1(as);
      break;

    case 0x05:
    case 0x06:
      this->register_handlers_mbc2(as);
      break;

    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
      this->register_handlers_mbc3(as);
      break;

    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
      this->register_handlers_mbc5(as);
      break;

    default:
      throw std::out_of_range("unknown MBC type");
  }
}

void cartridge::register_handlers_romonly(gbemu::memory::as& as)
{
  as.add_read_handler({ 0x0000, 0x7fff },
    [&](uint16_t off) -> uint8_t {
      return this->rom_[off];
    }
  );
}

void cartridge::register_handlers_mbc1(gbemu::memory::as& as)
{
  (void) as;
}

void cartridge::register_handlers_mbc2(gbemu::memory::as& as)
{
  (void) as;
}

void cartridge::register_handlers_mbc3(gbemu::memory::as& as)
{
  (void) as;
}

void cartridge::register_handlers_mbc5(gbemu::memory::as& as)
{
  (void) as;
}

}}
