#include "filesystem/File.hpp"

#include <utility>

namespace fsim {

File::File(std::string name,
           std::size_t size,
           Permissions permissions)
    : Node(std::move(name), permissions),
      size_(size) {}

NodeType File::type() const noexcept {
    return NodeType::File;
}

std::size_t File::size() const noexcept {
    return size_;
}

} // namespace fsim
