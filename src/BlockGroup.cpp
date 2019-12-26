#include "BlockGroup.h"

BlockGroup::BlockGroup(FilesystemImage &image, uint32_t i): image(image) {
        ext2_group_desc group_desc;

        uint64_t offset = i == 0 ? BOOT_SIZE : image.blocks_per_group * image.block_size * i;

        if (offset + sizeof(super) > image.filesystem_size) {
            errors.push_back("Super block out of filesystem");
            return;
        }

        image.istream.seekg(offset, std::ios::beg);
        image.istream.read((char*) &super, sizeof(super));


        // read the super block only for first group
        if (i == 0) {
            image.blocks_count = super.s_blocks_count;
            image.block_size = EXT2_BLOCK_SIZE(&super);
            image.blocks_per_group = super.s_blocks_per_group;
            image.inodes_per_group = super.s_inodes_per_group;

            std::cout << image.block_size << std::endl;
            image.block_usage = std::vector<bool>(image.filesystem_size / image.block_size);
            image.block_usage[0] = true;
        }

        uint64_t group_desc_offset = offset % image.block_size == 0 ? 0 : image.block_size - offset % image.block_size;
        if (group_desc_offset + sizeof(group_desc) > image.filesystem_size) {
            errors.push_back("Group description out of filesystem");
            return;
        }
        image.istream.seekg(group_desc_offset, std::ios_base::cur);
        image.istream.read((char*) &group_desc, sizeof(group_desc));
        // assigning all used by this group blocks (1 super block and 1 group description)
        image.block_usage[i * image.blocks_per_group + (i == 0 && image.block_size == 1024 ? 1 : 0)] = true;
        image.block_usage[i * image.blocks_per_group + (i == 0 && image.block_size == 1024 ? 2 : 1)] = true;


        inode_table_i = group_desc.bg_inode_table;
        inode_table_size = (image.inodes_per_group * sizeof(ext2_inode)) / image.block_size;
        if (inode_table_i + inode_table_size >= image.blocks_count) {
            errors.push_back("Inode-table is in invalid block " + std::to_string(inode_table_i));
        }
        // assigning all blocks used by the inode table;
        for (uint32_t used_block = 0; used_block < inode_table_size; ++used_block) {
            image.block_usage[inode_table_i + used_block] = true;
        }
}

ext2_inode BlockGroup::getInode(uint32_t i) {
    if (!errors.empty()) throw InvalidINode("Tried to access INode " + std::to_string(i) + " in a broken group");

    ext2_inode inode{};
    image.istream.seekg(inode_table_i * image.block_size + sizeof(ext2_inode) * ((i - 1) % image.inodes_per_group));
    image.istream.read((char *) &inode, sizeof(inode));
    return inode;
}
