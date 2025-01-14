#[derive(Debug, Clone, Copy, PartialEq)]
pub struct Point {
    pub x: f64,
    pub y: f64,
}

#[derive(Debug, Clone, PartialEq)]
pub struct Region {
    pub top_left: Point,
    pub bottom_right: Point,
}

#[derive(Debug, Clone, PartialEq)]
pub enum SpatialData {
    Point(Point),
    Region(Region),
}

#[derive(Debug)]
pub struct SpatialIndex {
    pub points: Vec<Point>,
    pub regions: Vec<Region>,
}

impl Point {
    pub fn new(x: f64, y: f64) -> Self {
        Point { x, y }
    }
}

impl Region {
    pub fn new(top_left: Point, bottom_right: Point) -> Self {
        Region { top_left, bottom_right }
    }

    pub fn contains(&self, point: &Point) -> bool {
        point.x >= self.top_left.x
            && point.x <= self.bottom_right.x
            && point.y <= self.top_left.y
            && point.y >= self.bottom_right.y
    }

    pub fn overlaps(&self, other: &Region) -> bool {
        !(self.bottom_right.x < other.top_left.x
            || self.top_left.x > other.bottom_right.x
            || self.bottom_right.y > other.top_left.y
            || self.top_left.y < other.bottom_right.y)
    }
}

impl SpatialData {
    pub fn is_point(&self) -> bool {
        matches!(self, SpatialData::Point(_))
    }

    pub fn is_region(&self) -> bool {
        matches!(self, SpatialData::Region(_))
    }
}

impl SpatialIndex {
    pub fn new() -> Self {
        SpatialIndex {
            points: Vec::new(),
            regions: Vec::new(),
        }
    }

    pub fn add_point(&mut self, point: Point) {
        self.points.push(point);
    }

    pub fn add_region(&mut self, region: Region) {
        self.regions.push(region);
    }

    pub fn find_points_in_region(&self, region: &Region) -> Vec<Point> {
        self.points
            .iter()
            .filter(|&point| region.contains(point))
            .cloned()
            .collect()
    }

    pub fn find_overlapping_regions(&self, target_region: &Region) -> Vec<Region> {
        self.regions
            .iter()
            .filter(|&region| region.overlaps(target_region))
            .cloned()
            .collect()
    }
}
