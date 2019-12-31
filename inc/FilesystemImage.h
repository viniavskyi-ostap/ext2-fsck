#ifndef EXT2_FSCK_FILESYSTEMIMAGE_H
#define EXT2_FSCK_FILESYSTEMIMAGE_H

#include <fstream>
#include "biofs.h"

using fs_t = off64_t;

inline fs_t fs_ceil_division(fs_t num, fs_t denom) {
    return (num + denom - 1) / denom;
}

inline fs_t fs_align(fs_t num, fs_t alignment) {
    return fs_ceil_division(num, alignment) * alignment;
}

const fs_t BOOT_SIZE = 1024;

struct FilesystemImage {
    biofs istream;

    fs_t filesystem_size;

    fs_t blocks_count;
    fs_t block_size;
    fs_t blocks_per_group;
    fs_t inode_size;
    fs_t inodes_per_group;
    fs_t reserved_group_description_blocks;

    std::vector<bool> block_usage;
    std::vector<bool> inode_usage;
};

#endif //EXT2_FSCK_FILESYSTEMIMAGE_H
