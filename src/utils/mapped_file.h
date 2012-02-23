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

  template<typename T>
  const T& read(uint64_t offset) const
  {
    if (offset >= this->size_)
      throw std::out_of_range(path_);

    return *((T*) (begin_ + offset));
  }

  uint8_t operator[](uint64_t offset) const
  {
    return this->read<uint8_t>(offset);
  }

  const std::string& getpath_() const { return path_; }
  uint64_t getsize_() const { return size_; }

private:
  std::string path_;
  bool        is_mapped_;
  const char* begin_;
  uint64_t    size_;
};

}}

#endif /* !UTILS_MAPPED_FILE_H_ */
