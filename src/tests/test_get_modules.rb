require_relative '../modules_get'

begin
  manager = LunarDB::ModuleManager.new
  puts "ModuleManager created successfully"
  
  # Test listing modules
  puts "Listing modules:"
  manager.list_modules
  
  # Test installing a module
  module_name = "TestModule"
  repo_url = "https://github.com/Kazooki123/LunarDB/modules/testmodule"
  puts "\nAttempting to install module: #{module_name}"
  manager.get_module(module_name, repo_url)
  
  # List modules again to verify installation
  puts "\nListing modules after installation:"
  manager.list_modules
  
  manager.finalize
  puts "ModuleManager finalized successfully"
rescue => e
  puts "An error occurred: #{e.message}"
  puts e.backtrace
end