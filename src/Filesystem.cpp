#include "Filesystem.h"

ext2_inode Filesystem::getInode(uint32_t i) {
    uint32_t group_i = i / image.inodes_per_group;
    if (group_i >= block_groups.size())
        throw InvalidINode("Tried to access INode in unexisting group " + std::to_string(group_i));

    return block_groups[i / image.inodes_per_group].getInode(i);
}

void Filesystem::createFilesystemTree(INode& directory) {
    // recursive function to read the inodes and blocks of filesystem
    if (!directory.errors.empty()) return;
    if (directory.type != DIRECTORY) return;

    directory.readDirectory(image);

    for (auto &child: directory.children) {
        uint32_t inode_i = std::get<1>(child);
        if (inodes.count(inode_i)) continue;

        try {
            ext2_inode ext2Inode = getInode(inode_i);

            inodes[inode_i] = INode(image, ext2Inode, inode_i);
            INode& inode = inodes[inode_i];

            createFilesystemTree(inode);
        } catch(InvalidINode& e) {
            // invalid inode is not saved to inodes map
            directory.errors.push_back("Referencing invalid inode " + std::to_string(inode_i));
        }
    }
}

Filesystem::Filesystem(std::string path) {
    // Open the filesystem as file
    image.istream = std::ifstream{path, std::ios::binary};

    // Save the size of the filesystem
    image.istream.seekg(0, std::ios::end);
    image.filesystem_size = image.istream.tellg();

    if (readSuperBlock() == 0) {
        initBlockUsageTable();

        // Create the block groups
        block_groups.emplace_back(image, 0);

        // Save the size of the filesystem
        // TODO perform other super block checks

        uint64_t groups_count = std::ceil((double) image.blocks_count / image.blocks_per_group);
        for (uint64_t i = 1; i < groups_count; ++i) {
            block_groups.emplace_back(image, i);
        }

        if (!block_groups[0].errors.empty())
            throw std::runtime_error("The filesystem cannot be read.");
        // Root folder
        inodes[ROOT_INODE_I] = INode{image, getInode(ROOT_INODE_I), 2};
        // Recursively add all the inodes
        createFilesystemTree(inodes[ROOT_INODE_I]);

        for (size_t i = 0; i < image.block_usage.size(); ++i) {
            std::cout << image.block_usage[i];
        }
        std::cout << std::endl;

        for (uint64_t i = 0; i< groups_count; ++i) {
            block_groups[i].additionalFieldsCheck();
        }
    }
}

std::ostream& operator<<(std::ostream& out, Filesystem& fs) {
    out << "[Filesystem" << std::endl;
    out << "\tblock size: " << fs.image.block_size << std::endl;
    out << "\tblocks number: " << fs.image.blocks_count << std::endl;
    out << "\tblocks per group: " << fs.image.blocks_per_group << std::endl;
    out << "\tgroups number: " << fs.block_groups.size() << std::endl;
    out << "\tinode table size in blocks: " << fs.image.inodes_per_group * sizeof(ext2_inode) / fs.image.block_size << std::endl;
    out << "]";
    return out;
}

std::string Filesystem::getAllErrors() {
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

void Filesystem::__fileTreeStringOneDepth(INode& directory, int depth, std::ostringstream& out, std::unordered_set<uint32_t>& usedMap) {
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

std::string Filesystem::fileTreeString(INode& directory) {
    std::ostringstream out;
    if (directory.type != DIRECTORY)
        return out.str();

    std::unordered_set<uint32_t> usedMap;
    __fileTreeStringOneDepth(directory, 0, out, usedMap);
    return out.str();
}
std::string Filesystem::fileTreeString() {
    return fileTreeString(inodes[ROOT_INODE_I]);
}

int Filesystem::readSuperBlock(){
    ext2_super_block super;

    if (BOOT_SIZE + sizeof(super) > image.filesystem_size) {
        errors.emplace_back("Super block out of filesystem");
        return -1;
    }

    image.istream.seekg(BOOT_SIZE, std::ios::beg);
    image.istream.read((char *) &super, sizeof(super));

    if (super.s_magic != EXT2_SUPER_MAGIC){
        errors.emplace_back("Given file is not an EXT2 FS image");
        return -2;
    }

    image.blocks_count = super.s_blocks_count;
    image.block_size = EXT2_BLOCK_SIZE(&super);
    image.blocks_per_group = super.s_blocks_per_group;
    image.inodes_per_group = super.s_inodes_per_group;

    image.inode_usage = std::vector<bool>(std::ceil(image.filesystem_size / image.inodes_per_group) * image.inodes_per_group);
    image.inode_usage[0] = true;

    // check if correct magick number
    return 0;
}

void Filesystem::initBlockUsageTable() {
    image.block_usage = std::vector<bool>(image.filesystem_size / image.block_size);

    uint32_t groups_count = std::ceil((double) image.blocks_count / image.blocks_per_group);
    uint32_t group_desc_blocks = std::ceil(double(sizeof(ext2_group_desc) * groups_count) / image.block_size);

    // iterating through 0, 1, 3 ^ n, 5 ^ n, 7 ^ n
    for (auto num: std::vector<uint32_t>{0, 1, 3, 5, 7}) {
        uint32_t i = num;
        while (i < groups_count) {
            // skip the boot + super if it is the first group
            uint32_t super_size = (i == 0 && image.block_size <= BOOT_SIZE) ? 2 : 1;
            // iterating through the super block and the group description table
            for (uint32_t j = 0; j < group_desc_blocks + super_size; ++j)
                image.block_usage[image.blocks_per_group * i + j] = true;

            if (num == 0 || num == 1) break;
            i *= num;
        }
    }
}

int Filesystem::readBlockGroupTable() {
    uint32_t groups_count = std::ceil(image.blocks_count) / image.blocks_per_group;
    block_group_descriptions = std::vector<ext2_group_desc>(groups_count);

    if (BOOT_SIZE + image.block_size + sizeof(ext2_group_desc) * groups_count > image.filesystem_size) {
        errors.emplace_back("Groups description table out of filesystem");
        return -1;
    }

    image.istream.seekg(BOOT_SIZE + image.block_size, std::ios::beg);
    for (uint32_t i = 0; i < groups_count; ++i) {
        image.istream.read((char *) &block_group_descriptions[i], sizeof(ext2_group_desc));
    }

    return 0;
}
