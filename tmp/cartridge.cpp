namespace { namespace to_str {
  static std::map<uint8_t, std::string> cgb_flag = {
    { 0x00, "CGB disabled" },
    { 0x80, "CGB support" },
    { 0xc0, "CBG only" },
  };

  static std::map<uint8_t, std::string> sgb_flag = {
    { 0x00, "SGB disabled" },
    { 0x03, "SGB support" },
  };

  static std::map<uint8_t, std::string> cartridge_type = {
    { 0x00, "ROM ONLY" },               { 0x13, "MBC3+RAM+BATTERY" },
    { 0x01, "MBC1" },                   { 0x15, "MBC4" },
    { 0x02, "MBC1+RAM" },               { 0x16, "MBC4+RAM" },
    { 0x03, "MBC1+RAM+BATTERY" },       { 0x17, "MBC4+RAM+BATTERY" },
    { 0x05, "MBC2" },                   { 0x19, "MBC5" },
    { 0x06, "MBC2+BATTERY" },           { 0x1A, "MBC5+RAM" },
    { 0x08, "ROM+RAM" },                { 0x1B, "MBC5+RAM+BATTERY" },
    { 0x09, "ROM+RAM+BATTERY" },        { 0x1C, "MBC5+RUMBLE" },
    { 0x0B, "MMM01" },                  { 0x1D, "MBC5+RUMBLE+RAM" },
    { 0x0C, "MMM01+RAM" },              { 0x1E, "MBC5+RUMBLE+RAM+BATTERY" },
    { 0x0D, "MMM01+RAM+BATTERY" },      { 0xFC, "POCKET CAMERA" },
    { 0x0F, "MBC3+TIMER+BATTERY" },     { 0xFD, "BANDAI TAMA5" },
    { 0x10, "MBC3+TIMER+RAM+BATTERY" }, { 0xFE, "HuC3" },
    { 0x11, "MBC3" },                   { 0xFF, "HuC1+RAM+BATTERY" },
    { 0x12, "MBC3+RAM" },
  };

  static std::map<uint8_t, std::string> rom_size = {
    { 0x00, "32KByte (no ROM banking)" },
    { 0x01, "64KByte (4 banks)" },
    { 0x02, "128KByte (8 banks)" },
    { 0x03, "256KByte (16 banks)" },
    { 0x04, "512KByte (32 banks)" },
    { 0x05, "1MByte (64 banks)  - only 63 banks used by MBC1" },
    { 0x06, "2MByte (128 banks) - only 125 banks used by MBC1" },
    { 0x07, "4MByte (256 banks)" },
    { 0x52, "1.1MByte (72 banks)" },
    { 0x53, "1.2MByte (80 banks)" },
    { 0x54, "1.5MByte (96 banks)" },
  };

  static std::map<uint8_t, std::string> ram_size = {
    { 0x00, "None" },
    { 0x01, "2 KBytes" },
    { 0x02, "8 Kbytes" },
    { 0x03, "32 KBytes (4 banks of 8KBytes each)" },
  };

  static std::map<uint8_t, std::string> destination_code = {
    { 0x00, "Japanese" },
    { 0x01, "Non-Japanese" },
  };
}}

void cartridge::dump(std::ostream& out) const
{
#define USE_TOSTR(Field) to_str::Field[Field]

  out << "Title: " << title << std::endl;

  out << "CGB flag: " << USE_TOSTR(cgb_flag) << std::endl;
  out << "SGB flag: " << USE_TOSTR(sgb_flag) << std::endl;
  
  out << "Cartridge type: " << USE_TOSTR(cartridge_type) << std::endl;
  out << "ROM size: " << USE_TOSTR(rom_size) << std::endl;
  out << "RAM size: " << USE_TOSTR(ram_size) << std::endl;
  out << "Destination code: " << USE_TOSTR(destination_code) << std::endl;
  
  out << std::hex;
  out << "Header checksum: " << (int) header_checksum << std::endl;
  out << "Global checksum: " << global_checksum << std::endl;
  out << std::dec;

#undef USE_TOSTR
}

ctor()
{
  title = file.read<char[0x143 - 0x134]>(0x134);

  cgb_flag = file.read<uint8_t>(0x143);
  if (cgb_flag != 0x80 && cgb_flag != 0xc0)
    cgb_flag = 0x00;

  sgb_flag = file.read<uint8_t>(0x146);
  if (sgb_flag != 0x03)
    sgb_flag = 0x00;

  cartridge_type = file.read<uint8_t>(0x147);
  rom_size = file.read<uint8_t>(0x148);
  ram_size = file.read<uint8_t>(0x149);
  destination_code = file.read<uint8_t>(0x14a);

  header_checksum = file.read<uint8_t>(0x014d);
  global_checksum = file.read<uint16_t>(0x14e);
}

{
  std::string title;
  uint8_t cgb_flag;
  uint8_t sgb_flag;
  uint8_t cartridge_type;
  uint8_t rom_size;
  uint8_t ram_size;
  uint8_t destination_code;
  uint8_t header_checksum;
  uint16_t global_checksum;
}
