const std = @import("std");

const NUM_OPERATIONS = 100000;

pub fn run(allocator: std.mem.Allocator) !void {
    var i: usize = 0;
    while (i < NUM_OPERATIONS) : (i += 1) {
        const key = try std.fmt.allocPrint(allocator, "key_{d}", .{i});
        defer allocator.free(key);
        const value = try std.fmt.allocPrint(allocator, "value_{d}", .{i});
        defer allocator.free(value);

        // Here you would call your LunarDB SET operation
        // For example: try db.set(key, value);
        
        // For now, we'll just print every 10000 operations
        if (i % 10000 == 0) {
            std.debug.print("Completed {d} SET operations\n", .{i});
        }
    }
    std.debug.print("Completed all {d} SET operations\n", .{NUM_OPERATIONS});
}