#ifndef EXT2_FSCK_FILESYSTEMIMAGE_H
#define EXT2_FSCK_FILESYSTEMIMAGE_H

#include <fstream>

const uint64_t BOOT_SIZE = 1024;

struct FilesystemImage {
    std::ifstream istream;

    uint64_t blocks_count;
    uint64_t block_size;
    uint64_t blocks_per_group;
    uint32_t inodes_per_group;
};

#endif //EXT2_FSCK_FILESYSTEMIMAGE_H
