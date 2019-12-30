#ifndef EXT2_FSCK_BLOCKGROUP_H
#define EXT2_FSCK_BLOCKGROUP_H

#include <vector>
#include <cmath>
#include <fstream>
#include <ext2fs/ext2fs.h>
#include "FilesystemImage.h"
#include "FilesystemErrors.h"
#include <iostream>

class BlockGroup {
public:
    FilesystemImage& image;

    uint32_t block_group_i;
    uint32_t inode_table_i;
    uint32_t inode_table_size;
    uint32_t block_bitmap_i;
    uint32_t inode_bitmap_i;

    std::vector<std::string> errors;

public:
    BlockGroup(FilesystemImage &image, uint32_t i);

    ext2_inode getInode(uint32_t i);
    void additionalFieldsCheck();
};

#endif //EXT2_FSCK_BLOCKGROUP_H
