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

// Authors: Kazooki123, Starloexoliz

/*
** Copyright 2024 Kazooki123
**
** Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
** documentation files (the “Software”), to deal in the Software without restriction, including without
** limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
** of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following
** conditions:
**
** The above copyright notice and this permission notice shall be included in all copies or substantial
** portions of the Software.
**
** THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
** LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
** THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
**/

#endif // MODULE_H
