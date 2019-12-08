#ifndef EXT2_FSCK_FILESYSTEMERRORS_H
#define EXT2_FSCK_FILESYSTEMERRORS_H

#include <stdexcept>

struct BaseFilesystemException: std::exception{
    std::string message;
    explicit BaseFilesystemException(std::string message): message(std::move(message)) {}
    std::string what_message() { return message; }
};

struct InvalidINode: BaseFilesystemException {
    explicit InvalidINode(std::string message): BaseFilesystemException(std::move(message)) {}
};

#endif //EXT2_FSCK_FILESYSTEMERRORS_H
