#include <linux/fs.h>
#include <iostream>
#include <fstream>

#include "Filesystem.h"

int main() {
    Filesystem fs{"../file-broken"};
    std::cout << fs.fileTreeString() << std::endl;
    INode& home = fs.inodes[std::get<1>(fs.inodes[2].children[3])];
    INode& user = fs.inodes[std::get<1>(home.children[2])];
    INode& hello = fs.inodes[std::get<1>(user.children[2])];

    for (size_t i = 0; i < fs.image.block_usage.size(); ++i)
        std::cout << fs.image.block_usage[i];
    std::cout << std::endl;

    std::cout << fs.getAllErrors() << std::endl;

    uint32_t inode_i = user.inode_i;
    ext2_inode& inode = user.inode;

    uint32_t block_size = fs.image.block_size;
    uint32_t group_i = (inode_i - 1) / fs.image.inodes_per_group;
    uint32_t offset = fs.block_groups[group_i].inode_table_i * block_size + (inode_i - 1) % fs.image.inodes_per_group * sizeof(ext2_inode);

    inode.i_size = 2048;
    inode.i_block[0] = 0;
    inode.i_block[1] = 1;

    std::fstream ostream{"../file-broken", std::ios::binary};
    ostream.seekp(offset, std::ios::beg);
    ostream.write((char *) &inode, sizeof(ext2_inode));
    ostream.flush();
    ostream.close();

    std::cout << "Successfully broke your filesystem!)" << std::endl;

    return 0;
}