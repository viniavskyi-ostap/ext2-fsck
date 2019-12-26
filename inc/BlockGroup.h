#ifndef EXT2_FSCK_BLOCKGROUP_H
#define EXT2_FSCK_BLOCKGROUP_H

#include <vector>
#include <cmath>
#include <fstream>
#include <ext2fs/ext2_fs.h>
#include "FilesystemImage.h"
#include "FilesystemErrors.h"
#include <iostream>

class BlockGroup {
public:
    FilesystemImage& image;

    uint32_t inode_table_i;
    uint32_t inode_table_size;

    std::vector<std::string> errors;

public:
    BlockGroup(FilesystemImage &image, uint32_t i);

    ext2_inode getInode(uint32_t i);
};

#endif //EXT2_FSCK_BLOCKGROUP_H
