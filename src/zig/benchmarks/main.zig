const std = @import("std");
const set_benchmark = @import("set_benchmark.zig");
const get_benchmark = @import("get_benchmark.zig");
const list_operations_benchmark = @import("list_operations_benchmark.zig");

pub fn main() !void {
    std.debug.print("Running LunarDB Benchmarks\n", .{});

    try set_benchmark.run();
    try get_benchmark.run();
    try list_operations_benchmark.run();

    std.debug.print("Benchmarks completed.\n", .{});
}
