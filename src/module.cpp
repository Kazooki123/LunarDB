#include "module.h"
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdlib>

namespace fs = std::filesystem;

ModuleManager::ModuleManager() : modulePath("../modules/") {
    loadExistingModules();
}

bool ModuleManager::addModule(const std::string& moduleName) {
    if (moduleExists(moduleName)) {
        return false;
    }
    loadedModules.push_back(moduleName);
    return true;
}

bool ModuleManager::removeModule(const std::string& moduleName) {
    auto it = std::find(loadedModules.begin(), loadedModules.end(), moduleName);
    if (it != loadedModules.end()) {
        loadedModules.erase(it);
        return true;
    }
    return false;
}

std::vector<std::string> ModuleManager::listModules() const {
    return loadedModules;
}

bool ModuleManager::installModule(const std::string& moduleName, const std::string& repoUrl) {
    // This function would implement the module installation logic
    // For now, we'll just add the module name to the list
    return addModule(moduleName);
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

// C interface implementation
extern "C" {
    ModuleManager* create_module_manager() {
        return new ModuleManager();
    }

    void destroy_module_manager(ModuleManager* manager) {
        delete manager;
    }

    bool add_module(ModuleManager* manager, const char* moduleName) {
        return manager->addModule(std::string(moduleName));
    }

    bool remove_module(ModuleManager* manager, const char* moduleName) {
        return manager->removeModule(std::string(moduleName));
    }

    const char** list_modules(ModuleManager* manager, int* count) {
        auto modules = manager->listModules();
        *count = modules.size();
        const char** result = (const char**)malloc(sizeof(char*) * *count);
        for (int i = 0; i < *count; i++) {
            result[i] = strdup(modules[i].c_str());
        }
        return result;
    }

    void free_module_list(const char** moduleList, int count) {
        for (int i = 0; i < count; i++) {
            free((void*)moduleList[i]);
        }
        free(moduleList);
    }

    bool install_module(ModuleManager* manager, const char* moduleName, const char* repoUrl) {
        return manager->installModule(std::string(moduleName), std::string(repoUrl));
    }
}
