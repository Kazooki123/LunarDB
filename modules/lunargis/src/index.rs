use rstar::{RTree, RTreeObject, AABB};

#[derive(Debug)]
pub struct GeoIndex {
    tree: RTree<GeoPoint>,
}

impl GeoIndex {
    pub fn new() -> Self {
        Self {
            tree: RTree::new(),
        }
    }

    pub fn insert(&mut self, point: GeoPoint) {
        self.tree.insert(point);
    }

    pub fn nearest_neighbor(&self, point: GeoPoint) -> Option<GeoPoint> {
        self.tree.nearest_neighbor(&point)
    }
}
