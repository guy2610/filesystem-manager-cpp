#include "filesystem/Directory.hpp"

#include <stdexcept>
#include <utility>

namespace fsim {

Directory::Directory(std::string name,
                     Permissions permissions)
    : Node(std::move(name), permissions) {}

NodeType Directory::type() const noexcept {
    return NodeType::Directory;
}

Node* Directory::findChild(const std::string& name) {
    auto it = children_.find(name);
    return it == children_.end()
        ? nullptr
        : it->second.get();
}

const Node* Directory::findChild(
    const std::string& name) const {

    auto it = children_.find(name);
    return it == children_.end()
        ? nullptr
        : it->second.get();
}

void Directory::addChild(std::unique_ptr<Node> child) {
    if (!child) {
        throw std::invalid_argument("Null child");
    }

    const std::string name = child->name();

    if (name.empty() ||
        name == "." ||
        name == ".." ||
        name.find('/') != std::string::npos) {
        throw std::invalid_argument("Invalid child name");
    }

    if (children_.find(name) != children_.end()) {
        throw std::invalid_argument("Duplicate child name");
    }

    children_.emplace(name, std::move(child));
}

std::unique_ptr<Node> Directory::removeChild(
    const std::string& name) {

    auto it = children_.find(name);

    if (it == children_.end()) {
        throw std::invalid_argument("Child not found");
    }

    auto removed = std::move(it->second);
    children_.erase(it);

    return removed;
}

const Directory::Children&
Directory::children() const noexcept {
    return children_;
}

} // namespace fsim
