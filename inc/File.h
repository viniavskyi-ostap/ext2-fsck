#ifndef EXT2_FSCK_FILE_H
#define EXT2_FSCK_FILE_H

#include <tuple>
#include <cmath>
#include <ext2fs/ext2_fs.h>
#include <vector>

#include "FilesystemImage.h"


class File {
public:
    ext2_inode inode;
    std::vector<uint32_t> blocks;

    using dir_entry_t = std::tuple<std::string, uint32_t>;
    std::vector<dir_entry_t> children;

    std::vector<uint32_t> getBlocks(FilesystemImage& image) {
        uint32_t block_count = std::ceil((double) inode.i_size / image.block_size);
        std::vector<uint32_t> blocks(block_count);

        for (size_t i = 0; i < block_count && i < EXT2_NDIR_BLOCKS; ++i) {
            blocks[i] = inode.i_block[i];
        }
        // TODO handle indirect blocks

        return blocks;
    }

    void readDirectory(FilesystemImage& image) {
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

            children.emplace_back(filename, entry.inode);
            inside_offset += entry.rec_len;
        }

    }

public:
    File(FilesystemImage& image, ext2_inode inode): inode(inode) {
        blocks = getBlocks(image);

        if (EXT2_FT_DIR & inode.i_mode) {

        }
    }
};

#endif //EXT2_FSCK_FILE_H
