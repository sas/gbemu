#ifndef MEMORY_CARTRIDGE_H_
# define MEMORY_CARTRIDGE_H_

# include <iostream>
# include <unordered_map>
# include <vector>

# include <stdint.h>

# include <memory/as.h>
# include <utils/mapped_file.h>

namespace gbemu { namespace memory {

class cartridge
{
  public:
    cartridge(memory::as& as, const utils::mapped_file& rom);

  private:
    void init_romonly(memory::as& as);
    void init_mbc1(memory::as& as);
    void init_mbc2(memory::as& as);
    void init_mbc3(memory::as& as);
    void init_mbc5(memory::as& as);

    enum class mode_select { rom, ram } mode_select_;

    const utils::mapped_file& rom_;
    uint16_t rom_bank_;

    std::unordered_map<int, std::vector<uint8_t>> ram_;
    uint8_t ram_bank_;
    bool ram_enabled_;
};

}}

#endif /* !MEMORY_CARTRIDGE_H_ */
