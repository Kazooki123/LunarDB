#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

class ModuleManager {
public:
    ModuleManager();
    bool addModule(const std::string& moduleName);
    bool removeModule(const std::string& moduleName);
    std::vector<std::string> listModules() const;
    bool installModule(const std::string& moduleName, const std::string& repoUrl);

private:
    std::string modulePath;
    std::vector<std::string> loadedModules;

    bool moduleExists(const std::string& moduleName) const;
    void loadExistingModules();
};

// C interface for FFI
extern "C" {
    ModuleManager* create_module_manager();
    void destroy_module_manager(ModuleManager* manager);
    bool add_module(ModuleManager* manager, const char* moduleName);
    bool remove_module(ModuleManager* manager, const char* moduleName);
    const char** list_modules(ModuleManager* manager, int* count);
    void free_module_list(const char** moduleList, int count);
    bool install_module(ModuleManager* manager, const char* moduleName, const char* repoUrl);
}

#endif // MODULE_H