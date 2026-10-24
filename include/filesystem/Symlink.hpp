#pragma once

#include "filesystem/Node.hpp"

namespace fsim {

class Symlink final : public Node {
public:
    Symlink(std::string name,
            std::string target,
            Permissions permissions = {});

    NodeType type() const noexcept override;
    const std::string& target() const noexcept;

private:
    std::string target_;
};

} // namespace fsim
