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


#include "module.h"
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdlib>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>

namespace fs = std::filesystem;

// Callback function for cURL to write data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

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
        fs::remove_all(modulePath + moduleName);
        return true;
    }
    return false;
}

std::vector<std::string> ModuleManager::listModules() const {
    return loadedModules;
}

// What this does is it installs the modules from the LunarDB github repository then once it
// is installed, it then builds it using Cargo since LunarDB modules are built in Rust
// and then it embeds to the CLI to make it work (kind of)
bool ModuleManager::installModule(const std::string& moduleName, const std::string& repoUrl) {
    std::string moduleUrl = repoUrl + "/" + moduleName + ".tar.gz";
    std::string outputPath = modulePath + moduleName + ".tar.gz";

    CURL* curl = curl_easy_init();
    if (curl) {
        FILE* fp = fopen(outputPath.c_str(), "wb");
        if (!fp) {
            std::cerr << "Failed to open file for writing: " << outputPath << std::endl;
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, moduleUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        CURLcode res = curl_easy_perform(curl);
        fclose(fp);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "Failed to download module: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        // Extract the tar.gz file
        std::string extractCmd = "tar -xzf " + outputPath + " -C " + modulePath;
        int extractResult = system(extractCmd.c_str());
        if (extractResult != 0) {
            std::cerr << "Failed to extract module" << std::endl;
            return false;
        }

        // Remove the tar.gz file
        fs::remove(outputPath);

        // Build the Rust module
        std::string buildCmd = "cd " + modulePath + moduleName + " && cargo build --release";
        int buildResult = system(buildCmd.c_str());
        if (buildResult != 0) {
            std::cerr << "Failed to build module" << std::endl;
            return false;
        }

        // Adds the module to the list
        return addModule(moduleName);
    }

    return false;
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
