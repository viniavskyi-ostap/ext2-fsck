#include <linux/fs.h>
#include <iostream>


#include "Filesystem.h"

int main(int argc, char** argv) {
    if (argc < 2)
        throw std::invalid_argument("Missing second argument: filesystem name");

    Filesystem fs{argv[1]};

    if (fs.errors.empty()){
        std::cout << fs << std::endl << std::endl;
        std::cout << fs.fileTreeString() << std::endl;
    }
    std::cout << fs.getAllErrors().substr(0, 1000) << std::endl;

    return 0;
}