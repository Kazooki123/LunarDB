use geo::{Point, LineString, Polygon};

/// Represents a 2D point with latitude and longitude.
pub type GeoPoint = Point<f64>;

/// Geometry utilities for handling shapes and points.
pub struct GeometryUtils;

impl GeometryUtils {
    pub fn create_point(lat: f64, lon: f64) -> GeoPoint {
            GeoPoint::new(lat, lon)
                }
                }