#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

class fstream64 {
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
    explicit fstream64(const std::string path, uint flags = 0);
    ~fstream64();

    off64_t size();

    void seek(off64_t offset, seek_t seek_type = seek_t::beg);
    void read(char *buffer, off64_t size);
    void write(char *buffer, off64_t size);
};
