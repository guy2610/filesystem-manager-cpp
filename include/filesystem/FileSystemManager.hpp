#pragma once

#include "filesystem/Directory.hpp"

#include <cstddef>
#include <deque>
#include <iostream>
#include <vector>
#include <string>
#include <utility>

namespace fsim {

enum class SortCriteria {
    Name,
    Size
};

struct FileEntry {
    std::string path;
    std::size_t size;
};

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

    Permissions getPermissions(
        const std::string& path
    );

    void changePermissions(
        const std::string& path,
        Permissions permissions,
        bool recursive = false
    );

    std::size_t maxDepth(
        const std::string& path
    );

    bool containsFile(
        const std::string& directoryPath,
        const std::string& filename
    );

    std::vector<FileEntry> listFiles(
        const std::string& directoryPath,
        SortCriteria criteria = SortCriteria::Name
    );

    void printFiles(
        const std::string& directoryPath,
        SortCriteria criteria = SortCriteria::Name,
        std::ostream& out = std::cout
    );

private:
    // TODO: Implement after interview reconstruction.
    static std::size_t calculateDepth(
        const Directory& directory
    );

    static bool findFileRecursive(
        const Directory& directory,
        const std::string& filename
    );

    static void collectFiles(
        const Directory& directory,
        const std::string& prefix,
        std::vector<FileEntry>& files
    );

    static void changePermissionsRecursive(
        Node& node,
        Permissions permissions
    );

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
