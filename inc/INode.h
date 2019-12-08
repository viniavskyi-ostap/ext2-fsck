#ifndef EXT2_FSCK_INODE_H
#define EXT2_FSCK_INODE_H

#include <tuple>
#include <cmath>
#include <ext2fs/ext2_fs.h>
#include <vector>
#include <ctime>

#include "FilesystemImage.h"

enum file_type {
    UNKNOWN,
    FIFO,
    CHAR_DEVICE,
    DIRECTORY,
    BLOCK_DEVICE,
    REGULAR,
    SYMLINK,
    SOCKET
};

std::vector<std::string> file_type_names = {"unknown", "fifo", "character device", "directory", "block device", "regular", "symlink", "socket"};

inline file_type get_file_type(const ext2_inode& inode) {
    int mode = inode.i_mode >> 12;
    switch(mode) {
        case 1: return FIFO;
        case 2: return CHAR_DEVICE;
        case 4: return DIRECTORY;
        case 6: return BLOCK_DEVICE;
        case 8: return REGULAR;
        case 10: return SYMLINK;
        case 12: return SOCKET;
        default: return UNKNOWN;
    }
}

class INode {
public:
    unsigned int inode_i;
    ext2_inode inode;
    std::vector<uint32_t> blocks;
    file_type type = UNKNOWN;

    std::vector<std::string> errors;

    using dir_entry_t = std::tuple<std::string, uint32_t>;
    std::vector<dir_entry_t> children;

    std::vector<uint32_t> getBlocks(FilesystemImage& image) {
        uint32_t block_count = std::ceil((double) inode.i_size / image.block_size);
        std::vector<uint32_t> blocks(block_count);

        for (size_t i = 0; i < block_count && i < EXT2_NDIR_BLOCKS; ++i) {
            blocks[i] = inode.i_block[i];
        }
        // TODO handle indirect blocks
        // TODO in the handling check the blocks

        return blocks;
    }

    void readDirectory(FilesystemImage& image) {
        children = {};

        uint32_t block_i = 0;
        uint32_t inside_offset = 0;
        char filename[EXT2_NAME_LEN + 1];

        ext2_dir_entry_2 entry;
        while(true) {
            if (inside_offset >= image.block_size) {
                ++block_i;
                if (block_i >= blocks.size()) return;
            }

            image.istream.seekg(blocks[block_i] * image.block_size + inside_offset, std::ios::beg);
            image.istream.read((char *) &entry.inode, sizeof(entry.inode));
            image.istream.read((char *) &entry.rec_len, sizeof(entry.rec_len));
            image.istream.read((char *) &entry.name_len, sizeof(entry.name_len));
            image.istream.read((char *) &entry.file_type, sizeof(entry.file_type));
            image.istream.read((char *) &entry.name, entry.name_len);

            std::copy(entry.name, entry.name + entry.name_len, filename);
            filename[entry.name_len] = '\0';

            if (entry.name_len)
                children.emplace_back(filename, entry.inode);
            inside_offset += entry.rec_len;
        }
    }

    void readSymLink(FilesystemImage &image) {
        // TODO implement
    }

public:
    friend std::ostream& operator<<(std::ostream& out, INode& file) {
        out << "[INode" << std::endl;
        out << "\tinode number: " << file.inode_i << std::endl;
        out << "\ttype: " << file_type_names[file.type] << std::endl;
        out << "\tblocks number: " << file.blocks.size() << std::endl;
        out << "\tfile size: " << file.inode.i_size << std::endl;

        std::time_t creation_time = file.inode.i_ctime;
        std::time_t modification_time = file.inode.i_mtime;

        out << "\tcreation date: " << std::asctime(std::localtime(&creation_time));
        out << "\tmodification date: " << std::asctime(std::localtime(&modification_time));

        if (file.type == DIRECTORY) {
            out << "\tchildren: [" << std::endl;
            for (auto &child: file.children) {
                out << "\t\t" << std::get<1>(child) << " " << std::get<0>(child) << std::endl;
            }
            out << "\t]" << std::endl;
        }
        out << "]";
        return out;
    };

    INode() = default;
    INode(FilesystemImage& image, ext2_inode inode, unsigned int inode_i):
        inode(inode), inode_i(inode_i)
    {
        blocks = getBlocks(image);

        for (unsigned int block : blocks) {
            if (block >= image.blocks_count)
                errors.push_back("File referencing invalid block " + std::to_string(block));
        }

        type = get_file_type(inode);
    }
};

#endif //EXT2_FSCK_INODE_H
