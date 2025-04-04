require 'open3'
require 'colorize'

SOURCE_DIR = "../../src/"

def scan_files(directory, extension = "**/*.rs")
  Dir.glob(File.join(directory, extension))
end

def analyze_file(file)
  puts "Analyzing: #{file}".blue
  cmd = "cppcheck --enable=all #{file}"

  stdout, stderr, status = Open3.capture3(cmd)
  if status.success?
    puts "No issues found in #{file}".green
  else
    puts "Issues found in #{file}".red
    puts stderr.yellow unless stderr.strip.empty?
    puts stdout unless stdout.strip.empty?
  end
end

cpp_files = scan_files(SOURCE_DIR)
if cpp_files.empty?
  puts "No Rust files found in #{SOURCE_DIR}".yellow
else
  cpp_files.each { |file| analyze_file(file) }
end
