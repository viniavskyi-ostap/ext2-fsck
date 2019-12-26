#ifndef EXT2_FSCK_FIESYSTEM_H
#define EXT2_FSCK_FIESYSTEM_H

#include <ext2fs/ext2_fs.h>
#include <fstream>
#include <vector>
#include <cmath>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include "BlockGroup.h"
#include "FilesystemImage.h"
#include "FilesystemErrors.h"
#include "INode.h"
#include <sstream>

const uint32_t ROOT_INODE_I = 2;

class Filesystem {
public:
    FilesystemImage image{};
    std::vector<BlockGroup> block_groups;

    std::unordered_map<uint32_t, INode> inodes;
    std::vector<std::string> errors;

    ext2_inode getInode(uint32_t i);
    void createFilesystemTree(INode& directory);

public:
    Filesystem(std::string path);

    friend std::ostream& operator<<(std::ostream& out, Filesystem& fs);
    std::string getAllErrors();

    void __fileTreeStringOneDepth(INode& directory, int depth, std::ostringstream& out, std::unordered_set<uint32_t>& usedMap);
    std::string fileTreeString(INode& directory);
    std::string fileTreeString();

};


#endif //EXT2_FSCK_FIESYSTEM_H
