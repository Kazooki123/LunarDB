require 'ffi'
require 'fileutils'
require 'open-uri'

module LunarDB
  extend FFI::Library
  ffi_lib '../bin/lunardb_module_manager.dll'

  attach_function :create_module_manager, [], :pointer
  attach_function :destroy_module_manager, [:pointer], :void
  attach_function :add_module, [:pointer, :string], :bool
  attach_function :remove_module, [:pointer, :string], :bool
  attach_function :list_modules, [:pointer, :pointer], :pointer
  attach_function :free_module_list, [:pointer, :int], :void
  attach_function :install_module, [:pointer, :string, :string], :bool

  class ModuleManager
    def initialize
      @manager = LunarDB.create_module_manager
    end

    def finalize
      LunarDB.destroy_module_manager(@manager)
    end

    def get_module(module_name, repo_url)
      puts "Fetching module '#{module_name}' from #{repo_url}..."
      if LunarDB.install_module(@manager, module_name, repo_url)
        puts "Module '#{module_name}' installed successfully!"
      else
        puts "Error installing module '#{module_name}'"
      end
    end

    def list_modules
      count_ptr = FFI::MemoryPointer.new(:int)
      modules_ptr = LunarDB.list_modules(@manager, count_ptr)
      count = count_ptr.read_int
      modules = count.times.map { |i| modules_ptr[i].read_pointer.read_string }
      LunarDB.free_module_list(modules_ptr, count)
      puts "Installed modules: #{modules.join(', ')}"
    end
  end
end

# Example usage:
# manager = LunarDB::ModuleManager.new
# manager.get_module('LunarVector', 'https://github.com/Kazooki123/LunarDB/modules/lunarvector')
# manager.list_modules
# manager.finalize