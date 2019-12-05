#include <linux/fs.h>
#include <ext2fs/ext2_fs.h>
#include <iostream>

#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <cmath>

#include "Filesystem.h"


int main() {
    Filesystem fs{"../file"};
    std::cout << fs.image.blocks_count << std::endl;

    return 0;
}