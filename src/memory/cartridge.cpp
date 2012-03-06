#include <stdexcept>

#include "cartridge.h"

namespace gbemu { namespace memory {

cartridge::cartridge(memory::as& as, const utils::mapped_file& rom)
  : mode_select_(mode_select::rom), rom_(rom), rom_bank_(1), ram_bank_(0), ram_enabled_(false)
{
  uint8_t cartridge_type = rom.read<uint8_t>(0x147);

  switch (cartridge_type)
  {
    case 0x00:
      this->init_romonly(as);
      break;

    case 0x01:
    case 0x02:
    case 0x03:
      this->init_mbc1(as);
      break;

    case 0x05:
    case 0x06:
      this->init_mbc2(as);
      break;

    case 0x0f:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
      this->init_mbc3(as);
      break;

    case 0x19:
    case 0x1a:
    case 0x1b:
    case 0x1c:
    case 0x1d:
    case 0x1e:
      this->init_mbc5(as);
      break;

    default:
      throw std::out_of_range("unknown MBC type");
  }

  /*
  ** Two memory regions of ROM access. The first one is directly mapped to the
  ** first 16KB of the cartridge, and the second one is a switchable memory
  ** region (depending on the MBC that is used) that can access multiple
  ** different 16KB zones.
  ** These two zones are actually common to all MBC types.
  */
  as.add_read_handler({ 0x0000, 0x3fff },
    [&](uint16_t off) -> uint8_t {
      return this->rom_[off];
    }
  );
  as.add_read_handler({ 0x4000, 0x7ffff },
    [&](uint16_t off) -> uint8_t {
      return this->rom_[this->rom_bank_ * 0x4000 + off];
    }
  );
}

void cartridge::init_romonly(memory::as&)
{
  /*
  ** This makes sure that the first 32KB of the address space are mapped to the
  ** first 32KB of the ROM.
  */
  this->rom_bank_ = 1;
}

void cartridge::init_mbc1(memory::as& as)
{
  /*
  ** Create the external RAM banks that this MBC supports.
  */
  for (int i = 0; i < 4; ++i)
    ram_[i] = std::vector<uint8_t>(0x2000);

  /*
  ** This MBC can contain some external RAM. These two handler are here to
  ** access this RAM.
  */
  as.add_read_handler({ 0xa000, 0xbfff },
    [&](uint16_t off) -> uint8_t {
      if (!this->ram_enabled_)
        return 0xff; /* XXX: log */

      return this->ram_[this->ram_bank_][off];
    }
  );
  as.add_write_handler({ 0xa000, 0xbfff },
    [&](uint16_t off, uint8_t val) -> void {
      if (!this->ram_enabled_)
        return; /* XXX: log */

      this->ram_[this->ram_bank_][off] = val;
    }
  );

  /*
  ** This memory area control the enabling of the external ram. By default, RAM
  ** is disabled. A write with the lower four bits set to 0xa will enable RAM.
  ** Any other value will disable it.
  */
  as.add_write_handler({ 0x0000, 0x1fff },
    [&](uint16_t, uint8_t val) -> void {
      this->ram_enabled_ = ((val & 0x0f) == 0xa);
    }
  );

  /*
  ** A write to this memory region selects the lower 5 bits of the memory ROM
  ** bank number.
  */
  as.add_write_handler({ 0x2000, 0x3fff },
    [&](uint16_t, uint8_t val) -> void {
      this->rom_bank_ = (this->rom_bank_ & ~0x1f) | (val & 0x1f);

      /* If software is trying to access bank 0, we redirect to bank 1. */
      if (this->rom_bank_ == 0x00)
        this->rom_bank_ = 0x01;
    }
  );

  /*
  ** This memory region will select the lower bits of the ROM bank number or the
  ** RAM bank number, depending on the following register.
  */
  as.add_write_handler({ 0x4000, 0x5fff },
    [&](uint16_t, uint8_t val) -> void {
      if (this->mode_select_ == mode_select::rom)
        this->rom_bank_ = (this->rom_bank_ & 0x1f) | ((val & 0x3) << 5);
      else if (this->mode_select_ == mode_select::ram)
        this->ram_bank_ = val & 0x3;
    }
  );

  /*
  ** ROM/RAM mode selector.
  */
  as.add_write_handler({ 0x6000, 0x7fff },
    [&](uint16_t, uint8_t val) -> void {
      if (val & 0x01)
        this->mode_select_ = mode_select::ram;
      else
        this->mode_select_ = mode_select::rom;
    }
  );
}

void cartridge::init_mbc2(memory::as& as)
{
  /*
  ** Create the built-in RAM block that this MBC suports.
  */
  ram_[0] = std::vector<uint8_t>(512);

  /*
  ** This MBC can contain some built-in internal RAM (in the MBC chip itself).
  ** This RAM consists of 512 4-bit values.
  */
  as.add_read_handler({ 0xa000, 0xa1ff },
    [&](uint16_t off) -> uint8_t {
      if (!this->ram_enabled_)
        return 0xff; /* XXX: log */

      return this->ram_[0][off];
    }
  );
  as.add_write_handler({ 0xa000, 0xa1ff },
    [&](uint16_t off, uint8_t val) -> void {
      if (!this->ram_enabled_)
        return; /* XXX: log */

      this->ram_[0][off] = val & 0x0f;
    }
  );

  /*
  ** This memory area control the enabling of the external ram. By default, RAM
  ** is disabled. To enable ram, software has to write in an address in this
  ** range with the least significant bit of the most significant byte beeing
  ** zero.
  ** XXX: Not sure if the value matters at all, for now we assume the value has
  ** the same meaning as for MBC1.
  */
  as.add_write_handler({ 0x0000, 0x1fff },
    [&](uint16_t off, uint8_t val) -> void {
      if (off & 0x0100)
        return; /* XXX: log */

      this->ram_enabled_ = ((val & 0x0f) == 0xa);
    }
  );

  /*
  ** ROM bank selector. Here, the least significant bit of the most significant
  ** byte of the address must be set for the action to take effect.
  */
  as.add_write_handler({ 0x2000, 0x3fff },
    [&](uint16_t off, uint8_t val) -> void {
      if (!(off & 0x0100))
        return; /* XXX: log */

      this->rom_bank_ = val & 0x0f;

      if (this->rom_bank_ == 0x00)
        this->rom_bank_ = 0x01;
    }
  );
}

void cartridge::init_mbc3(memory::as& as)
{
  /*
  ** Create the external RAM banks that this MBC supports.
  */
  for (int i = 0; i < 4; ++i)
    ram_[i] = std::vector<uint8_t>(0x2000);

  /*
  ** This MBC can contain some external RAM and an RTC clock. The following
  ** memory regions is used to access both devices. When the bank number is in
  ** 0x00-0x03, the four available RAM pages can be mapped. When it is in
  ** 0x08-0x0c, the registers of the RTC are mapped.
  */
  as.add_read_handler({ 0xa000, 0xbfff },
    [&](uint16_t off) -> uint8_t {
      if (!this->ram_enabled_)
        return 0xff; /* XXX: log */

      if (this->ram_bank_ <= 0x03)
        return this->ram_[this->ram_bank_][off];
      else if (0x08 <= this->ram_bank_ && this->ram_bank_ <= 0x0c)
      {
        std::cerr << "Trying to access RTC registers, not implemented." << std::endl;
        return 0xff;
      }
      else
        return 0xff; /* XXX: log */
    }
  );
  as.add_write_handler({ 0xa000, 0xbfff },
    [&](uint16_t off, uint8_t val) -> void {
      if (!this->ram_enabled_)
        return; /* XXX: log */

      if (this->ram_bank_ <= 0x03)
        this->ram_[this->ram_bank_][off] = val;
      else if (0x08 <= this->ram_bank_ && this->ram_bank_ <= 0x0c)
        std::cerr << "Trying to access RTC registers, not implemented." << std::endl;
      else
        return; /* XXX: log */
    }
  );

  /*
  ** Same thing that in MBC1: this memory region is used to enable/disable the
  ** RAM/RTC region.
  */
  as.add_write_handler({ 0x0000, 0x1fff },
    [&](uint16_t, uint8_t val) -> void {
      this->ram_enabled_ = ((val & 0x0f) == 0xa);
    }
  );

  /*
  ** A write to this memory region sets the 7 bits of the ROM bank selector.
  */
  as.add_write_handler({ 0x2000, 0x3fff },
    [&](uint16_t, uint8_t val) -> void {
      this->rom_bank_ = val & 0x8f;

      /* If software is trying to access bank 0, we redirect to bank 1. */
      if (this->rom_bank_ == 0x00)
        this->rom_bank_ = 0x01;
    }
  );

  /*
  ** RAM bank number selector.
  */
  as.add_write_handler({ 0x4000, 0x5fff },
    [&](uint16_t, uint8_t val) -> void {
      this->ram_bank_ = val;
    }
  );

  /*
  ** A write of 0x00 followed by a write of 0x01 in this region will update the
  ** RTC registers with the current value of time.
  */
  as.add_write_handler({ 0x6000, 0x7fff },
    [&](uint16_t, uint8_t) -> void {
      std::cerr << "Trying to access the Latch Clock Data region, not implemented." << std::endl;
    }
  );
}

void cartridge::init_mbc5(memory::as& as)
{
  /*
  ** Create the external RAM banks that this MBC supports.
  */
  for (int i = 0; i < 16; ++i)
    ram_[i] = std::vector<uint8_t>(0x2000);

  /*
  ** This MBC can contain some external RAM. These two handler are here to
  ** access this RAM.
  */
  as.add_read_handler({ 0xa000, 0xbfff },
    [&](uint16_t off) -> uint8_t {
      if (!this->ram_enabled_)
        return 0xff; /* XXX: log */

      return this->ram_[this->ram_bank_][off];
    }
  );
  as.add_write_handler({ 0xa000, 0xbfff },
    [&](uint16_t off, uint8_t val) -> void {
      if (!this->ram_enabled_)
        return; /* XXX: log */

      this->ram_[this->ram_bank_][off] = val;
    }
  );

  /*
  ** This memory area control the enabling of the external ram. By default, RAM
  ** is disabled. A write with the lower four bits set to 0xa will enable RAM.
  ** Any other value will disable it.
  */
  as.add_write_handler({ 0x0000, 0x1fff },
    [&](uint16_t, uint8_t val) -> void {
      this->ram_enabled_ = ((val & 0x0f) == 0xa);
    }
  );

  /*
  ** This memory region is used to set the low 8 bits of the ROM bank number.
  */
  as.add_write_handler({ 0x2000, 0x2fff },
    [&](uint16_t, uint8_t val) -> void {
      this->rom_bank_ = (this->rom_bank_ & 0xff00) | val;
    }
  );

  /*
  ** This memory region is used to set the low 9th bit of the ROM bank number.
  */
  as.add_write_handler({ 0x3000, 0x3fff },
    [&](uint16_t, uint8_t val) -> void {
      this->rom_bank_ = (this->rom_bank_ & 0x00ff) | ((val & 1) << 8);
    }
  );

  /*
  ** RAM bank number selector.
  */
  as.add_write_handler({ 0x4000, 0x5fff },
    [&](uint16_t, uint8_t val) -> void {
      this->ram_bank_ = val & 0x0f;
    }
  );
}

}}
