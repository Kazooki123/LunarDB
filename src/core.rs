use std::collections::{HashMap, VecDeque};
use std::time::{SystemTime, UNIX_EPOCH};
use serde::{Serialize, Deserialize};
use serde_json;
use std::fmt;
use regex::Regex;
use crate::cache::Cache;
use std::sync::{Arc, Mutex};

#[derive(Clone, Debug, Serialize, Deserialize)]
pub enum DataType {
  String(String),
  List(VecDeque<String>),
}

impl fmt::Display for DataType {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    match self {
      DataType::String(s) => write!(f, "{}", s),
      DataType::List(list) => {
        write!(f, "[")?;
        for (i, item) in list.iter().enumerate() {
          if i > 0 {
            write!(f, ", ")?;
          }
          write!(f, "{}", item)?;
        }
        write!(f, "]")
      }
    }
  }
}

pub struct Core {
  cache: Arc<Mutex<Cache<String, (DataType, Option<u64>)>>>,
}

impl Core {
  pub fn new() -> Self {
    Core {
      cache: Arc::new(Mutex::new(Cache::new(10000))),
    }
  }

  pub fn serialize(&self) -> String {
    let cache = self.cache.lock().unwrap();
    let serialized_cache: HashMap<String, (DataType, Option<u64>)> = cache.iter().map(|(k, v)| (k.clone(), v.clone())).collect();
    serde_json::to_string(&serialized_cache).unwrap_or_else(|_| "{}".to_string())
  }

  pub fn deserialize(&mut self, data: &str) {
    let deserialized_cache: Result<HashMap<String, (DataType, Option<u64>)>, _> = serde_json::from_str(data);
    if let Ok(cache_data) = deserialized_cache {
        let mut cache = self.cache.lock().unwrap();
        cache.clear();
        for (key, value) in cache_data {
            cache.put(key, value);
        }
    }
  }

  pub fn set(&self, key: String, value: DataType) {
    let mut cache = self.cache.lock().unwrap();
    cache.put(key, (value, None));
  }

  pub fn set_ex(&self, key: String, value: DataType, expiry_seconds: u64) {
    let mut cache = self.cache.lock().unwrap();
    let expiry = self.current_time() + expiry_seconds;
    cache.put(key, (value, Some(expiry)));
  }

  pub fn get(&self, key: &str) -> Option<DataType> {
    let mut cache = self.cache.lock().unwrap();
    if let Some((value, expiry)) = cache.get(&key.to_string()) {
        if let Some(exp_time) = expiry {
            if self.current_time() >= *exp_time {
                cache.remove(&key.to_string());
                return None;
            }
        }
        return Some(value.clone());
    }
    None
  }

  pub fn del(&self, keys: &[String]) -> usize {
    let mut cache = self.cache.lock().unwrap();
    let mut count = 0;
    for key in keys {
        if cache.remove(&key.to_string()).is_some() {
            count += 1;
        }
    }
    count
  }

  pub fn mset(&self, key_values: HashMap<String, DataType>) {
    let mut cache = self.cache.lock().unwrap();

    for (key, value) in key_values {
      cache.put(key, (value, None));
    }
  }

  pub fn mget(&self, keys: &[String]) -> Vec<Option<DataType>> {
    let mut result = Vec::with_capacity(keys.len());
    let mut cache = self.cache.lock().unwrap();
    let current_time = self.current_time();

    for key in keys {
      if let Some((value, expiry)) = cache.get(key) {
        if let Some(exp_time) = expiry {
          if current_time >= *exp_time {
            result.push(None);
            continue;
          }
        }
        result.push(Some(value.clone()));
      } else {
        result.push(None);
      }
    }
    result
  }

  pub fn keys(&self, pattern: Option<&str>) -> Vec<String> {
    let cache = self.cache.lock().unwrap();
    let current_time = self.current_time();

    let mut result = Vec::new();
    let regex = if let Some(p) = pattern {
      match Regex::new(&p.replace("*", ".").replace("?", ".")) {
        Ok(r) => Some(r),
        Err(_) => return result,
      }
    } else {
      None
    };

    for (key, (_, expiry)) in cache.iter() {
      if let Some(exp_time) = expiry {
        if current_time >= *exp_time {
          continue;
        }
      }

      if let Some(ref re) = regex {
        if re.is_match(key) {
          result.push(key.clone());
        }
      } else {
        result.push(key.clone());
      }
    }
    result
  }

  pub fn clear(&self) -> usize {
    let mut cache = self.cache.lock().unwrap();
    let size = cache.size();
    cache.clear();
    size
  }

  pub fn size(&self) -> usize {
    let cache = self.cache.lock().unwrap();
    cache.size()
  }

  pub fn cleanup(&self) -> usize {
    let mut cache = self.cache.lock().unwrap();
    let current_time = self.current_time();
    let mut to_remove = Vec::new();

    for (key, (_, expiry)) in cache.iter() {
      if let Some(exp_time) = expiry {
        if current_time >= *exp_time {
          to_remove.push(key.clone());
        }
      }
    }

    let count = to_remove.len();
    for key in to_remove {
      cache.remove(&key);
    }

    count
  }

  pub fn lpush(&self, key: String, values: Vec<String>) -> usize {
    let mut cache = self.cache.lock().unwrap();

    let entry = match cache.get(&key) {
      Some((DataType::List(list), expiry)) => {
        let mut new_list = list.clone();
        for value in values.iter() {
          new_list.push_front(value.clone());
        }
        (DataType::List(new_list), expiry.clone())
      },
      Some(_) => {
        let mut list = VecDeque::new();
        for value in values.iter() {
          list.push_front(value.clone());
        }
        (DataType::List(list), None)
      },
      None => {
        let mut list = VecDeque::new();
        for value in values.iter() {
          list.push_front(value.clone());
        }
        (DataType::List(list), None)
      }
    };

    let list_size = match &entry.0 {
      DataType::List(list) => list.len(),
      _ => 0,
    };

    cache.put(key, entry);
    list_size
  }

  pub fn rpush(&self, key: String, values: Vec<String>) -> usize {
    let mut cache = self.cache.lock().unwrap();

    let entry = match cache.get(&key) {
      Some((DataType::List(list), expiry)) => {
        let mut new_list = list.clone();
        for value in values.iter() {
          new_list.push_back(value.clone());
        }
        (DataType::List(new_list), expiry.clone())
      },
      Some(_) => {
        let mut list = VecDeque::new();
        for value in values.iter() {
          list.push_back(value.clone());
        }
        (DataType::List(list), None)
      },
      None => {
        let mut list = VecDeque::new();
        for value in values.iter() {
          list.push_back(value.clone());
        }
        (DataType::List(list), None)
      }
    };

    let list_size = match &entry.0 {
      DataType::List(list) => list.len(),
      _ => 0,
    };

    cache.put(key, entry);
    list_size
  }

  pub fn lpop(&self, key: &str) -> Option<String> {
    let mut cache = self.cache.lock().unwrap();

    match cache.get(&key) {
      Some((DataType::List(list), expiry)) => {
        if list.is_empty() {
          return None;
        }

        let mut new_list = list.clone();
        let result = new_list.pop_front().unwrap();

        cache.put(key.to_string(), (DataType::List(new_list), expiry.clone()));

        Some(result)
      },
      _ => None,
    }
  }

  pub fn rpop(&self, key: &str) -> Option<String> {
    let mut cache = self.cache.lock().unwrap();

    match cache.get(&key) {
      Some((DataType::List(list), expiry)) => {
        if list.is_empty() {
          return None;
        }

        let mut new_list = list.clone();
        let result = new_list.pop_back().unwrap();

        cache.put(key.to_string(), (DataType::List(new_list), expiry.clone()));

        Some(result)
      },
      _ => None,
    }
  }

  pub fn lrange(&self, key: &str, start: i32, stop: i32) -> Option<Vec<String>> {
    let mut cache = self.cache.lock().unwrap();

    match cache.get(&key) {
      Some((DataType::List(list), _)) => {
        let len = list.len() as i32;
        if len == 0 {
          return Some(Vec::new());
        }

        let start_idx = if start < 0 { (len + start).max(0) } else { start.min(len - 1) } as usize;
        let stop_idx = if stop < 0 { (len + stop).max(0) } else { stop.min(len - 1) } as usize;

        if start_idx > stop_idx || start_idx >= len as usize {
          return Some(Vec::new());
        }

        let result: Vec<String> = list.iter()
            .skip(start_idx)
            .take(stop_idx - start_idx + 1)
            .cloned()
            .collect();

        Some(result)
      },
      _ => None,
    }
  }

  pub fn llen(&self, key: &str) -> usize {
    let mut cache = self.cache.lock().unwrap();

    match cache.get(&key) {
      Some((DataType::List(list), _)) => list.len(),
      _ => 0,
    }
  }

  fn current_time(&self) -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap()
        .as_secs()
  }
}

impl Clone for Core {
  fn clone(&self) -> Self {
    Core {
      cache: Arc::clone(&self.cache),
    }
  }
}
