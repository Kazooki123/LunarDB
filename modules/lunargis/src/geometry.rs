use geo::{polygon, Point, Polygon, CoordsIter, Contains, Coord};
use geojson::{Feature, FeatureCollection, GeoJson, Geometry, Value};

pub struct GeometryFeature {
    pub polygon: Polygon<f64>,
}

impl GeometryFeature {
    pub fn new(coordinates: Vec<(f64, f64)>) -> Self {
        let exterior: Vec<Coord<f64>> = coordinates
            .into_iter()
            .map(|(x, y)| Coord { x, y })
            .collect();

        let polygon = polygon!{ exterior };
        GeometryFeature { polygon }
    }

    pub fn contains_point(&self, x: f64, y: f64) -> bool {
        let point = Point::new(x, y);
        self.polygon.contains(&point)
    }
}

pub fn features_to_geojson(features: Vec<GeometryFeature>) -> String {
    let geojson_features: Vec<Feature> = features
        .into_iter()
        .map(|feature| {
            let coords: Vec<Vec<f64>> = feature
                .polygon
                .exterior()
                .coords_iter()
                .map(|coord| vec![coord.x, coord.y])
                .collect();
            Feature {
                geometry: Some(Geometry::new(Value::Polygon(vec![coords]))),
                properties: None,
                bbox: None,
                id: None,
                foreign_members: None,
            }
        })
        .collect();

    let feature_collection = FeatureCollection {
        features: geojson_features,
        bbox: None,
        foreign_members: None,
    };

    GeoJson::FeatureCollection(feature_collection).to_string()
}
