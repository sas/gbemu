#include <iostream>

#include <utils/mapped_file.h>

int main(int argc, char** argv)
{
  if (argc != 2)
  {
    extern char* __progname; /* from crt0.o. */
    std::cerr << "usage: " << __progname << " ROM" << std::endl;
    return 1;
  }

  gbemu::mapped_file f(argv[1]);

  f.map();
}
