#include <stdexcept>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "mapped_file.h"

namespace gbemu { namespace utils {

mapped_file::mapped_file(const std::string& path)
  : path_(path), is_mapped_(false), begin_(nullptr), size_(0)
{
}

mapped_file::~mapped_file()
{
  if (is_mapped_)
    this->unmap();
}

void mapped_file::map()
{
  int fd;
  struct stat buf;
  char* begin;
  
  if ((fd = open(path_.c_str(), O_RDONLY)) == -1)
    throw std::invalid_argument(path_ + std::string(": ") + std::string(strerror(errno)));

  if (fstat(fd, &buf) == -1)
    throw std::runtime_error(std::string("fstat: ") + std::string(strerror(errno)));

  if ((begin = (char*) mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    throw std::runtime_error(std::string("mmap: ") + std::string(strerror(errno)));

  close(fd);

  is_mapped_ = true;
  begin_ = begin;
  size_ = buf.st_size;
}

void mapped_file::unmap()
{
  if (munmap((void*) begin_, size_) == -1)
    throw std::runtime_error(std::string("munmap: ") + std::string(strerror(errno)));

  is_mapped_ = false;
}

}}
