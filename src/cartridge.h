#ifndef CARTRIDGE_H_
# define CARTRIDGE_H_

# include <utils/mapped_file.h>

namespace gbemu {

class cartridge
{
public:
  cartridge(const gbemu::utils::mapped_file& file);

private:
  const gbemu::utils::mapped_file& file_;
};

}

#endif /* !CARTRIDGE_H_ */
