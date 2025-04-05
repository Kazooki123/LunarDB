use std::collections::{HashMap, VecDeque};
use std::hash::Hash;
use std::time::Instant;

pub struct Cache<K, V> {
  capacity: usize,
  map: HashMap<K, (V, usize)>,
  queue: VecDeque<K>,
  hits: usize,
  misses: usize,
  last_access_time: HashMap<K, Instant>,
}

impl <K, V> Cache<K, V> where K: Eq + Hash + Clone, V: Clone {
  pub fn new(capacity: usize) -> Self {
    Cache {
      capacity,
      map: HashMap::with_capacity(capacity),
      queue: VecDeque::with_capacity(capacity),
      hits: 0,
      misses: 0,
      last_access_time: HashMap::with_capacity(capacity),
    }
  }

  pub fn put(&mut self, key: K, value: V) {
    let idx = if let Some((_, old_idx)) = self.map.get(&key) {
        let idx = *old_idx;
        self.queue.remove(idx);
        self.queue.push_front(key.clone());
        0
    } else {
        if self.map.len() >= self.capacity {
            if let Some(old_key) = self.queue.pop_back() {
                self.map.remove(&old_key);
                self.last_access_time.remove(&old_key);
            }
        }
        self.queue.push_front(key.clone());
        0
    };

    self.map.insert(key.clone(), (value, idx));
    self.last_access_time.insert(key, Instant::now());
    self.update_indices();
  }

  pub fn get(&mut self, key: &K) -> Option<V> {
    if let Some((value, idx)) = self.map.get(key).map(|(v, i)| (v.clone(), *i)) {
        self.last_access_time.insert(key.clone(), Instant::now());
        self.queue.remove(idx);
        self.queue.push_front(key.clone());
        self.update_indices();
        self.hits += 1;
        Some(value)
    } else {
        self.misses += 1;
        None
    }
  }

  pub fn remove(&mut self, key: &K) -> Option<V> {
    if let Some((value, idx)) = self.map.remove(key) {
      self.queue.remove(*idx);
      self.last_access_time.remove(key);
      self.update_indices();
      return Some(value);
    }
    None
  }

  pub fn contains(&self, key: &K) -> bool {
    self.map.contains_key(key)
  }

  pub fn size(&self) -> usize {
    self.map.len()
  }

  pub fn capacity(&self) -> usize {
    self.capacity
  }

  pub fn clear(&mut self) {
    self.map.clear();
    self.queue.clear();
    self.last_access_time.clear();
  }

  pub fn hit_rate(&self) -> f64 {
    let total = self.hits + self.misses;
    if total == 0 {
      return 0.0;
    }
    (self.hits as f64) / (total as f64)
  }

  pub fn iter(&self) -> impl Iterator<Item = (&K, &V)> {
    self.map.iter().map(|(k, (v, _))| (k, v))
  }

  pub fn get_last_access_time(&self, key: &K) -> Option<Instant> {
    self.last_access_time.get(key).cloned()
  }

  fn move_to_front(&mut self, idx: usize) {
    if idx > 0 {
      let key = self.queue.remove(idx).unwrap();
      self.queue.push_front(key);
      self.update_indices();
    }
  }

  fn update_indices(&mut self) {
    for (i, key) in self.queue.iter().enumerate() {
      if let Some((_value, idx)) = self.map.get_mut(key) {
        *idx = i;
      }
    }
  }
}
