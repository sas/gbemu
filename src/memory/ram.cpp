#include "ram.h"

namespace gbemu { namespace memory {

ram::ram(gbemu::memory::as& as)
{
  /*
  ** We do not support CGB mode at the moment, so we only have one RAM bank.
  */

  /* create the block used as RAM */
  ram_banks_[0] = std::vector<uint8_t>(0xd000 - 0xc000);

  /* handlers */
  auto rh = [&](uint16_t off) -> uint8_t { return this->ram_banks_[0][off]; };
  auto wh = [&](uint16_t off, uint8_t val) -> void { this->ram_banks_[0][off] = val; };

  /* read/write handlers on bank 0 */
  as.add_read_handler({ 0xc000, 0xcfff }, rh);
  as.add_write_handler({ 0xc000, 0xcfff }, wh);

  /* handlers for the 0xe000 - 0xfdff echo zone */
  as.add_read_handler({ 0xe000, 0xfdff }, rh);
  as.add_write_handler({ 0xe000, 0xfdff }, wh);
}

}}
