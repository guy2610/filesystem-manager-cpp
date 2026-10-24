#include "filesystem/Directory.hpp"
#include "filesystem/File.hpp"
#include "filesystem/Symlink.hpp"

#include <iostream>
#include <memory>
#include <utility>

int main() {
    fsim::Directory root("/");

    auto home = std::make_unique<fsim::Directory>("home");

    home->addChild(
        std::make_unique<fsim::File>("notes.txt", 1024)
    );

    home->addChild(
        std::make_unique<fsim::Symlink>(
            "latest", "notes.txt")
    );

    root.addChild(std::move(home));

    auto* homeNode = root.findChild("home");

    if (!homeNode) {
        return 1;
    }

    auto& homeDir =
        static_cast<fsim::Directory&>(*homeNode);

    for (const auto& [name, node] : homeDir.children()) {
        std::cout << name << " - ";

        switch (node->type()) {
            case fsim::NodeType::File:
                std::cout << "File";
                break;
            case fsim::NodeType::Directory:
                std::cout << "Directory";
                break;
            case fsim::NodeType::Symlink:
                std::cout << "Symlink";
                break;
        }

        std::cout << '\n';
    }

    return 0;
}
