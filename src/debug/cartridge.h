#ifndef DEBUG_CARTRIDGE_H_
# define DEBUG_CARTRIDGE_H_

# include <iostream>

# include <stdint.h>

# include <utils/mapped_file.h>

namespace gbemu { namespace debug {

struct cartridge
{
  cartridge(const gbemu::utils::mapped_file& file);
  void dump(std::ostream& out) const;

  std::string title;
  uint8_t cgb_flag;
  uint8_t sgb_flag;
  uint8_t cartridge_type;
  uint8_t rom_size;
  uint8_t ram_size;
  uint8_t destination_code;
  uint8_t header_checksum;
  uint16_t global_checksum;
};

}}

#endif /* !DEBUG_CARTRIDGE_H_ */
