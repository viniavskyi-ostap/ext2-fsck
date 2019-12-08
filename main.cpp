#include <linux/fs.h>
#include <iostream>


#include "Filesystem.h"

int main() {
    Filesystem fs{"../file"};
    std::cout << fs << std::endl;
    std::cout << fs.fileTreeString() << std::endl;
    std::cout << fs.getAllErrors() << std::endl;

    return 0;
}