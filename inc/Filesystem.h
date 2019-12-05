#ifndef EXT2_FSCK_FIESYSTEM_H
#define EXT2_FSCK_FIESYSTEM_H

#include <ext2fs/ext2_fs.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <iostream>
#include "BlockGroup.h"
#include "FilesystemImage.h"
#include "File.h"

class Filesystem {
public:
    FilesystemImage image{};
    std::vector<BlockGroup> block_groups;

    ext2_inode getInode(uint32_t i) {
        return block_groups[i / image.inodes_per_group].getInode(i);
    }

public:
    Filesystem(std::string path) {
        // TODO check that the number of blocks is correct
          image.istream = std::ifstream{path, std::ios::binary};
        //  image.istream.seekg(0, std::ios::end);
        //  uint64_t file_size = image.istream.tellg();

       block_groups.emplace_back(image, 0);
       uint64_t groups_count = std::ceil((double) image.blocks_count / image.blocks_per_group);
//       for (uint64_t i = 1; i < groups_count; ++i) {
//           block_groups.emplace_back(image, i);
//       }
        ext2_inode root = getInode(2);
        std::cout << root.i_blocks << std::endl;
        std::cout << root.i_block[0] << std::endl;
        File file{image, root};
        file.readDirectory(image);
        std::cout << std::get<0>(file.children[0]) << std::endl;


    }


    // skip boot
    // read and store superblock
    // create blockgroup
    // read all other blockgroups
};


#endif //EXT2_FSCK_FIESYSTEM_H
