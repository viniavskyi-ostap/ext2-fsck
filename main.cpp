#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <iostream>

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cmath>

#include "Filesystem.h"

void read_to_buffer(int file, char* buffer, size_t buffer_size) {
    size_t total_number_read = 0;
    ssize_t number_read = 0;

    while (number_read < buffer_size) {
        number_read = read(file, &buffer[total_number_read], buffer_size - total_number_read);
        if (number_read < 0) {
            if (errno != EINTR) throw std::runtime_error("Cannot read from given file!");
        } else if (!number_read) {
            break;
        } else {
            total_number_read += number_read;
        }
    }
};

int main() {
    std::ifstream input{"../file", std::ios::binary};
    ext2_super_block super;

    input.seekg(1024);
    input.read((char*) &super, sizeof(super));

    uint64_t block_size = std::pow(2, super.s_log_block_size + 10);
    uint32_t blocks_in_group = super.s_blocks_per_group;
    uint32_t inodes_in_group = super.s_inodes_per_group;
    std::cout << "Block size: " << block_size << std::endl;
    std::cout << "Blocks in group: " << blocks_in_group << std::endl;
    std::cout << "Inodes in group: " << inodes_in_group << std::endl;

    ext2_group_desc group_description;
    input.read((char*) &group_description, sizeof(group_description));

    group_description.bg_inode_table;

    std::cout << super.s_inodes_count << std::endl;
    std::cout << super.s_blocks_count << std::endl;
    std::cout << super.s_free_blocks_count << std::endl;
    std::cout << super.s_first_data_block << std::endl;

    std::cout << group_description.bg_inode_table << std::endl;

    return 0;
}