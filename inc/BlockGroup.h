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

    fs_t block_group_i;
    fs_t inode_table_i;
    fs_t inode_table_size;
    fs_t block_bitmap_i;
    fs_t inode_bitmap_i;

    std::vector<std::string> errors;

public:
    BlockGroup(FilesystemImage &image, fs_t i);

    ext2_inode getInode(fs_t i);
    void additionalFieldsCheck();
};

#endif //EXT2_FSCK_BLOCKGROUP_H
