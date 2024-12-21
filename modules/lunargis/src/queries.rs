use crate::spatial::{Point, Region};

pub fn find_points_in_region(points: &[Point], region: &Region) -> Vec<Point> {
  points
    .iter()
    .filter(|&point| region.contains(point))
    .cloned()
    .collect()
}

pub fn calculate_distance(point1: &Point, point2: &Point) -> f64 {
  let dx = point1.x - point2.x;
  let dy = point1.y - point2.y;
  (dx * dx + dy * dy).sqrt()
}

pub fn find_nearest_point(points: &[Point], target: &Point) -> Option<Point> {
  points
    .iter()
    .min_by(|a, b| {
      calculate_distance(a, target)
        .partial_cmp(&calculate_distance(b, target))
        .unwrap_or(std::cmp::Ordering::Equal)
    })
    .cloned()
}

pub fn filter_points<F>(points: &[Point], predicate: F) -> Vec<Point> where F: Fn(&Point) -> bool, {
  points
    .iter()
    .filter(|&point| predicate(point))
    .cloned()
    .collect()
}

pub fn find_overlapping_regions(regions: &[Region], target_region: &Region) -> Vec<Region> {
  regions
    .iter()
    .filter(|&region| region.overlaps(target_region))
    .cloned()
    .collect()
}
