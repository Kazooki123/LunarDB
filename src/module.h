#ifndef MODULE_H
#define MODULE_H

#include <string>
#include <vector>

class ModuleManager {
public:
    ModuleManager();
    bool addModule(const std::string& moduleName);
    std::vector<std::string> listModules() const;

private:
    std::string modulePath;
    std::vector<std::string> loadedModules;

    bool moduleExists(const std::string& moduleName) const;
    void loadExistingModules();
};

#endif // MODULE_H