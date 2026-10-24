#include "filesystem/FileSystemManager.hpp"
#include "filesystem/File.hpp"

#include <iostream>

int main() {
    fsim::FileSystemManager fs;

    fs.createDirectory("/home");
    fs.createDirectory("/home/user");

    fs.createFile("/home/user/notes.txt", 1024);

    fs.createSymlink(
        "/home/user/latest",
        "notes.txt"
    );

    auto* node = fs.find("/home/user/latest");

    auto* file = dynamic_cast<fsim::File*>(node);

    if (!file) {
        std::cerr << "Expected a file\n";
        return 1;
    }

    std::cout << "File: " << file->name() << '\n';
    std::cout << "Size: " << file->size() << '\n';

    fs.remove("/home/user/latest");

    std::cout << "Symlink removed successfully\n";

    return 0;
}
