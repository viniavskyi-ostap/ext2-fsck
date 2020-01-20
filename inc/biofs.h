#ifndef EXT2_FSCK_BIOFS_H
#define EXT2_FSCK_BIOFS_H

#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

// Binary input output stream

class biofs {
public:
    enum seek_t {
        beg = SEEK_SET,
        cur = SEEK_CUR,
        end = SEEK_END
    };

    enum flags {
        READ = O_RDONLY,
        WRITE = O_WRONLY,
        READ_WRITE = O_RDWR,
        APPEND = O_APPEND,      // requires write
        CREATE = O_CREAT,       // create if not exists
        EXISTS_CLEAR = O_TRUNC, // clear the file if it exists
        EXISTS_THROW = O_EXCL   // do not open if exists
    };

private:
    int file;
    off64_t file_size;

public:
    biofs(biofs& o) = delete;
    biofs(biofs&& o) {
        operator=(std::move(o));
    }
    biofs& operator=(biofs& o) = delete;
    biofs& operator=(biofs&& o) noexcept {
        file = o.file;
        file_size = o.file_size;
        o.file = o.file_size = 0;
        return *this;
    }

    biofs(): file(0), file_size(0) {};
    explicit biofs(std::string path, uint flags = 0);
    ~biofs();

    off64_t size();

    void seek(off64_t offset, seek_t seek_type = seek_t::beg);
    void read(char *buffer, off64_t size);
    void write(char *buffer, off64_t size);

    template<typename T>
    biofs& operator>>(T& data) {
        read((char *) &data, sizeof(data));
        return *this;
    }

    template<typename T>
    biofs& operator>>(T*& data) = delete;

    template<typename T>
    biofs& operator<<(T& data) {
        write((char *) &data, sizeof(data));
        return *this;
    }

    template<typename T>
    biofs& operator<<(T*& data) = delete;
};

#endif