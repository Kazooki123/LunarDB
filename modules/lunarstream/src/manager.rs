use crate::stream::Stream;
use std::collections::HashMap;
use std::sync::{Arc, Mutex};

#[derive(Debug)]
pub struct Consumer {
    pub name: String,
}

#[derive(Debug)]
pub struct ConsumerGroup {
    pub name: String,
    pub consumers: Vec<Consumer>,
}

#[derive(Debug)]
pub struct StreamManager {
    streams: Arc<Mutex<HashMap<String, Stream>>>,
}

impl StreamManager {
    pub fn new() -> Self {
        Self {
            streams: Arc::new(Mutex::new(HashMap::new())),
        }
    }

    pub fn create_stream(&self, name: &str) -> bool {
        let mut streams = self.streams
            .lock()
            .unwrap();

        if streams.contains_key(name) {
            false
        } else {
            streams.insert(name.to_string(), Stream::new(name));
            true
        }
    }

    pub fn get_stream(&self, name: &str) -> Option<Stream> {
        let streams = self.streams
            .lock()
            .unwrap();
        streams.get(name).cloned()
    }

    pub fn delete_stream(&self, name: &str) -> bool {
        let mut streams = self.streams
            .lock()
            .unwrap();
        streams.remove(name).is_some()
    }
}
