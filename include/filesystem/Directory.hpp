#pragma once

#include "filesystem/Node.hpp"

#include <map>
#include <memory>
#include <string>

namespace fsim {

class Directory final : public Node {
public:
    explicit Directory(
        std::string name,
        Permissions permissions = {});

    NodeType type() const noexcept override;

    Node* findChild(const std::string& name);
    const Node* findChild(const std::string& name) const;

    void addChild(std::unique_ptr<Node> child);

    std::unique_ptr<Node> removeChild(
        const std::string& name);

    using Children =
        std::map<std::string, std::unique_ptr<Node>>;

    const Children& children() const noexcept;

private:
    Children children_;
};

} // namespace fsim
