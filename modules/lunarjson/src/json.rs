use serde_json::{Value, json};
use std::collections::HashMap;

pub struct LunarJSON {
    data: HashMap<String, Value>,
}

impl LunarJSON {
    pub fn new() -> Self {
        LunarJSON {
            data: HashMap::new(),
        }
    }

    pub fn set(&mut self, key: &str, json: Value) -> Result<(), String> {
        self.data.insert(key.to_string(), json);
        Ok(())
    }

    pub fn get(&self, key: &str) -> Option<&Value> {
        self.data.get(key)
    }

    pub fn get_field(&self, key: &str, path: &str) -> Option<&Value> {
        self.data.get(key).and_then(|value| {
            path.split('.').fold(Some(value), |acc, key| {
                acc.and_then(|v| v.get(key))
            })
        })
    }

    pub fn update_field(&mut self, key: &str, path: &str, new_value: Value) -> Result<(), String> {
        if let Some(json) = self.data.get_mut(key) {
            let mut current = json;
            let parts: Vec<&str> = path.split('.').collect();
            for (i, &part) in parts.iter().enumerate() {
                if i == parts.len() - 1 {
                    if let Value::Object(map) = current {
                        map.insert(part.to_string(), new_value);
                        return Ok(());
                    } else {
                        return Err("Invalid path".to_string());
                    }
                } else {
                    if let Some(next) = current.get_mut(part) {
                        current = next;
                    } else {
                        return Err("Path not found".to_string());
                    }
                }
            }
        }
        Err("Key not found".to_string())
    }

    pub fn delete_field(&mut self, key: &str, path: &str) -> Result<(), String> {
        if let Some(json) = self.data.get_mut(key) {
            let mut current = json;
            let parts: Vec<&str> = path.split('.').collect();
            for (i, &part) in parts.iter().enumerate() {
                if i == parts.len() - 1 {
                    if let Value::Object(map) = current {
                        map.remove(part);
                        return Ok(());
                    } else {
                        return Err("Invalid path".to_string());
                    }
                } else {
                    if let Some(next) = current.get_mut(part) {
                        current = next;
                    } else {
                        return Err("Path not found".to_string());
                    }
                }
            }
        }
        Err("Key not found".to_string())
    }

    pub fn push_to_array(&mut self, key: &str, path: &str, value: Value) -> Result<(), String> {
        if let Some(json) = self.data.get_mut(key) {
            if let Some(array) = self.get_mut_array(json, path) {
                array.push(value);
                return Ok(());
            }
        }
        Err("Array not found".to_string())
    }

    pub fn pop_from_array(&mut self, key: &str, path: &str) -> Result<Option<Value>, String> {
        if let Some(json) = self.data.get_mut(key) {
            if let Some(array) = self.get_mut_array(json, path) {
                return Ok(array.pop());
            }
        }
        Err("Array not found".to_string())
    }

    fn get_mut_array<'a>(&self, value: &'a mut Value, path: &str) -> Option<&'a mut Vec<Value>> {
        path.split('.').fold(Some(value), |acc, key| {
            acc.and_then(|v| v.get_mut(key))
        }).and_then(|v| v.as_array_mut())
    }
}

pub fn parse_json(s: &str) -> Result<Value, serde_json::Error> {
    serde_json::from_str(s)
}
