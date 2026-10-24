#pragma once

#include <string>

namespace fsim {

enum class NodeType {
    File,
    Directory,
    Symlink
};

struct Permissions {
    bool read = true;
    bool write = true;
    bool execute = false;
};

class Node {
public:
    virtual ~Node() = default;

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    virtual NodeType type() const noexcept = 0;

    const std::string& name() const noexcept;
    Permissions permissions() const noexcept;
    void setPermissions(Permissions permissions) noexcept;

protected:
    Node(std::string name, Permissions permissions);

private:
    std::string name_;
    Permissions permissions_;
};

} // namespace fsim
