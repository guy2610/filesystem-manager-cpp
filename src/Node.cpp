#include "filesystem/Node.hpp"

#include <utility>

namespace fsim {

Node::Node(std::string name, Permissions permissions)
    : name_(std::move(name)),
      permissions_(permissions) {}

const std::string& Node::name() const noexcept {
    return name_;
}

Permissions Node::permissions() const noexcept {
    return permissions_;
}

void Node::setPermissions(Permissions permissions) noexcept {
    permissions_ = permissions;
}

} // namespace fsim
