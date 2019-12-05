#ifndef EXT2_FSCK_BLOCKGROUP_H
#define EXT2_FSCK_BLOCKGROUP_H

#include <fstream>
#include <ext2fs/ext2_fs.h>

class BlockGroup {
    ext2_super_block super;
    ext2_group_desc group_desc;

    BlockGroup(std::ifstream &input, size_t offset=0) {

        input.seekg(offset);
        input.read((char*) &super, sizeof(super));

        uint64_t block_size = EXT2_BLOCK_SIZE(&super);
        uint32_t blocks_in_group = super.s_blocks_per_group;
        uint32_t inodes_in_group = super.s_inodes_per_group;

        input.seekg(offset % block_size == 0 ? 0 : block_size - offset % block_size, std::ios_base::cur);
        input.read((char*) &group_desc, sizeof(group_desc));

        group_desc.bg_inode_table;
    }

    // read description
    // read inode table
};

#endif //EXT2_FSCK_BLOCKGROUP_H
