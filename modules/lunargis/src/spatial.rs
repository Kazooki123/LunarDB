use crate::geometry::GeoPoint;
use crate::index::GeoIndex;

/// Manages spatial data for LunarGIS.
pub struct SpatialDatabase {
    index: GeoIndex,
}

impl SpatialDatabase {
    pub fn new() -> Self {
        Self {
            index: GeoIndex::new(),
        }
    }

    pub fn add_point(&mut self, lat: f64, lon: f64) {
        let point = GeoPoint::new(lat, lon);
        self.index.insert(point);
    }

    pub fn find_nearest(&self, lat: f64, lon: f64) -> Option<GeoPoint> {
        let point = GeoPoint::new(lat, lon);
        self.index.nearest_neighbor(point)
    }
}
