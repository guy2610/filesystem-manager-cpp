#include "filesystem/FileSystemManager.hpp"

#include "filesystem/File.hpp"
#include "filesystem/Symlink.hpp"

#include <algorithm>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

namespace fsim {

FileSystemManager::FileSystemManager()
    : root_("/") {}

std::deque<std::string> FileSystemManager::splitPath(
    const std::string& path) {

    std::deque<std::string> parts;
    std::size_t pos = 0;

    while (pos < path.size()) {
        while (pos < path.size() && path[pos] == '/') {
            ++pos;
        }

        if (pos == path.size()) {
            break;
        }

        auto end = path.find('/', pos);

        if (end == std::string::npos) {
            end = path.size();
        }

        parts.emplace_back(path.substr(pos, end - pos));
        pos = end;
    }

    return parts;
}

Node* FileSystemManager::resolve(
    const std::string& path,
    bool followFinalSymlink) {

    if (path.empty() ||
        (path.size() > 1 && path.back() == '/')) {
        throw std::invalid_argument("Invalid path");
    }

    constexpr std::size_t maxSymlinks = 40;
    std::size_t followed = 0;

    auto remaining = splitPath(path);

    // Stack of directories along the resolved path.
    std::vector<Directory*> dirs{&root_};

    while (!remaining.empty()) {
        std::string part = std::move(remaining.front());
        remaining.pop_front();

        if (part == ".") {
            continue;
        }

        if (part == "..") {
            if (dirs.size() > 1) {
                dirs.pop_back();
            }
            continue;
        }

        Node* node = dirs.back()->findChild(part);

        if (!node) {
            throw std::invalid_argument(
                "Path component not found: " + part
            );
        }

        const bool isLast = remaining.empty();

        if (node->type() == NodeType::Symlink &&
            (!isLast || followFinalSymlink)) {

            if (++followed > maxSymlinks) {
                throw std::runtime_error(
                    "Too many symbolic links"
                );
            }

            auto* link = static_cast<Symlink*>(node);
            const auto& target = link->target();

            if (target.empty()) {
                throw std::invalid_argument(
                    "Empty symbolic link target"
                );
            }

            auto targetParts = splitPath(target);

            // Absolute targets start from root.
            if (target.front() == '/') {
                dirs.resize(1);
            }

            // Relative targets start from link's parent.
            while (!targetParts.empty()) {
                remaining.push_front(
                    std::move(targetParts.back())
                );
                targetParts.pop_back();
            }

            continue;
        }

        if (isLast) {
            return node;
        }

        if (node->type() != NodeType::Directory) {
            throw std::invalid_argument(
                "Path component is not a directory"
            );
        }

        dirs.push_back(static_cast<Directory*>(node));
    }

    return dirs.back();
}

std::pair<Directory*, std::string>
FileSystemManager::resolveParent(
    const std::string& path) {

    if (path.empty() || path.back() == '/') {
        throw std::invalid_argument("Invalid entity path");
    }

    auto parts = splitPath(path);

    if (parts.empty()) {
        throw std::invalid_argument("Cannot modify root");
    }

    std::string name = std::move(parts.back());
    parts.pop_back();

    if (name.empty() || name == "." || name == "..") {
        throw std::invalid_argument("Invalid entity name");
    }

    std::string parentPath = "/";

    for (const auto& part : parts) {
        if (parentPath.size() > 1) {
            parentPath += '/';
        }
        parentPath += part;
    }

    Node* parentNode = resolve(parentPath, true);
    auto* parent = dynamic_cast<Directory*>(parentNode);

    if (!parent) {
        throw std::invalid_argument(
            "Parent is not a directory"
        );
    }

    return {parent, std::move(name)};
}

Node* FileSystemManager::find(
    const std::string& path,
    bool followFinalSymlink) {

    return resolve(path, followFinalSymlink);
}

void FileSystemManager::createFile(
    const std::string& path,
    std::size_t size,
    Permissions permissions) {

    auto [parent, name] = resolveParent(path);

    parent->addChild(
        std::make_unique<File>(
            std::move(name), size, permissions
        )
    );
}

void FileSystemManager::createDirectory(
    const std::string& path,
    Permissions permissions) {

    auto [parent, name] = resolveParent(path);

    parent->addChild(
        std::make_unique<Directory>(
            std::move(name), permissions
        )
    );
}

void FileSystemManager::createSymlink(
    const std::string& path,
    std::string target,
    Permissions permissions) {

    if (target.empty()) {
        throw std::invalid_argument(
            "Symbolic link target cannot be empty"
        );
    }

    auto [parent, name] = resolveParent(path);

    parent->addChild(
        std::make_unique<Symlink>(
            std::move(name),
            std::move(target),
            permissions
        )
    );
}

void FileSystemManager::remove(
    const std::string& path) {

    auto [parent, name] = resolveParent(path);
    parent->removeChild(name);
}


Permissions FileSystemManager::getPermissions(
    const std::string& path) {

    Node* node = resolve(path, false);
    return node->permissions();
}

void FileSystemManager::changePermissions(
    const std::string& path,
    Permissions permissions,
    bool recursive) {

    Node* node = resolve(path, false);

    if (recursive) {
        changePermissionsRecursive(*node, permissions);
    } else {
        node->setPermissions(permissions);
    }
}

void FileSystemManager::changePermissionsRecursive(
    Node& node,
    Permissions permissions) {

    node.setPermissions(permissions);

    if (node.type() != NodeType::Directory) {
        return;
    }

    auto& directory = static_cast<Directory&>(node);

    for (const auto& [name, child] :
         directory.children()) {

        changePermissionsRecursive(
            *child, permissions
        );
    }
}


std::size_t FileSystemManager::maxDepth(
    const std::string& path) {

    Node* node = resolve(path, true);

    if (node->type() != NodeType::Directory) {
        throw std::invalid_argument(
            "Path is not a directory");
    }

    return calculateDepth(
        static_cast<const Directory&>(*node));
}

bool FileSystemManager::containsFile(
    const std::string& directoryPath,
    const std::string& filename) {

    Node* node = resolve(directoryPath, true);

    if (node->type() != NodeType::Directory) {
        throw std::invalid_argument(
            "Path is not a directory");
    }

    return findFileRecursive(
        static_cast<const Directory&>(*node),
        filename);
}

std::vector<FileEntry> FileSystemManager::listFiles(
    const std::string& directoryPath,
    SortCriteria criteria) {

    Node* node = resolve(directoryPath, true);

    if (node->type() != NodeType::Directory) {
        throw std::invalid_argument(
            "Path is not a directory");
    }

    std::vector<FileEntry> files;

    collectFiles(
        static_cast<const Directory&>(*node),
        "",
        files);

    if (criteria == SortCriteria::Name) {
        std::sort(
            files.begin(),
            files.end(),
            [](const FileEntry& a, const FileEntry& b) {
                return a.path < b.path;
            });
    } else {
        std::sort(
            files.begin(),
            files.end(),
            [](const FileEntry& a, const FileEntry& b) {
                if (a.size != b.size) {
                    return a.size < b.size;
                }
                return a.path < b.path;
            });
    }

    return files;
}

void FileSystemManager::printFiles(
    const std::string& directoryPath,
    SortCriteria criteria,
    std::ostream& out) {

    const auto files = listFiles(
        directoryPath, criteria);

    for (const auto& file : files) {
        out << file.path
            << " (" << file.size << " bytes)\n";
    }
}

// ==================================================
// Interview reconstruction boundary
// The recursive helpers were NOT implemented.
// ==================================================

std::size_t FileSystemManager::calculateDepth(
    const Directory&) {

    throw std::logic_error(
        "TODO: calculateDepth");
}

bool FileSystemManager::findFileRecursive(
    const Directory&,
    const std::string&) {

    throw std::logic_error(
        "TODO: findFileRecursive");
}

void FileSystemManager::collectFiles(
    const Directory&,
    const std::string&,
    std::vector<FileEntry>&) {

    throw std::logic_error(
        "TODO: collectFiles");
}

} // namespace fsim
