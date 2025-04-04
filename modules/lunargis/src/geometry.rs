use geo::{Point, Polygon, LineString, Contains};
use geojson::{Feature, FeatureCollection, GeoJson, Geometry, Value};

pub struct GeometryFeature {
    pub polygon: Polygon<f64>,
}

impl GeometryFeature {
    pub fn new(coordinates: Vec<(f64, f64)>) -> Self {
        let line_string = LineString::from(coordinates);

        let polygon = Polygon::new(line_string, vec![]);

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
                .points()
                .map(|point| vec![point.x(), point.y()])
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