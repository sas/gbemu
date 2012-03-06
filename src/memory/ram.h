#ifndef MEMORY_RAM_H_
# define MEMORY_RAM_H_

# include <unordered_map>
# include <vector>

# include <stdint.h>

# include <memory/as.h>

namespace gbemu { namespace memory {

class ram
{
  public:
    ram(gbemu::memory::as& as);

  private:
    std::unordered_map<int, std::vector<uint8_t>> ram_banks_;
    std::vector<uint8_t> high_ram_;
};

}}

#endif /* !MEMORY_RAM_H_ */
