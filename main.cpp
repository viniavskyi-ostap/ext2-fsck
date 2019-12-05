#include <linux/fs.h>
#include <iostream>


#include "Filesystem.h"


int main() {
    Filesystem fs{"../file"};
    std::cout << fs.image.blocks_count << std::endl;

    return 0;
}