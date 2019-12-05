#ifndef EXT2_FSCK_BLOCKGROUP_H
#define EXT2_FSCK_BLOCKGROUP_H

#include <fstream>
#include <ext2fs/ext2_fs.h>
#include "FilesystemImage.h"


class BlockGroup {
    FilesystemImage& image;

    ext2_super_block super;
    ext2_group_desc group_desc;
    uint32_t inode_table_i;

public:
    BlockGroup(FilesystemImage &image, uint32_t i): image(image) {
        uint64_t offset = i == 0 ? BOOT_SIZE : image.blocks_per_group * image.block_size * i;

        image.istream.seekg(offset);
        image.istream.read((char*) &super, sizeof(super));

        // read the super block only for first group
        if (i == 0) {
            image.blocks_count = super.s_blocks_count;
            image.block_size = EXT2_BLOCK_SIZE(&super);
            image.blocks_per_group = super.s_blocks_per_group;
            image.inodes_per_group = super.s_inodes_per_group;
        }

        uint64_t group_desc_offset = offset % image.block_size == 0 ? 0 : image.block_size - offset % image.block_size;
        image.istream.seekg(group_desc_offset, std::ios_base::cur);
        image.istream.read((char*) &group_desc, sizeof(group_desc));

        inode_table_i = group_desc.bg_inode_table;
    }

    ext2_inode getInode(uint32_t i) {
        ext2_inode inode{};
        image.istream.seekg(inode_table_i * image.block_size + sizeof(ext2_inode) * ((i - 1) % image.inodes_per_group));
        image.istream.read((char *) &inode, sizeof(inode));
        return inode;
    }
};

#endif //EXT2_FSCK_BLOCKGROUP_H
