#ifndef __FILE_IO_VELOC
#define __FILE_IO_VELOC

#include "GenericFileIO.h"
#include "veloc/cache.hpp"

namespace gio {
class GenericFileIO_VELOC : public GenericFileIO {
    veloc::cached_file_t cached_file;

public:
    GenericFileIO_VELOC();
    ~GenericFileIO_VELOC();
    void open(const std::string &FN, bool ForReading = false, bool MustExist = false);
    void setSize(size_t sz);
    void read(void *buf, size_t count, off_t offset, const std::string &D);
    void write(const void *buf, size_t count, off_t offset, const std::string &D);
    static bool flush();
    static void shutdown();

protected:
    int FH;
};
}

#endif //__FILE_IO_VELOC
