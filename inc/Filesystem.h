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
#include <iostream>
#include <sstream>
#include <stack>

const uint32_t ROOT_INODE_I = 2;

class Filesystem {
public:
    FilesystemImage image{};
    std::vector<BlockGroup> block_groups;

    std::unordered_map<uint32_t, INode> inodes;
    std::vector<std::string> errors;

    ext2_inode getInode(uint32_t i) {
        uint32_t group_i = i / image.inodes_per_group;
        if (group_i >= block_groups.size())
            throw InvalidINode("Tried to access INode in unexisting group " + std::to_string(group_i));

        return block_groups[i / image.inodes_per_group].getInode(i);
    }

public:
    friend std::ostream& operator<<(std::ostream& out, Filesystem& fs) {
        out << "[Filesystem" << std::endl;
        out << "\tblocks number: " << fs.image.blocks_count << std::endl;
        out << "\tblocks per group: " << fs.image.blocks_per_group << std::endl;
        out << "\tgroups number: " << fs.block_groups.size() << std::endl;
        out << "\tinode table size in blocks: " << fs.image.inodes_per_group * sizeof(ext2_inode) / fs.image.block_size << std::endl;
        out << "]";
        return out;
    };

    void recurseDirectory(INode& directory) {
        if (!directory.errors.empty()) return;

        for (auto block: directory.blocks) {
            if (image.block_usage[block])
                directory.errors.push_back("Referencing already used block " + std::to_string(block));
            image.block_usage[block] = true;
        }

        if (directory.type != DIRECTORY) return;
        directory.readDirectory(image);

        for (auto &child: directory.children) {
            uint32_t inode_i = std::get<1>(child);
            if (inodes.count(inode_i)) continue;

            try {
                ext2_inode ext2Inode = getInode(inode_i);

                inodes[inode_i] = INode(image, ext2Inode, inode_i);
                INode& inode = inodes[inode_i];

                recurseDirectory(inode);
            } catch(InvalidINode& e) {
                // invalid inode is not saved to inodes map
                directory.errors.push_back("Referencing invalid inode " + std::to_string(inode_i));
            }
        }
    }

    void __fileTreeStringOneDepth(INode& directory, int depth, std::ostringstream& out, std::unordered_set<uint32_t>& usedMap) {
        for (auto child : directory.children) {
            if (usedMap.count(std::get<1>(child))) continue;
            if (!inodes.count(std::get<1>(child))) continue;
            if (std::get<0>(child)[0] == '.') continue;

            usedMap.insert(std::get<1>(child));
            for (int j = 0; j < depth; ++j) out << "  ";
            out << "\\" << std::get<0>(child) << std::endl;
            INode& inode = inodes[std::get<1>(child)];

            if (inode.type == DIRECTORY) {
                __fileTreeStringOneDepth(inode, depth+1, out, usedMap);
            }
        }
    }

    std::string fileTreeString(INode& directory) {
        std::ostringstream out;
        if (directory.type != DIRECTORY)
            return out.str();

        std::unordered_set<uint32_t> usedMap;
        __fileTreeStringOneDepth(directory, 0, out, usedMap);
        return out.str();
    }
    std::string fileTreeString() {
        return fileTreeString(inodes[ROOT_INODE_I]);
    }

    Filesystem(std::string path) {
        // Open the filesystem as file
        image.istream = std::ifstream{path, std::ios::binary};

        image.istream.seekg(0, std::ios::end);
        image.filesystem_size = image.istream.tellg();

        // Create the block groups
        block_groups.emplace_back(image, 0);

        // Save the size of the filesystem
        // TODO perform other super block checks

        uint64_t groups_count = std::ceil((double) image.blocks_count / image.blocks_per_group);
        for (uint64_t i = 1; i < groups_count; ++i) {
            block_groups.emplace_back(image, i);
        }

        // TODO check all other super blocks

        // Root folder
        inodes[ROOT_INODE_I] = INode{image, getInode(ROOT_INODE_I), ROOT_INODE_I};
        // Recursively add all the inodes
        recurseDirectory(inodes[ROOT_INODE_I]);
    }

    std::string getAllErrors() {
        std::ostringstream out;
        int error_number = 0;

        out << "ERRORS: " << std::endl;
        for (auto& error: errors) {
            out << "[Filesystem error] " << error << std::endl;
            error_number += 1;
        }
        for (size_t i = 0; i < block_groups.size(); ++i) {
            for (auto& error: block_groups[i].errors) {
                out << "[Block Group " << i << " error] " << error << std::endl;
                error_number += 1;
            }
        }
        for (auto& inode: inodes) {
            for (auto& error: inode.second.errors) {
                out << "[Inode " << inode.first << " error] " << error << std::endl;
                error_number += 1;
            }
        }

        out << "Total number of errors = " <<  error_number;
        return out.str();
    }
};


#endif //EXT2_FSCK_FIESYSTEM_H
