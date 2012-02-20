#include <stdexcept>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "mapped_file.h"

namespace gbemu {

mapped_file::mapped_file(const std::string& path)
  : _path(path), _is_mapped(false), _begin(nullptr), _size(0)
{
}

mapped_file::~mapped_file()
{
  if (_is_mapped)
    unmap();
}

void mapped_file::map()
{
  int fd;
  struct stat buf;
  char* begin;
  
  if ((fd = open(get_path(), O_RDONLY)) == -1)
    throw std::invalid_argument(_path + std::string(": ") + std::string(strerror(errno)));

  if (fstat(fd, &buf) == -1)
    throw std::runtime_error(std::string("fstat: ") + std::string(strerror(errno)));

  if ((begin = (char*) mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED)
    throw std::runtime_error(std::string("mmap: ") + std::string(strerror(errno)));

  close(fd);

  _is_mapped = true;
  _begin = begin;
  _size = buf.st_size;
}

void mapped_file::unmap()
{
  if (munmap((void*) _begin, _size) == -1)
    throw std::runtime_error(std::string("munmap: ") + std::string(strerror(errno)));

  _is_mapped = false;
}

const char* mapped_file::get_path() const
{
  return _path.c_str();
}

}
