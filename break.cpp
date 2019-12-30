#include <linux/fs.h>
#include <iostream>
#include <fstream>

#include "Filesystem.h"
#include "fstream64.h"


int main() {
    Filesystem fs{"../file"};
    std::cout << fs.fileTreeString() << std::endl;
    INode& example = fs.inodes[std::get<1>(fs.inodes[2].children[4])];

    INode& user = fs.inodes[std::get<1>(fs.inodes[2].children[3])];

//    for (size_t i = 0; i < fs.image.block_usage.size(); ++i)
//        std::cout << fs.image.block_usage[i];
//    std::cout << std::endl;

//    std::cout << fs.getAllErrors() << std::endl;

//    uint32_t inode_i = example.inode_i;
//    ext2_inode inode = example.inode;

    uint32_t inode_i = user.inode_i;
    ext2_inode inode = user.inode;

    uint32_t block_size = fs.image.block_size;
    uint32_t group_i = (inode_i - 1) / fs.image.inodes_per_group;
    uint32_t offset = fs.block_groups[group_i].inode_table_i * block_size + (inode_i - 1) % fs.image.inodes_per_group * sizeof(ext2_inode);

    inode.i_size = 0; // 2048
//    inode.i_block[0] = 0;
//    inode.i_block[1] = 1;

    fstream64 ostream{"../file3", fstream64::WRITE};
//    char* buffer = new char[offset];
//    fs.image.istream.seekg(0);
//    fs.image.istream.read(buffer, offset);
//    ostream.write(buffer, offset);
//    delete[] buffer;

    ostream.seek(offset);
    ostream.write((char*) &inode, sizeof(inode));

//    fs.image.istream.seekg(0, std::ios::end);
//    uint32_t size = fs.image.istream.tellg();
//    offset += sizeof(ext2_inode);
//    buffer = new char[size - offset];
//    fs.image.istream.seekg(offset);
//    fs.image.istream.read(buffer, size - offset);
//    ostream.write(buffer, size - offset);
//    delete[] buffer;

    std::cout << "Successfully broke your filesystem!)" << std::endl;

    return 0;
}