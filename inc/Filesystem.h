#ifndef EXT2_FSCK_FIESYSTEM_H
#define EXT2_FSCK_FIESYSTEM_H

#include <fstream>

class Filesystem {
    std::ifstream input;
    uint64_t block_size;

public:
    Filesystem(std::string path) {
        input = std::ifstream{path, std::ios::binary};
    }

    // skip boot
    // read and store superblock
    // create blockgroup
    // read all other blockgroups
};


#endif //EXT2_FSCK_FIESYSTEM_H
