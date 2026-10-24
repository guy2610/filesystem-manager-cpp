#include "filesystem/Symlink.hpp"

#include <utility>

namespace fsim {

Symlink::Symlink(std::string name,
                 std::string target,
                 Permissions permissions)
    : Node(std::move(name), permissions),
      target_(std::move(target)) {}

NodeType Symlink::type() const noexcept {
    return NodeType::Symlink;
}

const std::string& Symlink::target() const noexcept {
    return target_;
}

} // namespace fsim
