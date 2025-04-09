#include "FileIOVELOC.hpp"

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#define __DEBUG
#include "debug.hpp"

static std::string get_veloc_cfg() {
    char *ptr = getenv("GENERICIO_USE_VELOC");
    if (ptr == NULL)
	return "";
    return ptr;
}

namespace gio {

GenericFileIO_VELOC::GenericFileIO_VELOC() : cached_file(get_veloc_cfg()) {
}

GenericFileIO_VELOC::~GenericFileIO_VELOC() { }

void GenericFileIO_VELOC::open(const std::string &FN, bool ForReading, bool MustExist) {
    int flags = ForReading ? O_RDONLY : (O_WRONLY | (!MustExist ? O_CREAT : 0));
    int mode = S_IRUSR | S_IWUSR | S_IRGRP;
    FileName = FN;
    cached_file.open(FN, flags, mode);
}

void GenericFileIO_VELOC::setSize(size_t sz) { }

void GenericFileIO_VELOC::read(void *buf, size_t count, off_t offset, const std::string &D) {
    if (!cached_file.pread(buf, count, offset))
      throw std::runtime_error("Unable to read " + D + " from file: " + FileName + ": " + strerror(errno));
}

void GenericFileIO_VELOC::write(const void *buf, size_t count, off_t offset, const std::string &D) {
    if (!cached_file.pwrite(buf, count, offset))
	throw std::runtime_error("Unable to write " + D + " to file: " + FileName + ": " + strerror(errno));
}

bool GenericFileIO_VELOC::flush() {
    return veloc::cached_file_t::flush();
}

void GenericFileIO_VELOC::shutdown() {
    veloc::cached_file_t::shutdown();
}

}
