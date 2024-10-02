use std::time::{Duration, Instant};

pub struct LunarVector {
    data: Vec<String>,
    expiration: Option<Instant>,
}

impl LunarVector {
    pub fn new() -> Self {
        LunarVector {
            data: Vec::new(),
            expiration: None,
        }
    }

    pub fn push(&mut self, value: String) {
        self.data.push(value);
    }

    pub fn pop(&mut self) -> Option<String> {
        self.data.pop()
    }

    pub fn get(&self, index: usize) -> Option<&String> {
        self.data.get(index)
    }

    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    pub fn set_ttl(&mut self, ttl: Duration) {
        self.expiration = Some(Instant::now() + ttl);
    }

    pub fn is_expired(&self) -> bool {
        self.expiration.map_or(false, |exp| Instant::now() > exp)
    }
}