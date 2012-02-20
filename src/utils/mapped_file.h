#ifndef UTILS_MAPPED_FILE_H_
# define UTILS_MAPPED_FILE_H_

# include <stdexcept>
# include <string>

# include <stdint.h>

namespace gbemu { namespace utils {

class mapped_file
{
public:
  mapped_file(const std::string& path);
  ~mapped_file();

  void map();
  void unmap();
  const char* get_path() const;

  template<typename T>
  const T& read(uint64_t offset) const
  {
    if (offset >= this->_size)
      throw std::out_of_range(this->_path);

    return *((T*) (this->_begin + offset));
  }

  char operator[](uint64_t offset) const
  {
    return this->read<char>(offset);
  }

private:
  const std::string&  _path;
  bool                _is_mapped;
  const char*         _begin;
  uint64_t            _size;
};

}}

#endif /* !UTILS_MAPPED_FILE_H_ */
