#include "fstream64.h"
#include <stdexcept>


fstream64::fstream64(const std::string path, uint flags) {
    // RDONLY, WRONLY, RDWR, O_CREAT, O_TRUNC
    flags |= O_LARGEFILE;
    mode_t mode = 0; // S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    errno = 0;
    while ((file = open(path.c_str(), flags, mode)) < 0) {
        if (errno != EINTR) throw std::runtime_error("Cannot open file " + path);
        errno = 0;
    }

    struct stat64 stat_result{};
    while (fstat64(file, &stat_result) < 0) {
        if (errno != EINTR) throw std::runtime_error("Cannot read file " + path + " stat");
        errno = 0;
    }

    file_size = stat_result.st_size;
}

fstream64::~fstream64() {
    close(file);
}

off64_t fstream64::size() {
    return file_size;
}

void fstream64::seek(off64_t offset, seek_t seek_type) {
    errno = 0;
    while ((lseek64(file, offset, seek_type)) < 0) {
        if (errno != EINTR) throw std::runtime_error("Cannot perform seek operation");
        errno = 0;
    }
}

void fstream64::read(char *buffer, off64_t size) {
    off64_t read_size = 0;
    while (read_size < size) {
        errno = 0;
        off64_t result = ::read(file, buffer + read_size, size - read_size);
        if (result < 0 && errno != EINTR) throw std::runtime_error("Could not read from file");
        if (!result) throw std::runtime_error("Reached the end of file");
        read_size += result;
    }
}

void fstream64::write(char *buffer, off64_t size) {
    off64_t written_size = 0;
    while (written_size < size) {
        errno = 0;
        off64_t result = ::write(file, buffer + written_size, size - written_size);
        if (result < 0 && errno != EINTR) throw std::runtime_error("Could not write to file");
        written_size += result;
    }
}
