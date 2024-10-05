#include "module.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

ModuleManager::ModuleManager() : modulePath("module/") {
    loadExistingModules();
}

bool ModuleManager::addModule(const std::string& moduleName) {
    if (moduleExists(moduleName)) {
        return false;
    }

    // Here you would implement the actual module addition logic
    // For now, we'll just add the module name to the list
    loadedModules.push_back(moduleName);
    return true;
}

std::vector<std::string> ModuleManager::listModules() const {
    return loadedModules;
}

bool ModuleManager::moduleExists(const std::string& moduleName) const {
    return std::find(loadedModules.begin(), loadedModules.end(), moduleName) != loadedModules.end();
}

void ModuleManager::loadExistingModules() {
    for (const auto& entry : fs::directory_iterator(modulePath)) {
        if (entry.is_directory()) {
            loadedModules.push_back(entry.path().filename().string());
        }
    }
}
