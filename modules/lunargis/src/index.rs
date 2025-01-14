use rstar::{RTree, RTreeObject, AABB};

#[derive(Debug, Clone, Copy)]
pub struct SpatialPoint {
    pub x: f64,
    pub y: f64,
}

pub struct RTreeIndex {
    pub tree: RTree<SpatialPoint>,
}

impl RTreeObject for SpatialPoint {
    type Envelope = AABB<[f64; 2]>;

    fn envelope(&self) -> Self::Envelope {
        AABB::from_corners([self.x, self.y], [self.x, self.y])
    }
}

impl RTreeIndex {
    pub fn new() -> Self {
        RTreeIndex { tree: RTree::new() }
    }

    pub fn insert_point(&mut self, x: f64, y: f64) {
        self.tree.insert(SpatialPoint { x, y });
    }

    pub fn query_points_in_rectangle(
        &self,
        x1: f64,
        y1: f64,
        x2: f64,
        y2: f64,
    ) -> Vec<SpatialPoint> {
        let aabb = AABB::from_corners([x1, y1], [x2, y2]);
        self.tree.locate_in_envelope(&aabb).cloned().collect()
    }
}
