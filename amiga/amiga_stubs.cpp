// What libstdc++'s <filesystem> asks of the C library and libnix lacks.
// The AmiCradle Team. GPL 3 or later, like Pingus.
#include <cerrno>

struct statvfs;

// libstdc++ declares it itself (C++ linkage) for filesystem::space(),
// which Pingus never calls.
int statvfs(char const*, struct statvfs*)
{
  errno = ENOSYS;
  return -1;
}
