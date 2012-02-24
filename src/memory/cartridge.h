#ifndef MEMORY_CARTRIDGE_H_
# define MEMORY_CARTRIDGE_H_

# include <iostream>

# include <stdint.h>

# include <memory/as.h>
# include <utils/mapped_file.h>

namespace gbemu { namespace memory {

class cartridge
{
  public:
    cartridge(gbemu::memory::as& as, const gbemu::utils::mapped_file& rom);

  private:
    void register_handlers_romonly(gbemu::memory::as& as);
    void register_handlers_mbc1(gbemu::memory::as& as);
    void register_handlers_mbc2(gbemu::memory::as& as);
    void register_handlers_mbc3(gbemu::memory::as& as);
    void register_handlers_mbc5(gbemu::memory::as& as);

    const gbemu::utils::mapped_file& rom_;
};

}}

#endif /* !MEMORY_CARTRIDGE_H_ */
