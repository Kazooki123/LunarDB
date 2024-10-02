const std = @import("std");
const time = std.time;
const set_benchmark = @import("set_benchmark.zig");
const get_benchmark = @import("get_benchmark.zig");
const del_benchmark = @import("del_benchmark.zig");
const list_benchmark = @import("list_benchmark.zig");

pub fn main() !void {
    std.debug.print("Running LunarDB Benchmarks\n\n", .{});

    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // You'll need to initialize your LunarDB connection here
    // const db = try LunarDB.init(allocator);
    // defer db.deinit();

    try runBenchmark("SET Benchmark", set_benchmark.run, allocator);
    try runBenchmark("GET Benchmark", get_benchmark.run, allocator);
    try runBenchmark("DEL Benchmark", del_benchmark.run, allocator);
    try runBenchmark("List Operations Benchmark", list_benchmark.run, allocator);

    std.debug.print("\nAll benchmarks completed.\n", .{});
}

fn runBenchmark(name: []const u8, benchmarkFn: fn (std.mem.Allocator) anyerror!void, allocator: std.mem.Allocator) !void {
    std.debug.print("Running {s}...\n", .{name});
    const start = time.nanoTimestamp();
    try benchmarkFn(allocator);
    const end = time.nanoTimestamp();
    const duration = @as(f64, @floatFromInt(end - start)) / 1e9;
    std.debug.print("{s} completed in {d:.6} seconds\n\n", .{ name, duration });
}
