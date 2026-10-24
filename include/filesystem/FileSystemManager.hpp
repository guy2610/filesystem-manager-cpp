#pragma once

#include "filesystem/Directory.hpp"

#include <cstddef>
#include <deque>
#include <string>
#include <utility>

namespace fsim {

class FileSystemManager {
public:
    FileSystemManager();

    Node* find(
        const std::string& path,
        bool followFinalSymlink = true
    );

    void createFile(
        const std::string& path,
        std::size_t size = 0,
        Permissions permissions = {}
    );

    void createDirectory(
        const std::string& path,
        Permissions permissions = {}
    );

    void createSymlink(
        const std::string& path,
        std::string target,
        Permissions permissions = {}
    );

    void remove(const std::string& path);

private:
    Directory root_;

    static std::deque<std::string> splitPath(
        const std::string& path
    );

    std::pair<Directory*, std::string> resolveParent(
        const std::string& path
    );

    Node* resolve(
        const std::string& path,
        bool followFinalSymlink
    );
};

} // namespace fsim
