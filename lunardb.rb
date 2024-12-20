class Lunar < Formula
  desc "LunarDB: NoSQL, cache, in-memory key-value store database"
  homepage "https://github.com/Kazooki123/LunarDB"
  url "wget https://github.com/Kazooki123/LunarDB/archive/refs/tags/v0.1.6.tar.gz"
  sha256 "{{SHA256}}"
  license "MIT"

  depends_on "lua"
  depends_on "libpqxx"

  def install
    system "g++", "-std=c++17", "main.cpp", "cache.cpp", "concurrency.cpp", "saved.cpp", "sql.cpp", "module.cpp", "parser.cpp", "sharding.cpp", "hashing.cpp",
           "-I#{Formula["lua"].opt_include}/lua", "-llua", "-lpqxx", "-lpq", "-lcurl", "-lboost_system", "-pthread", "-o", "lunar"
    bin.install "lunar"
  end

  test do
    system "#{bin}/lunar"
  end
end
