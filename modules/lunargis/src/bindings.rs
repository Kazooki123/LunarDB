use crate::spatial::SpatialDatabase;

#[no_mangle]
pub extern "C" fn create_spatial_database() -> *mut SpatialDatabase {
    Box::into_raw(Box::new(SpatialDatabase::new()))
    }

    #[no_mangle]
    pub extern "C" fn add_point(db: *mut SpatialDatabase, lat: f64, lon: f64) {
        let db = unsafe { &mut *db };
            db.add_point(lat, lon);
            }

            #[no_mangle]
            pub extern "C" fn find_nearest(db: *mut SpatialDatabase, lat: f64, lon: f64) -> GeoPoint {
                let db = unsafe { &mut *db };
                    db.find_nearest(lat, lon).unwrap_or_default()
                    }