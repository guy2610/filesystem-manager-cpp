#pragma once

#include "filesystem/Node.hpp"
#include <cstddef>

namespace fsim {

class File final : public Node {
public:
    File(std::string name,
         std::size_t size,
         Permissions permissions = {});

    NodeType type() const noexcept override;
    std::size_t size() const noexcept;

private:
    std::size_t size_;
};

} // namespace fsim
